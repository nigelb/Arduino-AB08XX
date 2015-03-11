/*
 AdjustRTC is a sketch that lets you set the AB08XX Real Time
 Clock's time. Copyright (C) 2014 Nigel Bajema

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "Arduino.h"
#include "AB08XX_CLI.h"

AB08XX *abclock = NULL;
String *command = new String();

void prompt() {
	Serial.println();
	Serial.print("#> ");
}

#ifdef Arduino_h
void setup() {
	Serial.begin(115200);             // the Serial port of Arduino baud rate.
	delay(200);
	_abclock = new AB08XX_SPI(7);
	Serial.println();
	help(1, (char**) "");
	Serial.println();
	Serial.println("Enter \"help\" for list of available commands.");
	prompt();
}
#endif

void loop() {
	if (Serial.available()) {
		char read_v = (char) (Serial.read());

		if (read_v != 10 && read_v != 13) {
			if (read_v == 8 || read_v == 127) {
				if (command->length() > 0) {
					String *tmp = command;
					command = new String(
							command->substring(0, command->length() - 1));
					delete tmp;
					Serial.print("\b \b");
				}
			} else {
				command->concat(read_v);
				Serial.print(read_v);
			}
		} else {
			Serial.println();
			Serial.println();
			command->trim();
			unsigned int length = command->length();
			char _command[length + 1];
			int argc = 1;
			for (unsigned int i = 0; i < length; i++) {
				_command[i] = (*command)[i];
				if ((*command)[i] == ' ') {
					argc++;
					_command[i] = 0;
				}
			}

			_command[length] = 0;
			char* argv[argc];
			int pos = 0;
			for (int i = 0; i < argc; i++) {
				argv[i] = _command + pos;
				pos += strlen(argv[i]) + 1;
			}
			cmnd_t* to_run = get_cli(argv[0]);
			if (!to_run) {
				Serial.print("\t The command \"");
				Serial.print(argv[0]);
				Serial.println("\" could not be found.");

			}else
			{
				to_run->callback(argc, argv);
			}
			prompt();
			delete command;
			command = new String();
		}
	}
}

cmnd_t* find_cmnd(char* command, cmnd_t* command_vector) {
	for (int commands_pos = 0; command_vector[commands_pos].callback != NULL;
			commands_pos++) {
		if (strcmp(command_vector[commands_pos].command, command) == 0) {
			return command_vector + commands_pos;
		}
	}
	return NULL;
}

int set(int argc, char** argv) {
	ab08xx_tmElements_t time;
	if (argc != 8) {
		Serial.print("\tset command expected 7 parameters, found: ");
		Serial.println(argc - 1);
		Serial.println();
		set_help(argc, argv);
		return 2;
	}

	int count = 1;

	time.Year = CalendarYrToTm(atoi(argv[count++]));
	time.Month = atoi(argv[count++]);
	time.Day = atoi(argv[count++]);
	time.Hour = atoi(argv[count++]);
	time.Minute = atoi(argv[count++]);
	time.Second = atoi(argv[count++]);
	time.Wday = atoi(argv[count++]) + 1;

	abclock->write(time);

	Serial.print("\tSuccessfully set the clock. ");
	displayTime();
	return 0;
}

int get(int argc, char** argv) {
	Serial.print("\t");
	displayTime();
	return 0;
}

int set_help(int argc, char** argv) {
	ab08xx_tmElements_t time;
	abclock->read(time);
	Serial.println("Welcome to the AB08XX RTC Utility.");
	Serial.println("");
	Serial.println("Usage:");
	Serial.println("");
	Serial.println(
			"\tset <YEAR> <MONTH> <DAY> <HOUR24> <MINUTE> <SECOND> <DAY_OF_WEEK>");
	Serial.println("");
	Serial.println("\tExample:");
	Serial.println("");
	Serial.print("\t\t");

	displayTime(time);
	Serial.println("");
	Serial.println("\tExample Set Command:");
	Serial.println("");
	Serial.print("\t\tset ");
	Serial.print(tmYearToCalendar(time.Year));
	Serial.print(" ");
	Serial.print(time.Month);
	Serial.print(" ");
	Serial.print(time.Day);
	Serial.print(" ");
	Serial.print(time.Hour);
	Serial.print(" ");
	Serial.print(time.Minute);
	Serial.print(" ");
	Serial.print(time.Second);
	Serial.print(" ");
	Serial.print(time.Wday - 1);
	Serial.println("");
	Serial.println("");
	Serial.print("\t");
	print_days_of_week();
	Serial.println("");
	return 0;
}

int watch(int argc, char** argv) {
	ab08xx_tmElements_t time;
	uint16_t _delay = 1000;
	char* command_name = "get";

	int _argc = argc;
	char** _argv = argv;
	if(argc > 1)
	{
		_delay = atoi(argv[1]);
	}
	if (argc > 2) {
		_argc-=2;
		_argv+=2;
		command_name = argv[2];
	}
	cmnd_t* command = get_cli(command_name);

	Serial.print("The command will be displayed every ");
	Serial.print(_delay);
	Serial.print(" milliseconds until you press a key");
	Serial.println();
	if(command != NULL)
	{
		while (Serial.available() == 0) {
			abclock->read(time);
			command->callback(_argc, _argv);
			Serial.println();
			delay(_delay);
		}
	}else
	{
		Serial.print("Could not find command:");
		Serial.print(command_name);
		Serial.println();

	}


	Serial.read();
	return 0;
}

int stop(int argc, char** argv) {
	control1_t control;
	abclock->readControl1(control);
	control.STOP = true;
	abclock->writeControl1(control);
	return 0;
}

int start(int argc, char** argv) {
	control1_t control;
	abclock->readControl1(control);
	control.STOP = false;
	abclock->writeControl1(control);
	return 0;
}

int status(int argc, char** argv) {
	status_t status;
	abclock->readStatus(status);
	Serial.print("\tRegister Address:                 \t0x");
	LEADING_ZERO_ln(Serial, HEX, OFFSETOF(AB08XX_memorymap, status));
	Serial.println();
	Serial.print("\tCentury:                       CB:\t");
	Serial.println(status.CB);
	Serial.print("\tVBAT Power:                   BAT:\t");
	Serial.println(status.BAT);
	Serial.print("\tWatchdog Timer Enable:        WDT:\t");
	Serial.println(status.WDT);
	Serial.print("\tLow Battery:                   BL:\t");
	Serial.println(status.BL);
	Serial.print("\tCountdown Timer Enable:       TIM:\t");
	Serial.println(status.TIM);
	Serial.print("\tAlarm:                        ALM:\t");
	Serial.println(status.ALM);
	Serial.print("\tExternal 2:                   EX2:\t");
	Serial.println(status.EX2);
	Serial.print("\tExternal 1:                   EX1:\t");
	Serial.println(status.EX1);
	return 0;
}

int int_mask(int argc, char** argv) {
	inturrupt_mask_t mask;
	abclock->readInturruprMask(mask);
	bool display_usage = false;

	if (argc == 1) {
		Serial.print("\tRegister Address:                 \t0x");
		LEADING_ZERO_ln(Serial, HEX, OFFSETOF(AB08XX_memorymap, int_mask));
		Serial.println();
		Serial.print("\tCentury enable:                CEB:\t");
		Serial.println(mask.CEB);
		Serial.print("\tInturrupt mode (Table 17):      IM:\t");
		Serial.println(mask.IM);
		Serial.print("\tBattery low interrupt enable: BLIE:\t");
		Serial.println(mask.BLIE);
		Serial.print("\tTimer interrupt enable:        TIE:\t");
		Serial.println(mask.TIE);
		Serial.print("\tAlarm interrupt enable:        AIE:\t");
		Serial.println(mask.AIE);
		Serial.print("\tExternal Interrupt 2 enable:  EX2E:\t");
		Serial.println(mask.EX2E);
		Serial.print("\tExternal Interrupt 1 enable:  EX1E:\t");
		Serial.println(mask.EX1E);
		display_usage = true;
	} else if (argc >= 3) {
		for (int pos = 1; pos < (argc - 1); pos+=2) {
			upper(argv[pos]);
			Serial.print("\tSetting ");
			Serial.print(argv[pos]);
			Serial.print(" to: ");
			uint8_t value = strtol(argv[pos + 1], NULL, 16);
			Serial.print(value);

			if (strcmp(argv[pos], "CEB") == 0) {
				mask.CEB = value;
			} else if (strcmp(argv[pos], "IM") == 0) {
				mask.IM = value;
			} else if (strcmp(argv[pos], "BLIE") == 0) {
				mask.BLIE = value;
			} else if (strcmp(argv[pos], "TIE") == 0) {
				mask.TIE = value;
			} else if (strcmp(argv[pos], "AIE") == 0) {
				mask.AIE = value;
			} else if (strcmp(argv[pos], "EX2E") == 0) {
				mask.EX2E = value;
			} else if (strcmp(argv[pos], "EX1E") == 0) {
				mask.EX1E = value;
			}else
			{
				Serial.print(" : UNKNOWN ");
			}
			Serial.println();

		}
		Serial.println();
		abclock->writeInturruprMask(mask);
		int_mask(1, argv);
	}

	return 0;
}

int control1_mask(int argc, char** argv) {
	control1_t control1;
	abclock->readControl1(control1);
	if (argc == 1) {
		Serial.print("\tRegister Address:                 \t0x");
		LEADING_ZERO_ln(Serial, HEX, OFFSETOF(AB08XX_memorymap, control1));
		Serial.println();
		Serial.print("\tWrite RTC:                    WRTC:\t");
		Serial.println(control1.WRTC);
		Serial.print("\t????:                         PWR2:\t");
		Serial.println(control1.PWR2);
		Serial.print("\tAuto Reset Enable:            ARST:\t");
		Serial.println(control1.ARST);
		Serial.print("\t?????:                         RSP:\t");
		Serial.println(control1.RSP);
		Serial.print("\tOUT:                           OUT:\t");
		Serial.println(control1.OUT);
		Serial.print("\tOUTB:                         OUTB:\t");
		Serial.println(control1.OUTB);
		Serial.print("\t12 or 24 Hour Mode (0=24):   HMODE:\t");
		Serial.println(control1._12_24);
		Serial.print("\tOscillator Stop:              STOP:\t");
		Serial.println(control1.STOP);

	}else if (argc >= 3) {
		for (int pos = 1; pos < (argc - 1); pos+=2) {
				upper(argv[pos]);
				Serial.print("\tSetting ");
				Serial.print(argv[pos]);
				Serial.print(" to: ");
				uint8_t value = strtol(argv[pos + 1], NULL, 16);
				Serial.print(value);

				if (strcmp(argv[pos], "WRTC") == 0) {
					control1.WRTC = value;
				} else if (strcmp(argv[pos], "PWR2") == 0) {
					control1.PWR2 = value;
				} else if (strcmp(argv[pos], "ARST") == 0) {
					control1.ARST = value;
				} else if (strcmp(argv[pos], "RSP") == 0) {
					control1.RSP = value;
				} else if (strcmp(argv[pos], "OUT") == 0) {
					control1.OUT = value;
				} else if (strcmp(argv[pos], "OUTB") == 0) {
					control1.OUTB = value;
				} else if (strcmp(argv[pos], "HMODE") == 0) {
					control1._12_24 = value;
				} else if (strcmp(argv[pos], "STOP") == 0) {
					control1.STOP = value;
				}else
				{
					Serial.print(" : UNKNOWN ");
				}
				Serial.println();

			}
			Serial.println();
			abclock->writeControl1(control1);
			control1_mask(1, argv);
	}
	return 0;
}

int control2_mask(int argc, char** argv) {
	control2_t control2;
	abclock->readControl2(control2);
	if (argc == 1) {
		Serial.print("\tRegister Address:                 \t0x");
		LEADING_ZERO_ln(Serial, HEX, OFFSETOF(AB08XX_memorymap, control2));
		Serial.println();
		Serial.print("\tOutput 1 Select (Table 16):  OUT1S:\t");
		Serial.println(control2.OUT1S);
		Serial.print("\tOutput 2 Select (Table 15):  OUT2S:\t");
		Serial.println(control2.OUT2S);
		Serial.print("\t?????:                        RS1E:\t");
		Serial.println(control2.RS1E);
		Serial.print("\tOutput Push Pull (1=pushpull): TPP:\t");
		Serial.println(control2.OUTPP);
	}else if (argc >= 3) {
		for (int pos = 1; pos < (argc - 1); pos+=2) {
				upper(argv[pos]);
				Serial.print("\tSetting ");
				Serial.print(argv[pos]);
				Serial.print(" to: ");
				uint8_t value = strtol(argv[pos + 1], NULL, 16);
				Serial.print(value);

				if (strcmp(argv[pos], "OUT1S") == 0) {
					control2.OUT1S = value;
				} else if (strcmp(argv[pos], "OUT2S") == 0) {
					control2.OUT2S = value;
				} else if (strcmp(argv[pos], "RS1E") == 0) {
					control2.RS1E = value;
				} else if (strcmp(argv[pos], "OUTPP") == 0) {
					control2.OUTPP = value;
				}else
				{
					Serial.print(" : UNKNOWN ");
				}
				Serial.println();
			}
			Serial.println();
			abclock->writeControl2(control2);
			control2_mask(1, argv);
	}
	return 0;
}

int clock_id(int argc, char** argv)
{
	AB08XX_memorymap map;
	memset(&map, 0, sizeof map);
	uint8_t offset = OFFSETOF(AB08XX_memorymap, id);
	abclock->_read(offset, (uint8_t*)&map + offset, 7);
	Serial.println("Abracon Clock Identification:");
	Serial.println();
	Serial.print("\tModel:\n\t\tAB");
	Serial.print(bcd2bin(map.id.part_number_upper));
	Serial.println(bcd2bin(map.id.part_number_lower));
	Serial.print("\tRevision\n\t\tMajor: ");
	Serial.print(map.id.revision.MAJOR);
	Serial.print("\t\tMinor: ");
	Serial.println(map.id.revision.MINOR);
	Serial.println("\tManufacturing Information:");
	/*Serial.print("\t\tYear: ");
	Serial.println(map.id.date.YEAR);
	Serial.print("\t\tWeek Lower: ");
	Serial.println(map.id.date.WEEKL);
	Serial.print("\t\tWafer: ");
	Serial.println(map.id.info.WAFER);
	Serial.print("\t\tWeek Upper: ");
	Serial.println(map.id.info.WEEKU);
	Serial.print("\t\tLot: ");
	Serial.println(map.id.quad.LOT);
	Serial.print("\t\tQuadrant: ");
	Serial.println(map.id.quad.QUADRANT);
	Serial.print("\t\tserialization: ");*/


	Serial.print("\t\tYear: ");
	Serial.println(map.id.info.YEAR);
	Serial.print("\t\tWeek Lower: ");
	Serial.println(map.id.info.WEEKL);
	Serial.print("\t\tWafer: ");
	Serial.println(map.id.info.WAFER);
	Serial.print("\t\tWeek Upper: ");
	Serial.println(map.id.info.WEEKU);
	Serial.print("\t\tLot: ");
	Serial.println(map.id.info.LOT);
	Serial.print("\t\tQuadrant: ");
	Serial.println(map.id.info.QUADRANT);
	Serial.print("\t\tserialization: ");

	Serial.println(map.id.serialization);
	
}

int trickle(int argc, char** argv){
	AB08XX_memorymap map;
	memset(&map, 0, sizeof map);
	uint8_t offset = OFFSETOF(AB08XX_memorymap, trickle);
	abclock->_read(offset, (uint8_t*)&map + offset, 1);
	Serial.print("Trickle charge configuration:\n");
	Serial.print("\tResistor: ");
	Serial.println(map.trickle.resistor);
	Serial.print("\tDiode: ");
	Serial.println(map.trickle.diode);
	Serial.print("\tEnable: ");
	Serial.println(map.trickle.enable);
}

int hex_get(int argc, char** argv) {
	char str[10];

	uint8_t offset = 0;
	uint8_t size = sizeof(AB08XX_memorymap);

	if(argc > 1)
	{
		offset = strtol(argv[1], NULL, 16);
		size = 1;

	}

	if(argc > 2)
	{
		size = strtol(argv[2], NULL, 16);
		if(size + offset > sizeof(AB08XX_memorymap))
		{
			size = sizeof(AB08XX_memorymap) - offset;
		}
	}

	uint8_t buf[size];

	for (uint16_t i = 0; i < size; i++) {
		buf[i] = 0;
	}

	abclock->_read(offset, (uint8_t*)&buf, size);

	for (uint16_t i = 0; i < size; i++) {
		if (i % 8 == 0) {
			Serial.println();
			sprintf(str, "0x%02x: ", i + offset);
			Serial.print(str);
		}
		sprintf(str, "%02x ", buf[i]);
		Serial.print(str);
	}
	Serial.println();
	return 0;
}

int hex_set(int argc, char** argv)
{
	if (argc >= 3) {

			uint8_t address   = strtol(argv[1], NULL, 16);
			uint8_t size = argc - 2;
			uint8_t buf[size];
			Serial.print("\tSetting ");
			Serial.print(address, HEX);
			Serial.print(" to: ");

			for (int argc_pos = 2, buf_pos = 0; argc_pos < argc; argc_pos++, buf_pos++) {
				buf[buf_pos] = strtol(argv[argc_pos], NULL, 16);
				Serial.print(" 0x");
				Serial.print(buf[buf_pos], HEX);
			}
			Serial.println();
			abclock->_write(address, (uint8_t*)buf, size);
	}else
	{
		Serial.print("Expected at least two parameters, got: ");
		Serial.println(argc - 1);
		Serial.println();
		help(argc, argv);
	}
	return 0;
}

int bin_get(int argc, char** argv) {
	char str[10];
	uint8_t offset = 0;
	uint8_t size = sizeof(AB08XX_memorymap);

	if(argc > 1)
	{
		offset = strtol(argv[1], NULL, 16);
		size = 1;

	}

	if(argc > 2)
	{
		size = strtol(argv[2], NULL, 16);
		if(size + offset > sizeof(AB08XX_memorymap))
		{
			size = sizeof(AB08XX_memorymap) - offset;
		}
	}

	uint8_t buf[size];

	for (uint16_t i = 0; i < size; i++) {
		buf[i] = 0;
	}

	abclock->_read(offset, (uint8_t*)&buf, size);
	for (uint16_t i = 0; i < size; i++) {

		Serial.println();
		sprintf(str, "%02x: ", i + offset);
		Serial.print(str);

		sprintf(str, "%02x ", buf[i]);
		Serial.print(buf[i], BIN);
	}
	Serial.println();
	return 0;
}

int map(int argc, char** argv)
{
	for(int i = 0; register_map[i].register_name != NULL; i++)
	{
		Serial.print("\t");
		Serial.print(register_map[i].register_name);
		int8_t sz = strlen(register_map[i].register_name);
		PRINT_WIDTH(Serial, 18-sz, " ");
		Serial.print(": 0x");
		LEADING_ZERO_ln(Serial,HEX, register_map[i].offset);
	}
	return 0;
}

int _help(int argc, char** argv, cmnd_t* command_vecter) {
	if (argc == 1) {
		Serial.println("Help: ");
		for (int i = 0; command_vecter[i].callback != NULL; i++) {
			Serial.print("\t");
			Serial.print(command_vecter[i].command);
			PRINT_WIDTH(Serial, 10 - strlen(command_vecter[i].command), " ");
			Serial.print(command_vecter[i].description);
			Serial.println();
		}
		Serial.println();
		Serial.println("Command specific help:");
		Serial.println();
		Serial.println("\thelp <command>");
	} else {
		bool found = false;
		for (int i = 0; command_vecter[i].callback != NULL; i++) {
			if (strcmp(command_vecter[i].command, argv[1]) == 0 ) {
					if(command_vecter[i].help != NULL)
					{
						command_vecter[i].help(argc, argv);
					}else
					{
						Serial.print("\t");
						Serial.println(command_vecter[i].description);
					}
					found = true;
					break;
			}
	}
		if (!found) {
			Serial.print("Could not find help for: ");
			Serial.println(argv[1]);
		}
	}
	return 0;
}

int help(int argc, char** argv) {
	return _help(argc, argv, (cmnd_t*) &cli_vector);
}

int alarm(int argc, char** argv) {
	cmnd_t* command;
	char* command_name;
	if (argc == 1) {
		alarm_get(argc, argv);
		_help(argc, argv, (cmnd_t*) &alarm_vector);
	} else {
		command_name = argv[1];
		command = find_cmnd(command_name, (cmnd_t*) &alarm_vector);
		if (command != NULL) {
			command->callback(argc, argv);
		} else {
			Serial.print("Command ");
			Serial.print(command_name);
			Serial.println(" Not found.");
			Serial.println();
			alarm_help(argc, argv);
		}
	}
	return 0;
}

int alarm_set(int argc, char** argv) {
	if(argc == 10)
	{
		ab08xx_tmElements_t time;
		ab08xx_alarm_repeat_mode_t mode;
		int count = 2;
		time.Year = 0;
		time.Month = atoi(argv[count++]);
		time.Day = atoi(argv[count++]);
		time.Hour = atoi(argv[count++]);
		time.Minute = atoi(argv[count++]);
		time.Second = atoi(argv[count++]);
		time.Hundreths = atoi(argv[count++]);
		time.Wday = atoi(argv[count++]) + 1;
		uint8_t _mode = atoi(argv[count++]);
		mode = invalid_mode;
		if(_mode < get_alarm_mode_id(invalid_mode))
		{
			mode = alarm_repeat_map[_mode].mode;
		}
		abclock->writeAlarm(time, mode);
		Serial.println();
		abclock->readAlarm(time, mode);
		Serial.print("\t");
		alarm_get(argc, argv);
	}else
	{
		Serial.println("Invalid command.");
		Serial.println();
		alarm_help_set(argc, argv);
	}
	return 0;
}

int alarm_get(int argc, char** argv) {
	ab08xx_tmElements_t tm;
	ab08xx_alarm_repeat_mode_t mode;
	abclock->readAlarm(tm, mode);
	Serial.print("\t");
	print_alarm_element(tm);
	Serial.print(" ");
	Serial.println(get_alarm_mode(mode)->name);

	return 0;
}
int alarm_enable(int argc, char** argv) {
	return 0;
}
int alarm_dissable(int argc, char** argv) {
	return 0;
}

int alarm_help(int argc, char** argv) {
	_help(argc - 1, argv + 1, (cmnd_t*) &alarm_vector);
	return 0;

}

int alarm_help_set(int argc, char** argv) {
	ab08xx_tmElements_t alarm;
	ab08xx_alarm_repeat_mode_t mode;
	abclock->readAlarm(alarm, mode);
	Serial.println("Alarm set usage: ");
	Serial.println();
	Serial.println("\talarm set <MONTH> <DAY> <HOUR24> <MINUTE> <SECOND> <HUNDRETHS_OF_SECONDS> <DAY_OF_WEEK> <REPEAT_MODE>");
	Serial.println();
	Serial.println("\tExample: ");
	Serial.println();
	Serial.print("\t\t");
	print_alarm_element(alarm);
	Serial.print(" ");
	Serial.println(get_alarm_mode(mode)->name);
	Serial.println();
	Serial.println("\tExample Alarm Set command: ");
	Serial.println();
	Serial.print("\t\t alarm set ");
	Serial.print(alarm.Month);
	Serial.print(" ");
	Serial.print(alarm.Day);
	Serial.print(" ");
	Serial.print(alarm.Hour);
	Serial.print(" ");
	Serial.print(alarm.Minute);
	Serial.print(" ");
	Serial.print(alarm.Second);
	Serial.print(" ");
	Serial.print(alarm.Hundreths);
	Serial.print(" ");
	Serial.print(alarm.Wday - 1);
	Serial.print(" ");
	Serial.println(get_alarm_mode_id(mode));
	Serial.println();
	Serial.print("\t");
	print_days_of_week();
	Serial.println();
	Serial.print("\t");
	print_alarm_mode();
	return 0;
}

void displayTime() {
	ab08xx_tmElements_t time;
	abclock->read(time);
	displayTime(time);
}

void displayTime(ab08xx_tmElements_t &time) {
	Serial.print("");
	Serial.print("RTC's Current Time: ");
	print_time_element(time);

}
void print_time_element(ab08xx_tmElements_t &time) {
	Serial.print(tmYearToCalendar(time.Year), DEC);
	Serial.print('/');
	if (time.Month < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Month, DEC);
	Serial.print('/');
	if (time.Day < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Day, DEC);
	Serial.print(' ');
	if (time.Hour < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Hour, DEC);
	Serial.print(':');
	if (time.Minute < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Minute, DEC);
	Serial.print(':');
	if (time.Second < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Second, DEC);
	Serial.print(" ");
	if (time.Hundreths < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Hundreths, DEC);
	Serial.print(" ");
	Serial.print(week_day[time.Wday - 1]);
	Serial.println();
}

void print_alarm_element(ab08xx_tmElements_t &time) {
	Serial.print("RTC's Current Alarm: ");
	if (time.Month < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Month, DEC);
	Serial.print('/');
	if (time.Day < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Day, DEC);
	Serial.print(' ');
	if (time.Hour < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Hour, DEC);
	Serial.print(':');
	if (time.Minute < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Minute, DEC);
	Serial.print(':');
	if (time.Second < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Second, DEC);
	Serial.print(" ");
	if (time.Hundreths < 10) {
		Serial.print(0, DEC);
	}
	Serial.print(time.Hundreths, DEC);
	Serial.print(" ");
	Serial.print(week_day[time.Wday - 1]);
}

void print_alarm_mode()
{
	Serial.println("Alarm Modes:");
	Serial.println("");
	for(int i = 0; alarm_repeat_map[i].mode != invalid_mode; i++)
	{
		Serial.print("\t\t");
		Serial.print(alarm_repeat_map[i].mode, DEC);
		Serial.print(": ");
		Serial.println(alarm_repeat_map[i].name);

	}
}
void print_days_of_week()
{
	Serial.println("Days of Week:");
	Serial.println("");
	for (int i = 0; i < 7; i++) {
		Serial.print("\t\t");
		Serial.print(i, DEC);
		Serial.print(": ");
		Serial.println(week_day[i]);
	}
}

alarm_repeat_map_t* get_alarm_mode(ab08xx_alarm_repeat_mode_t to_get)
{
	return &(alarm_repeat_map[get_alarm_mode_id(to_get)]);
}

int get_alarm_mode_id(ab08xx_alarm_repeat_mode_t to_get)
{
	int i = 0;
	for(; alarm_repeat_map[i].mode != invalid_mode; i++)
	{
		if(alarm_repeat_map[i].mode == to_get)
		{
			return i;
		}
	}
	return i;
}

cmnd_t* get_cli(char* command)
{
	for (int commands_pos = 0; cli_vector[commands_pos].callback != NULL; commands_pos++) {
		if (strcmp(cli_vector[commands_pos].command, command) == 0) {
			return &(cli_vector[commands_pos]);
		}
	}
	return NULL;
}

char* upper(char* value) {
	for (uint8_t i = 0; i < (uint8_t) strlen(value); i++) {
		value[i] = toupper(value[i]);
	}
	return value;
}

