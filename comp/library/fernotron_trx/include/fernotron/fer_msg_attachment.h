/*
 * fer_frame.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#pragma once

#include <stdint.h>

#include "fer_msg_plain.h"
#include "fer_radio_timings.h"
#include "fer_msg_attachment_enum.h"
#include <misc/bcd.h>

#define FER_PRG_BYTE_CT          9
#define FER_PRG_PACK_CT         18   // 18 lines per program-frame
#define FER_RTC_PACK_CT          1   // 1 line per rtc-frame

#define FER_BYTES_MSG_PLAIN  sizeof (fer_cmd_bd)
#define FER_BYTES_MSG_RTC    FER_BYTES_MSG_PLAIN + sizeof (fer_rtc_bd)
#define FER_BYTES_MSG_TIMER  sizeof (fer_rawMsg)

// weekday by number in prg[0][fpr0_RTC_wday]
// values of high nibble of fpr0_RTC_wday (bit flags?)
#define FER_FLAG_TIMER_DATA 0x8

#define FER_FPR_RTC_START_ROW  0
#define FER_FPR_RTC_START_COL  0
#define FER_FPR_RTC_WIDTH      8 // nmb of bytes used for RTC data
#define FER_FPR_RTC_HEIGHT     1 // nmb of rows used for RTC data
#define FER_FPR_RTC_BYTE_CT    3 // nmb of bytes in data (6 for SS:MM:HH)

#define FER_FPR_ASTRO_START_ROW   5 // astro data start at this row in frame
#define FER_FPR_ASTRO_WIDTH       8 // nmb of bytes used for astro data each row
#define FER_FPR_ASTRO_HEIGHT     12 // nmb of rows used for astro data

#define FER_FPR_TIMER_START_ROW     1 // timer data start at this row in frame
#define FER_FPR_TIMER_WIDTH         8 // nmb of bytes used for timer data each row
#define FER_FPR_TIMER_STAMP_WIDTH   4 // nmb of bytes used for a time stamp (4 bytes for up MM:HH down MM:HH)
#define FER_FPR_TIMER_HEIGHT        4 // nmb of rows used for timer data

#define FER_FPR_WEEKLY_START_ROW  1
#define FER_FPR_WEEKLY_START_COL  0
#define FER_FPR_WEEKLY_TIMER_STAMP_CT 7 // nmb of time stamps in data

#define FER_FPR_DAILY_START_ROW  4
#define FER_FPR_DAILY_START_COL  4
#define FER_FPR_DAILY_TIMER_STAMP_CT 1 // nmb of time stamps in data

#define FER_FPR_LAST_START_ROW 17
#define FER_FPR_LAST_HEIGHT 1

#define FER_FPR_DATA_HEIGHT 18
#define FER_FPR_DATA_WIDTH 8
#define FER_FPR_CS_WIDTH 1

typedef uint8_t fer_cmd_bd[FER_CMD_BYTE_CT];
typedef uint8_t fer_rtc_bd[FER_PRG_BYTE_CT];
typedef uint8_t fer_last_byte_data[FER_PRG_BYTE_CT];
typedef uint8_t fer_astro_bd[FER_FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT];
typedef uint8_t fer_wdtimer_bd[FER_FPR_TIMER_HEIGHT][FER_PRG_BYTE_CT];

struct __attribute__((__packed__)) fer_rtc_sd {
  uint8_t secs, mint, hour;
  uint8_t wDayMask;
  uint8_t days, mont, wday;
  union {
    uint8_t bd;
    struct {
      uint8_t random :1;
      uint8_t unused_1 :1;
      uint8_t dst :1;
      uint8_t unused_3 :1;
      uint8_t unused_4 :1;
      uint8_t unused_5 :1;
      uint8_t unused_6 :1;
      uint8_t sunAuto :1;
    }  bits;
  }  flags;
  uint8_t checkSum;
};

union fer_rtc {
  fer_rtc_bd bd;
  struct fer_rtc_sd sd;
};

struct fer_time {
  uint8_t mint, hour;
};

struct fer_timer {
  struct fer_time up;
  struct fer_time down;
};

struct fer_timer_row {
  struct fer_timer timers[2];
  uint8_t checkSum;
};

union __attribute__((__packed__)) fer_wdtimer {
  fer_wdtimer_bd bd;

  struct fer_timer_row rows_arr[4];

  struct  {
    struct fer_timer_row sun_mon, tue_wed, thu_fri, sat_daily;
  }  rows;

  struct {
    struct fer_timer sun, mon;
    uint8_t cs0;
    struct fer_timer tue, wed;
    uint8_t cs1;
    struct fer_timer thu, fri;
    uint8_t cs2;
    struct fer_timer sat, daily;
    uint8_t cs3;
  }  days;
};

union __attribute__((__packed__)) fer_astro_row {
  uint8_t bd[FER_PRG_BYTE_CT];
  struct  {
    struct fer_time times[4];
    uint8_t checkSum;
  }  sd;
};

union fer_astro {
  fer_astro_bd bd;
  union fer_astro_row rows[FER_FPR_ASTRO_HEIGHT];
};

typedef uint8_t fer_last_byte_data[FER_PRG_BYTE_CT];
typedef uint8_t fer_astro_byte_data[FER_FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT];

typedef struct __attribute__((__packed__)) fer_raw_msg {
  union fer_cmd_row cmd;
  union fer_rtc rtc;
  union fer_wdtimer wdtimer;
  fer_astro_byte_data astro;
  fer_last_byte_data last;
}  fer_rawMsg;

#ifdef __cplusplus
static_assert(sizeof(fer_raw_msg) == FER_CMD_BYTE_CT + FER_PRG_PACK_CT * FER_PRG_BYTE_CT, "wrong msg size");
#endif

