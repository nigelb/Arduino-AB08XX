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
#include "AB08XX.h"

#define LEADING_ZERO(_Stream,_Base,_Value)   if(_Value < _Base){_Stream.print(0);}_Stream.print(_Value, _Base)
#define LEADING_ZERO_ln(_Stream,_Base,_Value)   LEADING_ZERO(_Stream,_Base,_Value);_Stream.println()
#define PRINT_WIDTH(__stream, __size, __pad) for(int8_t j = 0; j < __size; j++){__stream.print(__pad);}

char weekDay[][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

struct cmnd_t {
	char* command;
	int (*callback)(int argc, char** argv);
	char* description;
	int (*help)(int argc, char** argv);
};

struct register_map_t
{
	char* register_name;
	uint8_t offset;
};

cmnd_t* find_cmnd(char* command, cmnd_t* command_vector);

int set(int argc, char** argv);
int get(int argc, char** argv);
int alarm(int argc, char** argv);
int usage(int argc, char** argv);
int watch(int argc, char** argv);
int stop(int argc, char** argv);
int start(int argc, char** argv);
int status(int argc, char** argv);
int int_mask(int argc, char** argv);
int hex_get(int argc, char** argv);
int hex_set(int argc, char** argv);
int bin_get(int argc, char** argv);
int map(int argc, char** argv);
int _help(int argc, char** argv, cmnd_t* command_vecter);
int help(int argc, char** argv);

int alarm_set(int argc, char** argv);
int alarm_get(int argc, char** argv);
int alarm_enable(int argc, char** argv);
int alarm_dissable(int argc, char** argv);
int alarm_help(int argc, char** argv);
int alarm_help_set(int argc, char** argv);

void displayTime();
void displayTime(ab08xx_tmElements_t &time);
void print_time_element(ab08xx_tmElements_t &time);
void print_alarm_element(ab08xx_tmElements_t &time);

char* upper(char* value);

static cmnd_t cli_vector[] = {
		{ "set",      &set, "Set the clock.", &usage },
		{ "get",      &get, "Retrieve and display the time from the clock.", NULL },
		{ "alarm",    &alarm, "Retrieve and display the alarm information from the clock.", &alarm_help },
		{ "watch",    &watch, "Watch the time on the clock.", NULL },
		{ "usage",    &usage, "Display the usage of the set command.", NULL },
		{ "stop",     &stop, "Stop the oscillator.", NULL },
		{ "start",    &start, "Start the oscillator.", NULL },
		{ "status",   &status, "Display the status register.", NULL },
		{ "int",      &int_mask, "Display the Interrupt Mask register.", NULL },
		{ "binget",   &bin_get, "Display the contents of the clocks memory in Binary:\r\n\r\n\t\t\t\tbinget [offset[, count]]\r\n", NULL },
		{ "hexget",   &hex_get, "Display the contents of the clocks memory in Hex:\r\n\r\n\t\t\t\thexget [offset[, count]]\r\n", NULL },
		{ "hexset",   &hex_set, "Write data to the clock's memory in Hex (all values are read as hex):\r\n\r\n\t\t\t\thexset offset val1 [val2] [val3] [val4]\r\n", NULL },
		{ "map",      &map,   "Display register map.", NULL },
		{ "help",     &help,   "List available commands.", NULL },

		{ NULL, NULL, NULL }
};

static cmnd_t alarm_vector[] = {
		{ "set", &alarm_set, "Set the alarm.", &alarm_help_set },
		{ "get", &alarm_get, "Retrieve and display the alarm from the clock.", NULL },
		{ "enable", &alarm_enable, "Enable the alarm.", NULL },
		{ "disable", &alarm_dissable, "Disable the alarm.", NULL },
		{ "help", &alarm_help, "Display usage for the alarm command.", NULL },

		{ NULL, NULL, NULL }
};

struct register_map_t register_map[] = {
		{"time", OFFSETOF(AB08XX_memorymap,time)},
		{"alarm", OFFSETOF(AB08XX_memorymap,alarm)},
		{"status", OFFSETOF(AB08XX_memorymap,status)},
		{"control1", OFFSETOF(AB08XX_memorymap,control1)},
		{"control2", OFFSETOF(AB08XX_memorymap,control2)},
		{"int_mask", OFFSETOF(AB08XX_memorymap,int_mask)},
		{"sqw", OFFSETOF(AB08XX_memorymap,sqw)},
		{"cal_xt", OFFSETOF(AB08XX_memorymap,cal_xt)},
		{"cal_rc_hi", OFFSETOF(AB08XX_memorymap,cal_rc_hi)},
		{"cal_rc_low", OFFSETOF(AB08XX_memorymap,cal_rc_low)},
		{"sleep_control", OFFSETOF(AB08XX_memorymap,sleep_control)},
		{"timer_control", OFFSETOF(AB08XX_memorymap,timer_control)},
		{"timer", OFFSETOF(AB08XX_memorymap,timer)},
		{"timer_initial", OFFSETOF(AB08XX_memorymap,timer_initial)},
		{"wdt", OFFSETOF(AB08XX_memorymap,wdt)},
		{"osc_control", OFFSETOF(AB08XX_memorymap,osc_control)},
		{"osc_status", OFFSETOF(AB08XX_memorymap,osc_status)},
		{"RESERVED", OFFSETOF(AB08XX_memorymap,RESERVED)},
		{"configuration_key", OFFSETOF(AB08XX_memorymap,configuration_key)},
		{"trickle", OFFSETOF(AB08XX_memorymap,trickle)},
		{"bref_control", OFFSETOF(AB08XX_memorymap,bref_control)},
		{"RESERVED2", OFFSETOF(AB08XX_memorymap,RESERVED2)},
		{"id0", OFFSETOF(AB08XX_memorymap,id0)},
		{"id1", OFFSETOF(AB08XX_memorymap,id1)},
		{"id2", OFFSETOF(AB08XX_memorymap,id2)},
		{"id3", OFFSETOF(AB08XX_memorymap,id3)},
		{"id4", OFFSETOF(AB08XX_memorymap,id4)},
		{"id5", OFFSETOF(AB08XX_memorymap,id5)},
		{"id6", OFFSETOF(AB08XX_memorymap,id6)},
		{"astat", OFFSETOF(AB08XX_memorymap,astat)},
		{"octrl", OFFSETOF(AB08XX_memorymap,octrl)},
		{"RESERVED3", OFFSETOF(AB08XX_memorymap,RESERVED3)},
		{"extention_address", OFFSETOF(AB08XX_memorymap,extention_address)},
		{"ram", OFFSETOF(AB08XX_memorymap,ram)},

		{NULL, NULL}

};

AB08XX_I2C clock;
String *command;

void prompt() {
	Serial.println();
	Serial.print("#> ");
}

void setup() {
	Serial.begin(115200);             // the Serial port of Arduino baud rate.
	delay(200);

	Serial.println();
	command = new String();
	usage(0, (char**) "");
	Serial.println("Enter \"help\" for list of available commands.");
	prompt();
}

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
			bool found = false;
			for (int commands_pos = 0;
					cli_vector[commands_pos].callback != NULL; commands_pos++) {
				if (strcmp(cli_vector[commands_pos].command, argv[0]) == 0) {
					found = true;
					cli_vector[commands_pos].callback(argc, argv);
					prompt();
				}
			}
			if (!found) {
				Serial.print("\t The command \"");
				Serial.print(argv[0]);
				Serial.println("\" could not be found.");
				prompt();
			}
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
		usage(argc, argv);
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

	clock.write(time);

	Serial.print("\tSuccessfully set the clock. ");
	displayTime();
	return 0;
}

int get(int argc, char** argv) {
	Serial.print("\t");
	displayTime();
	return 0;
}

int usage(int argc, char** argv) {
	ab08xx_tmElements_t time;
	clock.read(time);
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
	Serial.println("\tDays of Week:");
	Serial.println("");
	for (int i = 0; i < 7; i++) {
		Serial.print("\t\t");
		Serial.print(i, DEC);
		Serial.print(": ");
		Serial.println(weekDay[i]);
	}
	Serial.println("");
	return 0;
}

int watch(int argc, char** argv) {
	ab08xx_tmElements_t time;
	Serial.println(
			"The time will be displayed every second until you press a key");
	Serial.println();
	while (Serial.available() == 0) {
		clock.read(time);
		displayTime();
		if (argc > 1 && atoi(argv[1]) > 0) {
			delay(atoi(argv[1]));
		} else {
			delay(1000);
		}
	}
	Serial.read();
	return 0;
}

int stop(int argc, char** argv) {
	control1_t control;
	clock.readControl1(control);
	control.STOP = true;
	clock.writeControl1(control);
	return 0;
}

int start(int argc, char** argv) {
	control1_t control;
	clock.readControl1(control);
	control.STOP = false;
	clock.writeControl1(control);
	return 0;
}

int status(int argc, char** argv) {
	status_t status;
	clock.readStatus(status);
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
	Serial.println(status.BL);
	Serial.print("\tAlarm:                        ALM:\t");
	Serial.println(status.TIM);
	Serial.print("\tExternal 2:                   EX2:\t");
	Serial.println(status.EX2);
	Serial.print("\tExternal 1:                   EX1:\t");
	Serial.println(status.EX1);

}

int int_mask(int argc, char** argv) {
	inturrupt_mask_t mask;
	clock.readInturruprMask(mask);
	bool display_usage = false;

	if (argc == 1) {
		Serial.print("\tRegister Address:                 \t0x");
		LEADING_ZERO_ln(Serial, HEX, OFFSETOF(AB08XX_memorymap, int_mask));
		Serial.println();
		Serial.print("\tCentury enable:                CEB:\t");
		Serial.println(mask.CEB);
		Serial.print("\tInturrupt mode:                 IM:\t");
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
//			Serial.print(argv[pos + 1]); Serial.print(" ");
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
		clock.writeInturruprMask(mask);
		int_mask(1, argv);
	}

	return 0;
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

	clock._read(offset, (uint8_t*)&buf, size);

	for (uint16_t i = 0; i < size; i++) {
		if (i % 8 == 0) {
			Serial.println();
			sprintf(str, "0x%02x: ", i + offset);
			Serial.print(str);
		}
		sprintf(str, "%02x ", buf[i]);
		Serial.print(str);
	}
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
			clock._write(address, (uint8_t*)buf, size);
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
	AB08XX_memorymap mem;
	uint8_t *buf = (uint8_t *) &mem;

	for (uint16_t i = 0; i < sizeof(AB08XX_memorymap); i++) {
		buf[i] = 0;
	}
	clock.read(mem);
	for (uint16_t i = 0; i < sizeof(AB08XX_memorymap); i++) {

		Serial.println();
		sprintf(str, "%02x: ", i);
		Serial.print(str);

		sprintf(str, "%02x ", buf[i]);
		Serial.print(buf[i], BIN);
	}
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
			Serial.print("\t\t");
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
			if (strcmp(command_vecter[i].command, argv[1])
					== 0&& command_vecter[i].help != NULL) {command_vecter[i].help(argc, argv);
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
	_help(argc, argv, (cmnd_t*) &cli_vector);
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
}

int alarm_set(int argc, char** argv) {
	ab08xx_tmElements_t time;
	int count = 1;
	time.Month = atoi(argv[count++]);
	time.Day = atoi(argv[count++]);
	time.Hour = atoi(argv[count++]);
	time.Minute = atoi(argv[count++]);
	time.Second = atoi(argv[count++]);
	time.Wday = atoi(argv[count++]) + 1;
	return 0;
}

int alarm_get(int argc, char** argv) {
	ab08xx_tmElements_t tm;
	ab08xx_alarm_repeat_mode_t mode;
	clock.readAlarm(tm, mode);
	Serial.println("\tCurrent Alarm: ");
	Serial.print("\t\t");
	print_alarm_element(tm);

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
	Serial.println("**********************************");
	return 0;
}

void displayTime() {
	ab08xx_tmElements_t time;
	clock.read(time);
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
	Serial.print(weekDay[time.Wday - 1]);
	Serial.println();
}

void print_alarm_element(ab08xx_tmElements_t &time) {
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
	Serial.print(weekDay[time.Wday - 1]);
	Serial.println();
}

char* upper(char* value) {
	for (uint8_t i = 0; i < (uint8_t) strlen(value); i++) {
		value[i] = toupper(value[i]);
	}
	return value;
}

