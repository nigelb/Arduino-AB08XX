
#include "HardwareSerial.h"
#include <string.h>

SerialImpl Serial;

void _printw(char* src, int length, char to_remove)
{
	for(int i = 0; i < length; i++)
	{
		if(src[i] != to_remove)
		{
			printw("%c", src[i]);
		}
	}
}

void _printw(const char* src, int length, char to_remove)
{
	_printw((char*)src, length, to_remove);
}

SerialImpl::SerialImpl(){}
void SerialImpl::begin()
{
	this->win = initscr();
	scrollok(this->win, true);
	noecho();
        char_available = false;
        value = 0;
	println("INIT");
}

SerialImpl::~SerialImpl()
{
	endwin();
	printf("EXIT\n");
}
void SerialImpl::print(char val)
{
	printw("%c",val);
	refresh();
}

void SerialImpl::print() {

}

void SerialImpl::println(const char *string) {
	_printw(string, strlen(string), '\r');
	printw("\n");
	refresh();
}

void SerialImpl::print(const char *string) {
	_printw(string, strlen(string), '\r');
	refresh();
}

void SerialImpl::print(char *string) {
	_printw(string, strlen(string), '\r');
	refresh();
}

void SerialImpl::println() {
	printw("\n");
	refresh();
}

void SerialImpl::print(int i) {
	printw("%i", i);
	refresh();
}

void SerialImpl::println(int i) {
	printw("%i", i);
	printw("\n");
	refresh();
}

void SerialImpl::print(int value, base_t base) {
	switch(base)
	{
		case DEC:
			printw("%i", value);
			refresh();
			break;;
		case HEX:
			printw("%x", value);
			refresh();
			break;;
		case BIN:
			std::cout << std::bitset<8>(value);
			refresh();
			break;;
	}
}

bool SerialImpl::available() {
//    return kbkit() > 0;
	bool to_ret = false;
	int kbuf;
	if (nodelay(this->win, true) != ERR)
	{
		kbuf = getch();
		if(kbuf != ERR)
		{
			value = kbuf;
			char_available = true;
			to_ret = true;
		}
		
	}
	nodelay(this->win, false);
	return to_ret;
}

char SerialImpl::read() {
	if(char_available)
	{
		char_available = false;
		return value;
	}
	return (char) getchar();
}


