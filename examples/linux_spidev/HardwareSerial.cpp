
#include "HardwareSerial.h"


void SerialImpl::print(char val)
{
	printf("%c",val);
}

void SerialImpl::print() {

}

void SerialImpl::println(const char *string) {
	printf(string);
	printf("\n");
}

void SerialImpl::print(const char *string) {
	printf(string);
}

void SerialImpl::print(char *string) {
	printf(string);
}

void SerialImpl::println() {
	printf("\n");
}

void SerialImpl::print(int i) {
	printf("%i", i);
}

void SerialImpl::println(int i) {
	printf("%i", i);
	printf("\n");
}

void SerialImpl::print(int value, base_t base) {
	switch(base)
	{
		case DEC:
			printf("%i", value);
			break;;
		case HEX:
			printf("%x", value);
			break;;
		case BIN:
			std::cout << std::bitset<8>(value);
			break;;
	}
}

bool SerialImpl::available() {
    return true;
}

char SerialImpl::read() {
    return (char) getchar();
}


