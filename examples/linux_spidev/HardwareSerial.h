#ifndef __HARDWARESERIAL_H__
#define __HARDWARESERIAL_H__

#include "AString.h"
#include "stdlib.h"
#include <iostream>
#include <bitset>
#include <stdio.h>
#include <curses.h>


enum base_t
{
    BIN,
    DEC,
    HEX
};

class SerialImpl
{
private:
	bool char_available;
	char value;
	WINDOW* win;
public:
    SerialImpl();
    ~SerialImpl();
    void begin();
    void print();
    void print(const char *);
    void print(char *);
    void print(char );
    void print(int);
    void println();
    void println(const char *);
    void println(int );
    bool available();
    char read();

    void print(int value, base_t base);
};

extern SerialImpl Serial;
#endif
