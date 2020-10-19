/**
 * \file   fernotron_trx/raw/fer_msg_attachment_enum.h
 * \brief  Enums for extended messages
 * \author bertw
 */

#pragma once

////////////// programming frame /////////////////////
//18 rows each 9 columns

/// Frame row 0. Each enum value represents a byte. 9 bytes total
enum fer_fpr00 {
  fpr0_RTC_secs,  ///<  RTC time
  fpr0_RTC_mint,  ///<  RTC time
  fpr0_RTC_hour,  ///<  RTC time
  fpr0_RTC_wdayMask, ///<  Sun...Sat = 0x01 ... 0x40 (each bit represents a weekday)
  fpr0_RTC_days,  ///<  day of month 1..31
  fpr0_RTC_mont,  ///<  month of year 1..12
  fpr0_RTC_wday, ///<  low nibble: Mon...Sun = 1...7.  high nibble is 0x0 (full program frame) or 0x8 (RTC only short program frame)
  fpr0_FlagBits, ///<  bits to enable SunAutomatic, Random, DST
  fpr0_checksum  ///< checksum of all bytes before in this message (including the plain message part, and including its checksum)
};

/// \brief values of fpr0_RTC_wdayMask
enum fer_rtc_wdayMaskBits {
  wd_SUN, wd_MON, wd_TUE, wd_WED, wd_THU, wd_FRI, wd_SAT
};

// weekday by bitmask in prg[0][fpr0_RTC_wdayMask]
// values of low nibble of fpr0_RTC_wday
enum fer_rtc_wday {
  wd2_MON = 1, wd2_TUE, wd2_WED, wd2_THU, wd2_FRI, wd2_SAT, wd2_SUN,
};

/// \brief values of fpr0_FlagBits.
enum fer_fpr00_FlagBitsValues {
  flag_Random, ///<  shutter opens/closes at random times (theft protection during longer absence). (FIXME: ignored ???)
  flag_1,  ///< ?
  flag_DST,  ///<  daylight saving time. if set, the shutter will add 1 hour to its saved astro time table. (never ignored)
  flag_3,  ///< ?
  flag_4,  ///< ?
  flag_5,  ///< ?
  flag_6,  ///< ?
  flag_SunAuto  ///<  enable automatic sun-sensor. if off the sun-down command does not work. (ignored at rtc-only frames)
};

// frame rows 1 - 4: timer

/// \brief frame row 1
enum fer_fpr01 {
  fpr1_T_sun_up_mint,     ///< weekly timer Sunday. (up, minute of hour)
  fpr1_T_sun_up_hour,     ///< weekly timer Sunday. (up, hour of day)
  fpr1_T_sun_down_mint,   ///< weekly timer Sunday. (down, minute of hour)
  fpr1_T_sun_down_hour,   ///< weekly timer Sunday. (down, hour of day)
  fpr1_T_mon_up_mint,     ///< weekly timer Monday. (up, minute of hour)
  fpr1_T_mon_up_hour,     ///< weekly timer Monday. (up, hour of day)
  fpr1_T_mon_down_mint,   ///< weekly timer Monday. (up, hour of day)
  fpr1_T_mon_down_hour,   ///< weekly timer Monday. (up, hour of day)
  fpr1_checksum  ///< checksum of all bytes before in this message (including the plain message part, and including all previous checksums)
};

/// \brief frame row 2
enum fer_fpr02 {
  fpr2_T_tue_up_mint,     ///< weekly timer Tuesday. (up, minute of hour)
  fpr2_T_tue_up_hour,     ///< weekly timer Tuesday. (up, hour of day)
  fpr2_T_tue_down_mint,   ///< weekly timer Tuesday. (down, minute of hour)
  fpr2_T_tue_down_hour,   ///< weekly timer Tuesday. (down, hour of day)
  fpr2_T_wed_up_mint,     ///< weekly timer Wednesday. (up, minute of hour)
  fpr2_T_wed_up_hour,     ///< weekly timer Wednesday. (up, hour of day)
  fpr2_T_wed_down_mint,   ///< weekly timer Wednesday. (down, minute of hour)
  fpr2_T_wed_down_hour,   ///< weekly timer Wednesday. (down, hour of day)
  fpr2_checksum ///< checksum of all bytes before in this message (including the plain message part, and including all previous checksums)
};
/// \brief frame row 3
enum fer_fpr03 {
  fpr3_T_thu_up_mint,     ///< weekly timer Thursday. (up, minute of hour)
  fpr3_T_thu_up_hour,     ///< weekly timer Thursday. (up, hour of day)
  fpr3_T_thu_down_mint,   ///< weekly timer Thursday. (down, minute of hour)
  fpr3_T_thu_down_hour,   ///< weekly timer Thursday. (down, hour of day)
  fpr3_T_fri_up_mint,     ///< weekly timer Friday. (up, minute of hour)
  fpr3_T_fri_up_hour,     ///< weekly timer Friday. (up, hour of day)
  fpr3_T_fri_down_mint,   ///< weekly timer Friday. (down, minute of hour)
  fpr3_T_fri_down_hour,   ///< weekly timer Friday. (down, hour of day)
  fpr3_checksum ///< checksum of all bytes before in this message (including the plain message part, and including all previous checksums)
};
/// \brief frame row 4
enum fer_fpr04 {
  fpr4_T_sat_up_mint,     ///< weekly timer Saturday. (up, minute of hour)
  fpr4_T_sat_up_hour,     ///< weekly timer Saturday. (up, hour of day)
  fpr4_T_sat_down_mint,   ///< weekly timer Saturday. (down, minute of hour)
  fpr4_T_sat_down_hour,   ///< weekly timer Saturday. (down, hour of day)
  fpr4_T_daily_up_mint,   ///< daily timer. (up, minute of hour)
  fpr4_T_daily_up_hour,   ///< daily timer. (up, hour of day)
  fpr4_T_daily_down_mint, ///< daily timer. (down, minute of hour)
  fpr4_T_daily_down_hour, ///< daily timer. (down, hour of day)
  fpr4_checksum ///< checksum of all bytes before in this message (including the plain message part, and including all previous checksums)
};



//////////////////////////////////////////////////////////////////////////

// frame rows 5 - 16: astro

//////////////////////////////////////////////////////////////////////////

/// \brief  frame row 17 (special last row/footer)
enum fer_fpr17 {
  fpr17_0,  ///< ?
  fpr17_Addr_2,  ///< ?
  fpr17_Addr_1,  ///< ?
  fpr17_Addr_0,  ///< ?
  fpr17_4,  ///< ?
  fpr17_5,  ///< ?
  fpr17_6,  ///< ?
  fpr17_7,  ///< ?
  fpr17_8  ///< ?
};

//////////////////////////////////////////////////////////////////////////

// sub frames

/**
 *  \brief Order of the weekly/daily timers
 *
 *  There are two timers per row (8 timers in 4 rows). This enum describes their order
 */
enum fer_fpr_tim {
  fpr_tim_sun, ///< Weekly Sunday in \link fer_fpr01 \endlink
  fpr_tim_mon, ///< Weekly Monday in \link fer_fpr01 \endlink
  fpr_tim_tue, ///< Weekly Tuesday in \link fer_fpr02 \endlink
  fpr_tim_wed, ///< Weekly Wednesday in \link fer_fpr02 \endlink
  fpr_tim_thu, ///< Weekly Thursday in \link fer_fpr03 \endlink
  fpr_tim_fri, ///< Weekly Friday in \link fer_fpr03 \endlink
  fpr_tim_sat, ///< Weekly Saturday in \link fer_fpr04 \endlink
  fpr_tim_daily ///< Daily in \link fer_fpr04 \endlink
};



