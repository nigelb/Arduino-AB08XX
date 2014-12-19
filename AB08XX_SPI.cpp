/*
 * AB08XX_SPI.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: eng-nbb
 */

#include "AB08XX_SPI.h"
#include "Arduino.h"

#define SPI_READ(offset) (127 & offset)
#define SPI_WRITE(offset) (128 | offset)


AB08XX_SPI::AB08XX_SPI(uint16_t cs_pin) : AB08XX()
{
	this->cs_pin = cs_pin;
	pinMode(cs_pin, OUTPUT);
	pinMode(SS, OUTPUT);
	digitalWrite(cs_pin, HIGH);
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV128);
//	SPI.begin();
}

size_t AB08XX_SPI::_read(uint8_t offset, uint8_t* buf, uint16_t size)
{
	digitalWrite(this->cs_pin, LOW);
	SPI.transfer(SPI_READ(offset));
	for(uint16_t i = 0; i < size; i++)
	{
		buf[i] = SPI.transfer(0);
	}
	digitalWrite(this->cs_pin, HIGH);
	return size;
}

size_t AB08XX_SPI::_write(uint8_t offset, uint8_t* buf, uint16_t size)
{
	return 0;
}
