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
#include "Arduino.h"
#include "HardwareSerial.h"


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
	ab08xx_tmElements_t tm;
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

void AB08XX::read(ab08xx_tmElements_t &tm)
{
	ab08xx_time_t buf;

	_read(OFFSETOF(AB08XX_memorymap, time), (uint8_t*)&buf, sizeof(ab08xx_time_t));

	tm.Year = CalendarYrToTm(bcd2bin(buf.years) /*+ (buf.weekdays.GP * 100)*/) + 2000;
	tm.Month = bcd2bin(buf.month.data);
	tm.Day = bcd2bin(buf.date.data);
	tm.Hour = bcd2bin(buf.hours_24.data);
	tm.Minute = bcd2bin(buf.minutes.data);
	tm.Second = bcd2bin(buf.seconds.data);
	tm.Hundreths = bcd2bin(buf.hundreths);
	tm.Wday = buf.weekdays.data + 1;
}

void AB08XX::read(AB08XX_memorymap &mem)
{
	_read(0, (uint8_t*)&mem, sizeof(AB08XX_memorymap));
}

void AB08XX::write(ab08xx_tmElements_t &tm)
{

	ab08xx_time_t buf;

	uint16_t year = tmYearToCalendar(tm.Year);
	buf.years = bin2bcd(year % 100);
//	buf.weekdays.GP = year / 100;
	buf.month.data = bin2bcd(tm.Month);
	buf.date.data = bin2bcd(tm.Day);
	buf.hours_24.data = bin2bcd(tm.Hour);
	buf.date.data = bin2bcd(tm.Day);
	buf.minutes.data = bin2bcd(tm.Minute);
	buf.seconds.data = bin2bcd(tm.Second);
	buf.weekdays.data = tm.Wday - 1;
	buf.hundreths = 0;

	_write(OFFSETOF(AB08XX_memorymap, time), (uint8_t*)&buf, sizeof(ab08xx_time_t));
}

void AB08XX::readAlarm(ab08xx_tmElements_t &alarm, ab08xx_alarm_repeat_mode_t &mode)
{
	ab08xx_alarm_t  buf;
	timer_control_t	timer_control;
	four_four_t HA;

	_read(OFFSETOF(AB08XX_memorymap, alarm), (uint8_t*)&buf, sizeof(ab08xx_alarm_t));
	_read(OFFSETOF(AB08XX_memorymap, timer_control), (uint8_t*)&timer_control, sizeof(timer_control_t));

	alarm.Year = 0;
	alarm.Month = bcd2bin(buf.month_alarm.data);
	alarm.Day = bcd2bin(buf.date_alram.data);
	alarm.Hour = bcd2bin(buf.hour_alarm.data);
	alarm.Minute = bcd2bin(buf.minute_alarm.data);
	alarm.Second = bcd2bin(buf.seconds_alarm.data);
	alarm.Hundreths = bcd2bin(buf.hundredths_alarm);
	alarm.Wday = buf.weekday_alarm.data + 1;

	switch(timer_control.RPT)
	{
	default:
	case 0:
		mode = alarm_disabled;
		break;
	case 1:
		mode = once_per_year;
		break;
	case 2:
		mode = once_per_month;
		break;
	case 3:
		mode = once_per_week;
		break;
	case 4:
		mode = once_per_day;
		break;
	case 5:
		mode = once_per_hour;
		break;
	case 6:
		mode = once_per_minute;
		break;
	case 7:
		HA.value = buf.hundredths_alarm;
		if(HA.fields.MSB == 0xF && HA.fields.LSB <= 10)
		{
			mode = once_per_tenth;
			alarm.Hundreths = HA.fields.LSB;
		}
		else if(buf.hundredths_alarm == 0xFF)
		{
			mode = once_per_hundreth;
			alarm.Hundreths = 0;
		}else
		{
			mode = once_per_second;
		}
		break;
	}

}

void AB08XX::writeAlarm(ab08xx_tmElements_t &alarm, ab08xx_alarm_repeat_mode_t mode)
{
	ab08xx_alarm_t ab08xx_alarm;
	timer_control_t timer_control;
	four_four_t HA;

	_read(OFFSETOF(AB08XX_memorymap, timer_control), (uint8_t*)&timer_control, sizeof(timer_control_t));

	ab08xx_alarm.month_alarm.data = bin2bcd(alarm.Month);
	ab08xx_alarm.date_alram.data = bin2bcd(alarm.Day);
	ab08xx_alarm.hour_alarm.data = bin2bcd(alarm.Hour);
	ab08xx_alarm.minute_alarm.data = bin2bcd(alarm.Minute);
	ab08xx_alarm.seconds_alarm.data = bin2bcd(alarm.Second);
	ab08xx_alarm.hundredths_alarm = bin2bcd(alarm.Hundreths);
	ab08xx_alarm.weekday_alarm.data = bin2bcd(alarm.Wday - 1);

	switch(mode)
	{

	case once_per_year:
		timer_control.RPT = 1;
		break;
	case once_per_month:
		timer_control.RPT = 2;
		break;
	case once_per_week:
		timer_control.RPT = 3;
		break;
	case once_per_day:
		timer_control.RPT = 4;
		break;
	case once_per_hour:
		timer_control.RPT = 5;
		break;
	case once_per_minute:
		timer_control.RPT = 6;
		break;
	case once_per_second:
		timer_control.RPT = 7;
		break;
	case once_per_tenth:
		timer_control.RPT = 7;
		HA.value = ab08xx_alarm.hundredths_alarm;
		HA.fields.MSB = 0xF;
		ab08xx_alarm.hundredths_alarm = HA.value;
		break;
	case once_per_hundreth:
		timer_control.RPT = 7;
		ab08xx_alarm.hundredths_alarm = 0xFF;
		break;
	default:
		timer_control.RPT = 0;
		break;
	}

	_write(OFFSETOF(AB08XX_memorymap, timer_control), (uint8_t*)&timer_control, sizeof(timer_control_t));
	_write(OFFSETOF(AB08XX_memorymap, alarm), (uint8_t*)&ab08xx_alarm, sizeof(ab08xx_alarm_t));
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

size_t AB08XX::readRAM(uint8_t ram_offset, uint8_t* buf, uint8_t buf_offset, uint16_t size)
{
	return _read(OFFSETOF(AB08XX_memorymap, ram) + ram_offset, buf + buf_offset, size);
}

size_t AB08XX::writeRAM(uint8_t ram_offset, uint8_t* buf, uint8_t buf_offset, uint16_t size)
{
	return _write(OFFSETOF(AB08XX_memorymap, ram) + ram_offset, buf + buf_offset, size);
}





