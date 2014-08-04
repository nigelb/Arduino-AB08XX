/*
 AdjustRTC is a sketch that lets you set the AB08XX Real Time
 Clock's time. Copyright (C) 2012 Nigel Bajema

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


AB08XX clock;
tmElements_t time;
String *command;

char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

int set(int argc, char** argv);
int get(int argc, char** argv);
int usage(int argc, char** argv);
int help(int argc, char** argv);
int watch(int argc, char** argv);

void  displayTime ();



struct cmnd
{
	char* command;
	int (*callback)(int argc, char** argv);
	char* description;
};

static cmnd cli_vector[] = {
		{"set",   &set,   "Set the clock."},
		{"get",   &get,   "Retrieve and display the time from the clock."},
		{"watch", &watch, "Watch the time on the clock."},
		{"usage", &usage, "Display the usage of the set command."},
		{"help", &help,   "Display this help."},
		{NULL,   NULL,    NULL},
};

void prompt()
{
	Serial.println();
	Serial.print("#> ");
}

void setup()
{
  Serial.begin(115200);             // the Serial port of Arduino baud rate.
  delay(200);
  command = new String();
  Serial.println();
  usage(0, (char**)"");
  prompt();
}



void loop()
{
	if(Serial.available())
	{
		char read_v = (char)(Serial.read());
		if(read_v != 10 && read_v != 13)
		{
			if(read_v == 127){
				if(command->length() > 0){
					String *tmp = command;
					command = new String(command->substring(0, command->length()-1));
					delete tmp;
					Serial.print("\b \b");
				}
			}else{
				command->concat(read_v);
				Serial.print(read_v);
			}
		}else
		{
			Serial.println();
			Serial.println();
			command->trim();
			unsigned int length = command->length();
			char _command[length + 1];
			int argc = 1;
			for(unsigned int i = 0; i < length; i++)
			{
				_command[i] = (*command)[i];
				if((*command)[i] == ' ')
				{
					argc++;
					_command[i] = 0;
				}
			}

			_command[length] = 0;
			char* argv[argc];
			int pos = 0;
			for (int i = 0; i < argc; i++)
			{
				argv[i] = _command + pos;
				pos += strlen(argv[i]) + 1;
			}
			bool found = false;
			for(int commands_pos = 0; cli_vector[commands_pos].callback != NULL; commands_pos++)
			{
				if(strcmp(cli_vector[commands_pos].command, argv[0]) == 0)
				{
					found = true;
					cli_vector[commands_pos].callback(argc, argv);
					prompt();
				}
			}
			if(!found)
			{
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

int set(int argc, char** argv)
{
	if(argc != 8)
	{
		Serial.print("\tset command expected 7 parameters, found: ");
		Serial.println(argc -1);
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

int get(int argc, char** argv)
{
	Serial.print("\t");
	displayTime();
	return 0;
}

int usage(int argc, char** argv){
	  Serial.println("Welcome to the RTC Time Setting Utility.");
	  Serial.println("");
	  Serial.println("Usage:");
	  Serial.println("");
	  Serial.println("\tset <YEAR> <MONTH> <DAY> <HOUR24> <MINUTE> <SECOND> <DAY_OF_WEEK>");
	  Serial.println("");
	  Serial.println("\tExample:");
	  Serial.println("");
	  Serial.print("\t\t");

	  displayTime();
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
	  for(int i = 0; i < 7; i++)
	  {
	      Serial.print("\t\t");
	      Serial.print(i, DEC);
	      Serial.print(": ");
	      Serial.println(weekDay[i]);
	  }
	  Serial.println("");
}

int watch(int argc, char** argv)
{
	while(1 == 1)
	{
		clock.read(time);
		displayTime();
		delay(1000);
	}
	return 0;
}

int help(int argc, char** argv){
	Serial.println("Help: ");
	for(int i = 0; cli_vector[i].callback != NULL; i++)
	{
		Serial.print("\t");
		Serial.print(cli_vector[i].command);
		Serial.print("\t");
		Serial.print(cli_vector[i].description);
		Serial.println();
	}
}

void  displayTime ()
{
    clock.read(time);
    Serial.print("");
    Serial.print("RTC's Current Time: ");
    Serial.print(tmYearToCalendar(time.Year), DEC);
    Serial.print('/');
    if(time.Month < 10){Serial.print(0, DEC);}
    Serial.print(time.Month, DEC);
    Serial.print('/');
    if(time.Day < 10){Serial.print(0, DEC);}
    Serial.print(time.Day, DEC);
    Serial.print(' ');
    if(time.Hour < 10){Serial.print(0, DEC);}
    Serial.print(time.Hour, DEC);
    Serial.print(':');
    if(time.Minute < 10){Serial.print(0, DEC);}
    Serial.print(time.Minute, DEC);
    Serial.print(':');
    if(time.Second < 10){Serial.print(0, DEC);}
    Serial.print(time.Second, DEC);
    Serial.print(" ");
    Serial.print(weekDay[time.Wday - 1]);
    Serial.println();
}


