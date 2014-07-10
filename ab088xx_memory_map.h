/*
 * Real Time Clock driver for Abracon AB08XX series RTC
 *
 * Copyright (c) 2014 
 *   NigelB <nigel.blair@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License. See the file "COPYING" in the main directory of this
 * archive for more details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef AB08XX_MEMORY_MAP_H_
#define AB08XX_MEMORY_MAP_H_

#ifndef OFFSETOF
#define OFFSETOF(type, field)    ((unsigned long) &(((type *) 0)->field))
#endif

struct AB08XX_memorymap
{
	byte subseconds;
	byte seconds;
	byte hours_24;
	byte hours_12;
	byte date;
	byte month;
	byte years;
	byte weekdays;
	byte hundredths_alarm;
	byte seconds_alarm;
	byte minute_alarm;
	byte hour_alarm;
	byte date_alram;
	byte month_alarm;
	byte weekday_alarm;
	byte status;
	byte control1;
	byte control2;
	byte int_mask;
	byte sqw;
	byte cal_xt;
	byte cal_rc_hi;
	byte cal_rc_low;
	byte sleep_control;
	byte timer_control;
	byte timer;
	byte timer_initial;
	byte wdt;
	byte osc_control;
	byte osc_status;
	byte RESERVED;
	byte configuration_key;
	byte trickle;
	byte bref_control;
	byte RESERVED2[6];
//	byte RESERVED;
//	byte RESERVED;
//	byte RESERVED;
//	byte RESERVED;
//	byte RESERVED;
	byte id0;
	byte id1;
	byte id2;
	byte id3;
	byte id4;
	byte id5;
	byte id6;
	byte astat;
	byte octrl;
	byte extention_address;
	byte ram[64];
};



#endif /* AB08XX_MEMORY_MAP_H_ */
