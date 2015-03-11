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


        printf("spi device: %s\n", device);
        printf("spi mode: %d\n", mode);
        printf("spi delay: %d\n", delay);
        printf("bits per word: %d\n", bits);
        printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

}

size_t AB08XX_SPI_Linux::_read(uint8_t offset, uint8_t* buf, uint16_t size)
{
	struct spi_ioc_transfer xfer[2];

	memset(xfer, 0, sizeof xfer);
//	memset(buf, 0, size);

	printf("Offset %d\n", offset);

	buf[0] = SPI_READ(offset);

        xfer[0].tx_buf = (unsigned long)buf;
//        xfer[0].rx_buf = (unsigned long)buf;
        xfer[0].len = 1;
        xfer[0].cs_change = 1;
//        xfer[0].delay_usecs     = delay;
//        xfer[0].speed_hz        = speed;
//        xfer[0].bits_per_word   = bits;

        xfer[1].rx_buf = (unsigned long)buf;
        xfer[1].len = size;
        xfer[0].cs_change = 0;

        int status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
        if (status < 0) {
                perror("SPI_IOC_MESSAGE");
                return -1;
        }
	printf("\n %d \n", status);
        for (int i = 0; i < size; i++)
        {
                printf(" %02x ", buf[i]);
        }

	printf("\n");

	return (size_t)status;
}

size_t AB08XX_SPI_Linux::_write(uint8_t offset, uint8_t* buf, uint16_t size)
{
	struct spi_ioc_transfer xfer[2];
	uint8_t  addr[] = {SPI_WRITE(offset)};
	memset(xfer, 0, sizeof xfer);
//	memset(buf, 0, size);

	printf("Offset %d\n", offset);
	printf("Size %d\n", size);


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
	printf("\n %d \n", status);
        for (int i = 0; i < size; i++)
        {
                printf(" %02x ", buf[i]);
        }

	printf("\n");

	return (size_t)status;
	return -1;
}

AB08XX_SPI_Linux::~AB08XX_SPI_Linux()
{
	printf("Clossing...\n");
	close(fd);
}

