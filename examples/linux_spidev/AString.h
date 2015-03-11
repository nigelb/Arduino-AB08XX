#ifndef __ASTRING_H__
#define __ASTRING_H__

#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include <string>
#include <algorithm>

class String
{
        std::string data;
public:
        String();
        String(std::string from);
        int length();
        String substring(int pos, int length);
        char operator[](int pos);
        void operator+=(char* val);
        void operator+=(char val);
        void concat(char toAdd);
        void concat(char* toAdd);
        void concat(const char* toAdd);
        void  trim();

};

#endif
