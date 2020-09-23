/*
 * fer_msg_attachment_enums.h
 *
 *  Created on: 19.03.2020
 *      Author: bertw
 */

#pragma once

////////////// programming frame /////////////////////
//18 rows each 9 columns

/// entire frame
enum fer_fpr0 {
  fpr0_RTC_secs, fpr0_RTC_mint, fpr0_RTC_hour,  // RTC time
  fpr0_RTC_wdayMask, // Sun...Sat = 0x01 ... 0x40 (each bit represents a weekday)
  fpr0_RTC_days,  // day of month 1..31
  fpr0_RTC_mont,  // month of year 1..12
  fpr0_RTC_wday, // low nibble: Mon...Sun = 1...7.  high nibble is 0x0 (full program frame) or 0x8 (RTC only short program frame)
  fpr0_FlagBits, // bits to enable SunAutomatic, Random, DST
  fpr0_checksum
};

// values of fpr0_RTC_wdayMask
enum fer_rtc_wdayMaskBits {
  wd_SUN, wd_MON, wd_TUE, wd_WED, wd_THU, wd_FRI, wd_SAT
};

// weekday by bitmask in prg[0][fpr0_RTC_wdayMask]
// values of low nibble of fpr0_RTC_wday
enum fer_rtc_wday {
  wd2_MON = 1, wd2_TUE, wd2_WED, wd2_THU, wd2_FRI, wd2_SAT, wd2_SUN,
};

// values of fpr0_FlagBits.
enum fer_fpr0_FlagBitsValues {
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
enum fer_fpr1 {
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

enum fer_fpr2 {
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

enum fer_fpr3 {
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

enum fer_fpr4 {
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

enum fer_fpr17 {
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
enum fer_fpr_tim {
  fpr_tim_sun,
  fpr_tim_mon,
  fpr_tim_tue,
  fpr_tim_wed,
  fpr_tim_thu,
  fpr_tim_fri,
  fpr_tim_sat,
  fpr_tim_daily
};



