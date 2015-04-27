/*
 * AB08XX_SPI.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: eng-nbb
 */

#include "AB08XX_SPI_Linux.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h> /* memset */
#include <unistd.h> /* close */
#include <curses.h>


#define SPI_READ(offset) (127 & offset)
#define SPI_WRITE(offset) (128 | offset)

void pabort(const char *s)
{
        perror(s);
        abort();
}



AB08XX_SPI_Linux::AB08XX_SPI_Linux(const char* device, int mode, int bits, int speed, int delay) : AB08XX()
{
        fd = open(device, O_RDWR);
        if (fd < 0)
                pabort("can't open device");

        /*
         * spi mode
         */
//        mode = SPI_MODE_0;
        int ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1)
                pabort("can't set spi mode");

        ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
        if (ret == -1)
                pabort("can't get spi mode");

        /*
         * bits per word
         */
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't set bits per word");

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't get bits per word");

        /*
         * max speed hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't set max speed hz");

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't get max speed hz");


        printw("spi device: %s\n", device);
        printw("spi mode: %d\n", mode);
        printw("spi delay: %d\n", delay);
        printw("bits per word: %d\n", bits);
        printw("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

}

size_t AB08XX_SPI_Linux::_read(uint8_t offset, uint8_t* buf, uint16_t size)
{
	struct spi_ioc_transfer xfer[2];
	uint8_t rx_buf_size = 8, tx[1], rx[rx_buf_size], rx_size;

	memset(xfer, 0, sizeof xfer);
	memset(buf, 0, size);


	for(int i = 0; i < size; i+=8)
	{

		tx[0] = SPI_READ((offset + i));
	//	printw("Offset %d\n", tx[0]);

        	xfer[0].tx_buf = (unsigned long)tx;
//        xfer[0].rx_buf = (unsigned long)buf;
	        xfer[0].len = 1;
        	//xfer[0].cs_change = 1;
	        xfer[0].cs_change = 0;
//        xfer[0].delay_usecs     = delay;
//        xfer[0].speed_hz        = speed;
//        xfer[0].bits_per_word   = bits;

		rx_size = size - i;
		if(rx_size > rx_buf_size)
		{
			rx_size = rx_buf_size;
		}
	        xfer[1].rx_buf = (unsigned long) rx;
        	xfer[1].len = rx_buf_size;


	        int status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
	//	printw("status:  %d\n", status);
		for(int j = 0; j < rx_size; j++)
		{
			buf[i + j] = rx[j];
		}
        	if (status < 0) {
                	perror("SPI_IOC_MESSAGE");
	                return -1;
        	}
	}
	//printw("\n %d \n", status);
        //for (int i = 0; i < size; i++)
        //{
        //        printw(" %02x ", buf[i]);
        //}

	//printw("\n");
	//refresh();

	//return (size_t)status;
	return (size_t)1;
}

size_t AB08XX_SPI_Linux::_write(uint8_t offset, uint8_t* buf, uint16_t size)
{
	struct spi_ioc_transfer xfer[2];
	uint8_t  addr[] = {SPI_WRITE(offset)};
	memset(xfer, 0, sizeof xfer);
//	memset(buf, 0, size);

	printw("Offset %d\n", offset);
	printw("Size %d\n", size);


        xfer[0].tx_buf = (unsigned long)addr;
//        xfer[0].rx_buf = (unsigned long)buf;
        xfer[0].len = 1;
        xfer[0].cs_change = 1;
//        xfer[0].delay_usecs     = delay;
//        xfer[0].speed_hz        = speed;
//        xfer[0].bits_per_word   = bits;

        xfer[1].tx_buf = (unsigned long)buf;
        xfer[1].len = size;
        xfer[0].cs_change = 0;

        int status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
        if (status < 0) {
                perror("SPI_IOC_MESSAGE");
                return -1;
        }
	//printw("\n %d \n", status);
        //for (int i = 0; i < size; i++)
        //{
        //        printw(" %02x ", buf[i]);
        //}

	//printw("\n");
	//refresh();

	return (size_t)status;
}

AB08XX_SPI_Linux::~AB08XX_SPI_Linux()
{
	printw("Clossing...\n");
	close(fd);
}

