/*
 * AB08XX_SPI.h
 *
 *  Created on: Aug 25, 2014
 */

#ifndef AB08XX_SPI_H_
#define AB08XX_SPI_H_

/*
                 AB080X                                      AB081X

             Xi  XO  VSS VCC                              Xi  XO  VSS VCC
          +---|---|---|---|-+                          +---|---|---|---|-+
    NC   _|1 _____________  |_ nIRQ      (1)     NC   _|1 _____________  |_ nCE
(1) WDI  _| /             | |_ FOUT/nIRQ     (3) WDI  _| /             | |_ FOUT/nIRQ
    NC   _| |     VSS     | |_ EXTI      (1)     NC   _| |    VSS      | |_ EXTI      (3)
    nIRQ2_| |     PAD     | |_ VSS               nIRQ2_| |    PAD      | |_ VSS
          | |_____________| |                          | |_____________| |
          |                 |                          |                 |
          +---|---|---|---|-+                          +---|---|---|---|-+
            VBAT SDA SCL CLKOUT/nIRQ3                    VBAT SDA SCL CLKOUT/nIRQ3
             (2)          (1)                             (4)          (3)


                          (1) - Available in AB0804 and AB0805 only, else NC
                          (2) - Available in AB0803 and AB0805 only, else VSS
                          (3) - Available in AB0814 and AB0815 only, else NC
                          (4) - Available in AB0813 and AB0815 only, else VSS
*/

#include "AB08XX.h"
#include "SPI.h"

class AB08XX_SPI: public AB08XX
{
private:
	uint16_t cs_pin;
public:
	AB08XX_SPI(uint16_t cs_pin);
	virtual size_t _read(uint8_t offset, uint8_t* buf, uint16_t size);
	virtual size_t _write(uint8_t offset, uint8_t* buf, uint16_t size);

};


#endif /* AB08XX_SPI_H_ */
