#ifndef __HARDWARESERIAL_H__
#define __HARDWARESERIAL_H__

#include "AString.h"
#include "stdlib.h"
#include <iostream>
#include <bitset>
//#include <time.h>
#include <stdio.h>
//#include <string.h>


enum base_t
{
    BIN,
    DEC,
    HEX
};

class SerialImpl
{
public:
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

static SerialImpl Serial;
#endif
