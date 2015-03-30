#ifndef __AB08XX_CLI_H__
#define __AB08XX_CLI_H__

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include "AB08XX.h"
#include "Arduino.h"

#define LEADING_ZERO(_Stream,_Base,_Value)   if(_Value < _Base){_Stream.print(0);}_Stream.print(_Value, _Base)
#define LEADING_ZERO_ln(_Stream,_Base,_Value)   LEADING_ZERO(_Stream,_Base,_Value);_Stream.println()
#define PRINT_WIDTH(__stream, __size, __pad) for(int8_t j = 0; j < __size; j++){__stream.print(__pad);}


const char week_day[][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

struct alarm_repeat_map_t
{
    ab08xx_alarm_repeat_mode_t mode;
    const char* name;

};

struct trickle_diode_map_t
{
	trickle_diode_t diode;
	const char* name;	
};

struct trickle_resistor_map_t
{
	trickle_resistor_t resistor;
	const char* name;
};

struct trickle_charge_enable_map_t
{
	trickle_charge_enable_t enable;
	const char* name;
};

struct cmnd_t {
    const char* command;
    int (*callback)(int argc, char** argv);
    const char* description;
    int (*help)(int argc, char** argv);
};

struct register_map_t
{
    const char* register_name;
    uint64_t offset;
};

void setup();
void loop();
void prompt();


cmnd_t* find_cmnd(char* command, cmnd_t* command_vector);
int run_subcomand(int argc, char** argv, int (*default_subcommand)(int argc, char** argv), int (*help_subcommand)(int argc, char** argv), cmnd_t* subcommand_vecter);

int set(int argc, char** argv);
int get(int argc, char** argv);
int alarm(int argc, char** argv);
int set_help(int argc, char** argv);
int watch(int argc, char** argv);
int stop(int argc, char** argv);
int start(int argc, char** argv);
int status(int argc, char** argv);
int int_mask(int argc, char** argv);
int control1_mask(int argc, char** argv);
int control2_mask(int argc, char** argv);
int clock_id(int argc, char** argv);
int hex_get(int argc, char** argv);
int hex_set(int argc, char** argv);
int bin_get(int argc, char** argv);
int map(int argc, char** argv);
int _help(int argc, char** argv, cmnd_t* command_vecter);
int help(int argc, char** argv);
int exit(int argc, char** argv);

int trickle(int argc, char** argv);
int trickle_set(int argc, char** argv);
int trickle_set_help(int argc, char** argv);
int trickle_get(int argc, char** argv);
int trickle_help(int argc, char** argv);

int alarm_set(int argc, char** argv);
int alarm_get(int argc, char** argv);
int alarm_enable(int argc, char** argv);
int alarm_disable(int argc, char** argv);
int alarm_help(int argc, char** argv);
int alarm_help_set(int argc, char** argv);

void displayTime();
void displayTime(ab08xx_tmElements_t &time);
void print_time_element(ab08xx_tmElements_t &time);
void print_alarm_element(ab08xx_tmElements_t &time);

void print_alarm_mode();
void print_days_of_week();
alarm_repeat_map_t* get_alarm_mode(ab08xx_alarm_repeat_mode_t to_get);
int get_alarm_mode_id(ab08xx_alarm_repeat_mode_t to_get);
cmnd_t* get_cli(char* command);
char* upper(char* value);

static cmnd_t cli_vector[] = {
        { "set",      &set, "Set the clock.", &set_help },
        { "get",      &get, "Retrieve and display the time from the clock.", NULL },
        { "alarm",    &alarm, "Retrieve and display the alarm information from the clock.", &alarm_help },
        { "watch",    &watch, "Watch the time on the clock.", NULL },
        //{ "usage",    &set_help, "Display the usage of the set command.", NULL },
        { "stop",     &stop, "Stop the oscillator.", NULL },
        { "start",    &start, "Start the oscillator.", NULL },
        { "status",   &status, "Display the status register.", NULL },
        { "int",      &int_mask, "Display and set the Interrupt Mask register.\r\n\r\n\t\t\t\tSetting the inturrupt register:\r\n\r\n\t\t\t\t\tint <INTURRUPT> <VALUE>\r\n", NULL },
        { "control1", &control1_mask, "Display the control1 register.", NULL },
        { "control2", &control2_mask, "Display the control2 register.", NULL },
        { "id",       &clock_id, "Display the ID registers.", NULL },
        { "trickle",  &trickle, "Display the trickle register.", NULL },
        { "binget",   &bin_get, "Display the contents of the clocks memory in Binary:\r\n\r\n\t\t\t\tbinget [offset[, count]]\r\n", NULL },
        { "hexget",   &hex_get, "Display the contents of the clocks memory in Hex:\r\n\r\n\t\t\t\thexget [offset[, count]]\r\n", NULL },
        { "hexset",   &hex_set, "Write data to the clock's memory in Hex (all values are read as hex):\r\n\r\n\t\t\t\thexset offset val1 [val2] [val3] [val4]\r\n", NULL },
        { "map",      &map,   "Display register map.", NULL },
        { "help",     &help,   "List available commands.", NULL },
#ifndef Arduino_h
	{ "exit",     &exit, "Exit", NULL},
	{ "quit",     &exit, "Exit", NULL},
#endif

        { NULL, NULL, NULL }
};

static cmnd_t alarm_vector[] = {
        { "set", &alarm_set, "Set the alarm.", &alarm_help_set },
        { "get", &alarm_get, "Retrieve and display the alarm from the clock.", NULL },
        { "enable", &alarm_enable, "Enable the alarm.", NULL },
        { "disable", &alarm_disable, "Disable the alarm.", NULL },
        { "help", &alarm_help, "Display usage for the alarm command.", NULL },

        { NULL, NULL, NULL }
};

static cmnd_t trickle_vector[] = {
	{"set", &trickle_set, "Set the trickle register.", &trickle_set_help},
	{"get", &trickle_get, "Get the trickle register.", NULL},
	{"help", &trickle_help, "Display usage for trickle command.", NULL},

        { NULL, NULL, NULL, NULL }
};

static register_map_t register_map[] = {
        {"time", 				OFFSETOF(AB08XX_memorymap,time)},
        {"alarm", 				OFFSETOF(AB08XX_memorymap,alarm)},
        {"status", 				OFFSETOF(AB08XX_memorymap,status)},
        {"control1", 			OFFSETOF(AB08XX_memorymap,control1)},
        {"control2", 			OFFSETOF(AB08XX_memorymap,control2)},
        {"int_mask", 			OFFSETOF(AB08XX_memorymap,int_mask)},
        {"sqw", 				OFFSETOF(AB08XX_memorymap,sqw)},
        {"cal_xt", 				OFFSETOF(AB08XX_memorymap,cal_xt)},
        {"cal_rc_hi", 			OFFSETOF(AB08XX_memorymap,cal_rc_hi)},
        {"cal_rc_low", 			OFFSETOF(AB08XX_memorymap,cal_rc_low)},
        {"sleep_control",	 	OFFSETOF(AB08XX_memorymap,sleep_control)},
        {"timer_control",	 	OFFSETOF(AB08XX_memorymap,timer_control)},
        {"timer", 				OFFSETOF(AB08XX_memorymap,timer)},
        {"timer_initial",	 	OFFSETOF(AB08XX_memorymap,timer_initial)},
        {"wdt", 				OFFSETOF(AB08XX_memorymap,wdt)},
        {"osc_control", 		OFFSETOF(AB08XX_memorymap,osc_control)},
        {"osc_status", 			OFFSETOF(AB08XX_memorymap,osc_status)},
        {"RESERVED", 			OFFSETOF(AB08XX_memorymap,RESERVED)},
        {"configuration_key", 	OFFSETOF(AB08XX_memorymap,configuration_key)},
        {"trickle", 			OFFSETOF(AB08XX_memorymap,trickle)},
        {"bref_control", 		OFFSETOF(AB08XX_memorymap,bref_control)},
        {"RESERVED2", 			OFFSETOF(AB08XX_memorymap,RESERVED2)},
        {"id", 				OFFSETOF(AB08XX_memorymap,id)},
//        {"id0", 				OFFSETOF(AB08XX_memorymap,id0)},
//        {"id1", 				OFFSETOF(AB08XX_memorymap,id1)},
//        {"id2", 				OFFSETOF(AB08XX_memorymap,id2)},
//        {"id3", 				OFFSETOF(AB08XX_memorymap,id3)},
//        {"id4", 				OFFSETOF(AB08XX_memorymap,id4)},
//        {"id5", 				OFFSETOF(AB08XX_memorymap,id5)},
//        {"id6", 				OFFSETOF(AB08XX_memorymap,id6)},
        {"astat", 				OFFSETOF(AB08XX_memorymap,astat)},
        {"octrl", 				OFFSETOF(AB08XX_memorymap,octrl)},
        {"RESERVED3", 			OFFSETOF(AB08XX_memorymap,RESERVED3)},
        {"extention_address", 	OFFSETOF(AB08XX_memorymap,extention_address)},
        {"ram", 				OFFSETOF(AB08XX_memorymap,ram)},

        {0, 0}
};

static alarm_repeat_map_t alarm_repeat_map[] =
{
	{alarm_disabled, "alarm_disabled"},
       	{once_per_year, "once_per_year"},
       	{once_per_month, "once_per_month"},
       	{once_per_week, "once_per_week"},
       	{once_per_day, "once_per_day"},
       	{once_per_hour, "once_per_hour"},
       	{once_per_minute, "once_per_minute"},
       	{once_per_second, "once_per_second"},
       	{once_per_tenth, "once_per_tenth"},
       	{once_per_hundreth, "once_per_hundreth"},
       	{invalid_mode, "invalid_mode"},
};

static trickle_diode_map_t trickle_diode_map[] =
{
	{disabled_diode, "disabled_diode"},
        {schottky_diode, "schottky_diode"},
        {standard_diode, "standard_diode"},
};

static trickle_resistor_map_t trickle_resistor_map[] =
{
	{disable_resistor, "disable_resistor"},
	{_3k, "3K ohm resistor"},
	{_6K, "6K ohm resistor"},
	{_11K, "11K ohm resistor"},
};

static trickle_charge_enable_map_t trickle_charge_enable_map[] =
{
	{disable_trickle_charge, "disable_trickle_charge"},
	{enable_trickle_charge, "enable_trickle_charge"},
};


extern AB08XX *abclock;
extern String *command;
#endif
