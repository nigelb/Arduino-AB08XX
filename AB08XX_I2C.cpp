/*
 * AB08XX_I2C.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: eng-nbb
 */

#include "AB08XX_I2C.h"


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

	for(uint8_t pos = 0, chunk_size; pos < size; pos += chunk_size)
	{
		chunk_size = size - pos;
		if(chunk_size > (BUFFER_LENGTH - 1))
		{
			chunk_size = (BUFFER_LENGTH - 1);

		}
		if((offset + pos) < 0x40 && (pos + offset + chunk_size) > 0x40)
		{
			chunk_size = 0x40 - (offset + pos);
		}
		Wire.requestFrom((uint8_t)AB08XX_ADDRESS, chunk_size);
		total_read = Wire.readBytes(buf + pos, chunk_size);
	}

	if(total_read != size)
	{
		setError(AB08XX_READ_ERROR);
	}
	return total_read;
}

size_t AB08XX_I2C::_write(uint8_t offset, uint8_t* buf, uint16_t size)
{
	size_t total_written = 0;
	for(uint8_t pos = 0, chunk_size; pos < size; pos += chunk_size)
	{
		chunk_size = size - pos;

		//For some reason if I use BUFFER_LENGTH instead of (BUFFER_LENGTH - 1) the
		//last byte is not written to the device.
		if(chunk_size > (BUFFER_LENGTH - 1))
		{
			chunk_size = (BUFFER_LENGTH - 1);
		}
		Wire.beginTransmission(AB08XX_ADDRESS);
		Wire.write(offset + pos);
		total_written += Wire.write(buf + pos, chunk_size);
		Wire.endTransmission();

	}

	if(total_written != size)
	{
		setError(AB08XX_WRITE_ERROR);
	}
	return total_written;
}
