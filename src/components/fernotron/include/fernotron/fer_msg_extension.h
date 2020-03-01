/*
 * fer_frame.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_FRAME_H_
#define USER_MAIN_FER_FRAME_H_

#include <stdint.h>

#include "fer_msg_basic.h"
#include "fer_timings.h"

////////////// programming frame /////////////////////
//18 rows each 9 columns

/// entire frame
enum fpr0 {
  fpr0_RTC_secs, fpr0_RTC_mint, fpr0_RTC_hour,  // RTC time
  fpr0_RTC_wdayMask, // Sun...Sat = 0x01 ... 0x40 (each bit represents a weekday)
  fpr0_RTC_days,  // day of month 1..31
  fpr0_RTC_mont,  // month of year 1..12
  fpr0_RTC_wday, // low nibble: Mon...Sun = 1...7.  high nibble is 0x0 (full program frame) or 0x8 (RTC only short program frame)
  fpr0_FlagBits, // bits to enable SunAutomatic, Random, DST
  fpr0_checksum
};

// values of fpr0_FlagBits.
enum fpr0_FlagBitsValues { // FIXME: there may be more flag bits in use. try to find out
  flag_Random, // shutter opens/closes at random times (theft protection during longer absence). (FIXME: ignored ???)
  flag_1,  // ???
  flag_DST,  // daylight saving time. if set, the shutter will add 1 hour to its saved astro time table. (never ignored)
  flag_3,  //
  flag_4,  // ???
  flag_5,  // ???
  flag_6,  // ???
  flag_SunAuto  // enable automatic sun-sensor. if off the sun-down command does not work. (ignored at rtc-only frames)
};

// frame rows 1 - 4: timer
enum fpr1 {
  fpr1_T_sun_up_mint,
  fpr1_T_sun_up_hour,
  fpr1_T_sun_down_mint,
  fpr1_T_sun_down_hour,
  fpr1_T_mon_up_mint,
  fpr1_T_mon_up_hour,
  fpr1_T_mon_down_mint,
  fpr1_T_mon_down_hour,
  fpr1_checksum
};

enum fpr2 {
  fpr2_T_tue_up_mint,
  fpr2_T_tue_up_hour,
  fpr2_T_tue_down_mint,
  fpr2_T_tue_down_hour,
  fpr2_T_wed_up_mint,
  fpr2_T_wed_up_hour,
  fpr2_T_wed_down_mint,
  fpr2_T_wed_down_hour,
  fpr2_checksum
};

enum fpr3 {
  fpr3_T_thu_up_mint,
  fpr3_T_thu_up_hour,
  fpr3_T_thu_down_mint,
  fpr3_T_thu_down_hour,
  fpr3_T_fri_up_mint,
  fpr3_T_fri_up_hour,
  fpr3_T_fri_down_mint,
  fpr3_T_fri_down_hour,
  fpr3_checksum
};

enum fpr4 {
  fpr4_T_sat_up_mint,
  fpr4_T_sat_up_hour,
  fpr4_T_sat_down_mint,
  fpr4_T_sat_down_hour,
  fpr4_T_daily_up_mint,
  fpr4_T_daily_up_hour,
  fpr4_T_daily_down_mint,
  fpr4_T_daily_down_hour,
  fpr4_checksum
};

//////////////////////////////////////////////////////////////////////////

// frame rows 5 - 16: astro

//////////////////////////////////////////////////////////////////////////

// frame row 17

enum fpr17 {
  fpr17_0,
  fpr17_Addr_2,
  fpr17_Addr_1,
  fpr17_Addr_0,
  fpr17_4,
  fpr17_5,
  fpr17_6,
  fpr17_7,
  fpr17_8
};

//////////////////////////////////////////////////////////////////////////

// sub frames

// timer sub frame. There are 8 timer stamps (Up/Down).  7 are for weekly timer (one timer for each weekday) and 1 is for daily timer (for every day).
enum fpr_tim {
  fpr_tim_sun,
  fpr_tim_mon,
  fpr_tim_tue,
  fpr_tim_wed,
  fpr_tim_thu,
  fpr_tim_fri,
  fpr_tim_sat,
  fpr_tim_daily
};

// values of fpr0_RTC_wdayMask
enum wdays {
  wd_SUN, wd_MON, wd_TUE, wd_WED, wd_THU, wd_FRI, wd_SAT
};
// weekday by bitmask in prg[0][fpr0_RTC_wdayMask]
// values of low nibble of fpr0_RTC_wday
enum wday2 {
  wd2_MON = 1, wd2_TUE, wd2_WED, wd2_THU, wd2_FRI, wd2_SAT, wd2_SUN,
};
// weekday by number in prg[0][fpr0_RTC_wday]
// values of high nibble of fpr0_RTC_wday (bit flags?)
#define FLAG_TIMER_DATA 0x8

extern uint8_t bcd2dec(uint8_t bcd);

#define FPR_RTC_START_ROW  0
#define FPR_RTC_START_COL  0
#define FPR_RTC_WIDTH      8 // nmb of bytes used for RTC data
#define FPR_RTC_HEIGHT     1 // nmb of rows used for RTC data
#define FPR_RTC_BYTE_CT    3 // nmb of bytes in data (6 for SS:MM:HH)

#define FPR_ASTRO_START_ROW   5 // astro data start at this row in frame
#define FPR_ASTRO_WIDTH       8 // nmb of bytes used for astro data each row
#define FPR_ASTRO_HEIGHT     12 // nmb of rows used for astro data

#define FPR_TIMER_START_ROW     1 // timer data start at this row in frame
#define FPR_TIMER_WIDTH         8 // nmb of bytes used for timer data each row
#define FPR_TIMER_STAMP_WIDTH   4 // nmb of bytes used for a time stamp (4 bytes for up MM:HH down MM:HH)
#define FPR_TIMER_HEIGHT        4 // nmb of rows used for timer data

#define FPR_WEEKLY_START_ROW  1
#define FPR_WEEKLY_START_COL  0
#define FPR_WEEKLY_TIMER_STAMP_CT 7 // nmb of time stamps in data

#define FPR_DAILY_START_ROW  4
#define FPR_DAILY_START_COL  4
#define FPR_DAILY_TIMER_STAMP_CT 1 // nmb of time stamps in data

#define FPR_LAST_START_ROW 17
#define FPR_LAST_HEIGHT 1

#define FPR_DATA_HEIGHT 18
#define FPR_DATA_WIDTH 8
#define FPR_CS_WIDTH 1



typedef uint8_t cmd_byte_data[FER_CMD_BYTE_CT];
typedef uint8_t rtc_byte_data[FER_PRG_BYTE_CT];
typedef uint8_t last_byte_data[FER_PRG_BYTE_CT];
typedef uint8_t astro_byte_data[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT];
typedef uint8_t wdtimer_byte_data[FPR_TIMER_HEIGHT][FER_PRG_BYTE_CT];

typedef struct fer_msg {
  cmd_byte_data cmd;
  rtc_byte_data rtc;
  wdtimer_byte_data wdtimer;
  astro_byte_data astro;
  last_byte_data last;
}__attribute__((__packed__)) fer_msg;


#define BYTES_MSG_PLAIN  sizeof (cmd_byte_data)
#define BYTES_MSG_RTC    BYTES_MSG_PLAIN + sizeof (rtc_byte_data)
#define BYTES_MSG_TIMER  sizeof (fer_msg)


#endif /* USER_MAIN_FER_FRAME_H_ */
