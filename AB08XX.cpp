/*
 Arduino-AB08XX is an Arduino library for the Abracon AB08XX series Real Time
 Clock. Copyright (C) 2014 Nigel Bajema

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

}

AB08XX::~AB08XX(){}


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

void AB08XX::read(AB08XX_memorymap &mem)
{
	_read(0, (uint8_t*)&mem, sizeof(AB08XX_memorymap));
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

void AB08XX::readAlarm(tmElements_t &tm)
{

}

void AB08XX::writeAlarm(tmElements_t &tm)
{

}

void AB08XX::readStatus(status_t &data)
{
	_read(OFFSETOF(AB08XX_memorymap, status), (uint8_t*)&data, sizeof(status_t));
}

void AB08XX::writeStatus(status_t &data)
{
	_write(OFFSETOF(AB08XX_memorymap, status), (uint8_t*)&data, sizeof(status_t));
}

void AB08XX::readControl1(control1_t &data)
{
	_read(OFFSETOF(AB08XX_memorymap, control1), (uint8_t*)&data, sizeof(control1_t));
}

void AB08XX::writeControl1(control1_t &data)
{
	_write(OFFSETOF(AB08XX_memorymap, control1), (uint8_t*)&data, sizeof(control1_t));
}

void AB08XX::readControl2(control2_t &data)
{
	_read(OFFSETOF(AB08XX_memorymap, control2), (uint8_t*)&data, sizeof(control2_t));
}

void AB08XX::writeControl2(control2_t &data)
{
	_write(OFFSETOF(AB08XX_memorymap, control2), (uint8_t*)&data, sizeof(control2_t));
}

void AB08XX::readInturruprMask(inturrupt_mask_t &data)
{
	_read(OFFSETOF(AB08XX_memorymap, int_mask), (uint8_t*)&data, sizeof(inturrupt_mask_t));
}

void AB08XX::writeInturruprMask(inturrupt_mask_t &data)
{
	_write(OFFSETOF(AB08XX_memorymap, int_mask), (uint8_t*)&data, sizeof(inturrupt_mask_t));
}


AB08XX_I2C::AB08XX_I2C() : AB08XX()
{
	Wire.begin();
}

size_t AB08XX_I2C::_read(uint8_t offset, uint8_t* buf, uint16_t size)
{
	Wire.beginTransmission(AB08XX_ADDRESS);
	Wire.write(offset);
	Wire.endTransmission();

	size_t total_read = 0;

	uint16_t position = 0;
	for(; position < size; position += BUFFER_LENGTH)
	{
		Wire.requestFrom(AB08XX_ADDRESS, BUFFER_LENGTH);
		total_read += Wire.readBytes(buf + position, BUFFER_LENGTH);

	}
	if((size - position) > 0)
	{
		Wire.requestFrom(AB08XX_ADDRESS, size - position);
		total_read += Wire.readBytes(buf + position, BUFFER_LENGTH);
	}
	if(total_read != size)
	{
		setError(AB08XX_READ_ERROR);
	}
	return total_read;
}

size_t AB08XX_I2C::_write(uint8_t offset, uint8_t* buf, uint16_t size)
{
	Wire.beginTransmission(AB08XX_ADDRESS);
	Wire.write(offset);
	size_t sz = Wire.write((uint8_t*)buf, size);
	Wire.endTransmission();
	if(sz != size)
	{
		setError(AB08XX_WRITE_ERROR);
	}
	return sz;
}




