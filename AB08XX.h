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

#ifndef AB08XX_H_
#define AB08XX_H_

#include "stdint.h"
#include "Time.h"
#include "stddef.h"


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

#ifndef OFFSETOF
#define OFFSETOF(type, field)    ((unsigned long) &(((type *) 0)->field))
#endif

#define AB08XX_NO_ERROR 0
#define AB08XX_READ_ERROR 1
#define AB08XX_WRITE_ERROR 2

#define AB08XX_ADDRESS (0xD2 >> 1)



struct seven_one_t
{
	uint8_t data:7;
	uint8_t GP:1;
};

struct six_two_t
{
	uint8_t data:6;
	uint8_t GP:2;
};

struct five_three_t
{
	uint8_t data:5;
	uint8_t GP:3;
};

//struct four_four_t
//{
//	uint8_t data:4;
//	uint8_t GP:4;
//};
union four_four_t
{
	struct
	{
		uint8_t LSB:4;
		uint8_t MSB:4;
	} fields;
	uint8_t value;
};

struct three_five_t
{
	uint8_t data:3;
	uint8_t GP:5;
};

struct ab08xx_time_t
{
	uint8_t hundreths;
	seven_one_t seconds;
	seven_one_t minutes;
	six_two_t hours_24;
	six_two_t date;
	five_three_t month;
	uint8_t years;
	three_five_t weekdays;
};

struct ab08xx_alarm_t
{
	uint8_t hundredths_alarm;
	seven_one_t seconds_alarm;
	seven_one_t minute_alarm;
	six_two_t hour_alarm;
	six_two_t date_alram;
	five_three_t month_alarm;
	three_five_t weekday_alarm;
};

struct status_t {
	bool EX1: 1;
	bool EX2: 1;
	bool ALM: 1;
	bool TIM: 1;
	bool BL : 1;
	bool WDT: 1;
	bool BAT: 1;
	bool CB : 1;
};

struct control1_t {
	bool WRTC: 1;
	bool PWR2: 1;
	bool ARST: 1;
	bool RSP : 1;
	bool OUT : 1;
	bool OUTB: 1;
	bool _12_24: 1;
	bool STOP: 1;
};

struct control2_t {
		uint8_t OUT1S: 2;
		uint8_t OUT2S: 3;
		bool    RS1E: 1;
		bool    PAD: 1;
		bool    OUTPP: 1;
};

struct inturrupt_mask_t
{
	bool EX1E : 1;
	bool EX2E : 1;
	bool AIE : 1;
	bool TIE : 1;
	bool BLIE : 1;
	bool IM : 2;
	bool CEB : 1;
};

struct square_wave_t {
	uint8_t SQFS: 5;
	uint8_t PAD: 2;
	bool SQWE: 1;
};

struct cal_xt_t
{
	uint8_t OFFSETX: 7;
	uint8_t CMDX: 1;
};

struct cal_rc_hi_t
{
	uint8_t OFFSETR: 6;
	uint8_t CMDR: 2;
};

struct cal_rc_low_t
{
	uint8_t OFFSETR;
};

struct sleep_control_t
{
	uint8_t SLTO : 3;
	uint8_t SLST : 1;
	uint8_t EX1P : 1;
	uint8_t EX2P : 1;
	uint8_t SLRES: 1;
	uint8_t SLP  : 1;
};

struct timer_control_t
{
	uint8_t TFS : 2;
	uint8_t RPT : 3;
	uint8_t TRPT: 1;
	uint8_t TM  : 1;
	uint8_t TE  : 1;
};

enum watchdog_timer_freq_select_t: uint8_t
{
	_16hz = 0,
	_4hz = 1,
	_1hz = 2,
	quater_hz = 3,
};

struct watchdog_timer_t
{
	watchdog_timer_freq_select_t WRB: 2; //Watchdog clock frequency
	uint8_t BMB: 5;
	uint8_t WDS: 1; //Watchdog Steering:
					//				0 -> Watchdog Timer will generate WIRQ when it times out.
					//				1 -> Watchdog Timer will generate a reset when it times out.
};

enum trickle_charge_enable_t: uint8_t
{
	disable_trickle_charge = 0,
	enable_trickle_charge = 10,
};

enum trickle_diode_t: uint8_t
{
	disabled_diode = 0, //Any value that is not a valid 
	schottky_diode = 1, //0.3V drop
	standard_diode = 2, //0.6V drop
};

enum trickle_resistor_t: uint8_t
{
	disable_resistor = 0,
	_3k = 1,
	_6K = 2,
	_11K = 3,
};

enum configuration_key_t: uint8_t
{
	enable_oscillator_control_register = 0xA1,
	conduct_software_reset = 0x3C,
	enable_trickle_register = 0x9D,
	enable_bref_register = 0x9D,
	enable_afctrl_register = 0x9D,
	enable_batmode_io_register = 0x9D,
	enable_output_control_register = 0x9D,
};

//struct idenstification_t
//{
//	uint8_t part_msb;
//	uint8_t part_lsb;
//	uint8_t revision_minor: 3;
//	uint8_t revision_major: 5;
//	uint8_t mfg_week:4;
//	uint8_t mfg_year:4;
//};

struct AB08XX_memorymap
{
	ab08xx_time_t time;
	ab08xx_alarm_t alarm;
	status_t status;
	control1_t control1;
	control2_t control2;
	inturrupt_mask_t int_mask;
	square_wave_t sqw;

	cal_xt_t     cal_xt;
	cal_rc_hi_t  cal_rc_hi;
	cal_rc_low_t cal_rc_low;
	sleep_control_t sleep_control;
	timer_control_t timer_control;
	uint8_t timer;
	uint8_t timer_initial;
	watchdog_timer_t wdt;
	//uint8_t wdt;
	//uint8_t osc_control;
	struct osc_control_t
	{
		uint8_t _ACIE: 1;
		uint8_t OFIE: 1;
		uint8_t PWGT: 1;
		uint8_t	FOS:  1;
		uint8_t AOS:  1;
		uint8_t ACAL: 2;
		uint8_t OSEL: 1;
	} osc_control;
	//uint8_t osc_status;
	struct osc_status_t
	{
		uint8_t ACF:      1;
		uint8_t OF:       1;
		uint8_t RESERVED: 2;
		uint8_t OMODE:    1;
		uint8_t LKO2:     1;
		uint8_t XTCAL:    1;
	} osc_status;
	uint8_t RESERVED;
	uint8_t configuration_key;
//	uint8_t trickle;
	struct trickle_t
	{
		uint8_t resistor: 2;
		uint8_t diode:    2;
		uint8_t	enable:   4;
	} trickle;
	uint8_t bref_control;
	uint8_t RESERVED2[4];

	uint8_t	afctrl;
	struct batmode_io_t
	{
		uint8_t RESERVED: 7;
		bool IOBM:     1;
	} batmode_io;
	struct id_t {
		uint8_t part_number_upper;
		uint8_t part_number_lower;
		struct revision_t
		{
			uint8_t MINOR: 3;
			uint8_t MAJOR: 5;
		} revision;
	/*	struct manufacturing_date_t
		{
			uint8_t WEEKL: 4;
			uint8_t YEAR: 4;
		} date;
		struct manufacturing_info_t
		{
			uint8_t WEEKU: 2;
			uint8_t WAFER: 6;
		} info;
		struct manufacturing_info2_t
		{
			uint8_t QUADRANT: 6;
			uint8_t LOT: 2;
		} quad; */
	
		struct manufacturing_info_t
		{
			uint8_t WEEKL: 4;
			uint8_t YEAR: 4;
			uint8_t WEEKU: 2;
			uint8_t WAFER: 6;
			uint8_t QUADRANT: 6;
			uint8_t LOT: 2;
		} info;
		
		uint8_t serialization;
		
	} id;
//	uint8_t id0;
//	uint8_t id1;
//	uint8_t id2;
//	uint8_t id3;
//	uint8_t id4;
//	uint8_t id5;
//	uint8_t id6;
	uint8_t astat;
	uint8_t octrl;
	uint8_t RESERVED3[14];
	uint8_t extention_address;
	uint8_t ram[64];
};

struct ab08xx_tmElements_t: tmElements_t
{
	uint8_t Hundreths;
};

//From the Alarm repeat function table (20)
enum ab08xx_alarm_repeat_mode_t
{
	alarm_disabled = 0,
	once_per_year,
	once_per_month,
	once_per_week,
	once_per_day,
	once_per_hour,
	once_per_minute,
	once_per_second,	//HA = [0-9][0-9]
	once_per_tenth,     //HA = F[0-9]
	once_per_hundreth,  //HA = FF
	invalid_mode        //Where HA is ab08xx_alarm_t.hundredths_alarm
};

uint8_t bcd2bin(uint8_t value);
uint8_t bin2bcd(uint8_t value);

class AB08XX
{
private:

	uint64_t error_code;

//	size_t read(uint8_t offset, uint8_t* buf, uint16_t size);
//	size_t write(uint8_t offset, uint8_t* buf, uint16_t size);
protected:
	void setError(uint64_t error_code);
public:
	AB08XX();
	/*
	 * Read <size> bytes of the clocks memory starting at <offset> into <buf>.
	 * If size is larger than
	 */
	virtual size_t _read(uint8_t offset, uint8_t* buf, uint16_t size) = 0;
	virtual size_t _write(uint8_t offset, uint8_t* buf, uint16_t size) = 0;
	virtual ~AB08XX();

	time_t get();
	void read(ab08xx_tmElements_t &tm);
	void write(ab08xx_tmElements_t &tm);

	void read(AB08XX_memorymap &mem);

	uint64_t getError();

	void readAlarm(ab08xx_tmElements_t &alarm, ab08xx_alarm_repeat_mode_t &mode);
	void writeAlarm(ab08xx_tmElements_t &alarm, ab08xx_alarm_repeat_mode_t mode);

	void readStatus(status_t &data);
	void writeStatus(status_t &data);
	void readControl1(control1_t &data);
	void writeControl1(control1_t &data);
	void readControl2(control2_t &data);
	void writeControl2(control2_t &data);
	void readInturruprMask(inturrupt_mask_t &data);
	void writeInturruprMask(inturrupt_mask_t &data);

	size_t readRAM(uint8_t ram_offset, uint8_t* buf, uint8_t buf_offset, uint16_t size);
	size_t writeRAM(uint8_t ram_offset, uint8_t* buf, uint8_t buf_offset, uint16_t size);

};


#endif /* AB08XX_H_ */
