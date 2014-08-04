/*
 * AB08XX.cpp
 *
 *  Created on: Jul 27, 2014
 *      Author: eng-nbb
 */

#include "AB08XX.h"

uint8_t bcd2bin(uint8_t value)
{
	return (value & 0x0F) + ((value >> 4) * 10);
}

uint8_t bin2bcd(uint8_t value)
{
	return ((value / 10) << 4) + value % 10;
}


AB08XX::AB08XX(){
	error_code = AB08XX_NO_ERROR;
	Wire.begin();
}


time_t AB08XX::get()
{
	tmElements_t tm;
	read(tm);
	return makeTime(tm);
}

void AB08XX::setError(uint64_t error_code)
{
	this->error_code |= error_code;
}

uint64_t AB08XX::getError()
{
	return error_code;
}

void AB08XX::read(tmElements_t &tm)
{
	AB08XX_memorymap buf;

	Wire.beginTransmission(AB08XX_ADDRESS);

	Wire.write(OFFSETOF(AB08XX_memorymap, hundreths));
	Wire.endTransmission();

	size_t size = OFFSETOF(AB08XX_memorymap, hundredths_alarm) - OFFSETOF(AB08XX_memorymap, hundreths);
	Wire.requestFrom(AB08XX_ADDRESS, size);
	size_t sz = Wire.readBytes((uint8_t *)&buf, size);
	if(sz != size)
	{
		setError(AB08XX_READ_ERROR);
	}

	tm.Year = CalendarYrToTm(bcd2bin(buf.years) + (buf.weekdays.GP * 100));
	tm.Month = bcd2bin(buf.month.data);
	tm.Day = bcd2bin(buf.date.data);
	tm.Hour = bcd2bin(buf.hours_24.data);
	tm.Minute = bcd2bin(buf.minutes.data);
	tm.Second = bcd2bin(buf.seconds.data);
	tm.Wday = buf.weekdays.data + 1;

}

void AB08XX::write(tmElements_t &tm)
{

	size_t size = OFFSETOF(AB08XX_memorymap, hundredths_alarm) - OFFSETOF(AB08XX_memorymap, hundreths);
	AB08XX_memorymap buf;

	uint16_t year = tmYearToCalendar(tm.Year);
	buf.years = bin2bcd(year % 100);
	buf.weekdays.GP = year / 100;
	buf.month.data = bin2bcd(tm.Month);
	buf.date.data = bin2bcd(tm.Day);
	buf.hours_24.data = bin2bcd(tm.Hour);
	buf.date.data = bin2bcd(tm.Day);
	buf.minutes.data = bin2bcd(tm.Minute);
	buf.seconds.data = bin2bcd(tm.Second);
	buf.weekdays.data = tm.Wday - 1;
	buf.hundreths = 0;

	Wire.beginTransmission(AB08XX_ADDRESS);
	Wire.write(OFFSETOF(AB08XX_memorymap, hundreths));
	size_t count = Wire.write((uint8_t*)&buf, size);
	Wire.endTransmission();
	if(count != size)
	{
		setError(AB08XX_WRITE_ERROR);
	}
}




