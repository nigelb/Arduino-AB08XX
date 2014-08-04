/*
 * AB08XX.h
 *
 *  Created on: Jul 27, 2014
 *      Author: eng-nbb
 */

#ifndef AB08XX_H_
#define AB08XX_H_

#include "stdint.h"
#include "Time.h"
#include "Wire.h"

#ifndef OFFSETOF
#define OFFSETOF(type, field)    ((unsigned long) &(((type *) 0)->field))
#endif

#define AB08XX_NO_ERROR 0
#define AB08XX_READ_ERROR 1
#define AB08XX_WRITE_ERROR 2
#define AB08XX_ADDRESS (0xD2 >> 1)



struct seven_one
{
	uint8_t data:7;
	uint8_t GP:1;
};

struct six_two
{
	uint8_t data:6;
	uint8_t GP:2;
};

struct five_three
{
	uint8_t data:5;
	uint8_t GP:3;
};

struct four_three
{
	uint8_t data:4;
	uint8_t GP:3;
};

struct three_five
{
	uint8_t data:3;
	uint8_t GP:5;
};

struct status_t {
	bool EX1: 1;
	bool EX2: 1;
	bool AML: 1;
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


struct AB08XX_memorymap
{
	uint8_t hundreths;
	seven_one seconds;
	seven_one minutes;
	six_two hours_24;
	six_two date;
	five_three month;
	uint8_t years;
	three_five weekdays;
	uint8_t hundredths_alarm;
	uint8_t seconds_alarm;
	uint8_t minute_alarm;
	uint8_t hour_alarm;
	uint8_t date_alram;
	uint8_t month_alarm;
	uint8_t weekday_alarm;

	//	uint8_t status;
//	struct {
//		bool EX1: 1;
//		bool EX2: 1;
//		bool AML: 1;
//		bool TIM: 1;
//		bool BL : 1;
//		bool WDT: 1;
//		bool BAT: 1;
//		bool CB : 1;
//	} status;
	status_t status;

//	uint8_t control1;
	control1_t ontrol1;

//	uint8_t control2;
//	struct {
//		uint8_t OUT1S: 2;
//		uint8_t OUT2S: 3;
//		bool    RS1E: 1;
//		bool    PAD: 1;
//		bool    OUTPP: 1;
//	} control2;
	control2_t control2;

//	uint8_t int_mask;
	struct{
		bool EX1E : 1;
		bool EX2E : 1;
		bool AIE : 1;
		bool TIE : 1;
		bool BLIE : 1;
		bool IM : 2;
		bool CEB : 1;

	} int_mask;

//	uint8_t sqw;
	struct{
		uint8_t SQFS: 5;
		uint8_t PAD: 2;
		bool SQWE: 1;
	} sqw;
	uint8_t cal_xt;
	uint8_t cal_rc_hi;
	uint8_t cal_rc_low;
	uint8_t sleep_control;
	uint8_t timer_control;
	uint8_t timer;
	uint8_t timer_initial;
	uint8_t wdt;
	uint8_t osc_control;
	uint8_t osc_status;
	uint8_t RESERVED;
	uint8_t configuration_key;
	uint8_t trickle;
	uint8_t bref_control;
	uint8_t RESERVED2[6];
//	byte RESERVED;
//	byte RESERVED;
//	byte RESERVED;
//	byte RESERVED;
//	byte RESERVED;
	uint8_t id0;
	uint8_t id1;
	uint8_t id2;
	uint8_t id3;
	uint8_t id4;
	uint8_t id5;
	uint8_t id6;
	uint8_t astat;
	uint8_t octrl;
	uint8_t extention_address;
	uint8_t ram[64];
};

uint8_t bcd2bin(uint8_t value);
uint8_t bin2bcd(uint8_t value);

class AB08XX
{
private:
	AB08XX_memorymap map;
	uint64_t error_code;
	void setError(uint64_t error_code);
public:
	AB08XX();
	time_t get();
	void read(tmElements_t &tm);
	void write(tmElements_t &tm);
	uint64_t getError();
	void readControl(control1 d);
};

#endif /* AB08XX_H_ */
