/*
 * fer_frame.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_FRAME_H_
#define USER_MAIN_FER_FRAME_H_

#include <stdint.h>

#include "fer_msg_plain.h"
#include "fer_radio_timings.h"
#include "fer_msg_attachment_enum.h"

#define FER_PRG_BYTE_CT          9
#define FER_PRG_PACK_CT         18   // 18 lines per program-frame
#define FER_RTC_PACK_CT          1   // 1 line per rtc-frame

#define BYTES_MSG_PLAIN  sizeof (fer_cmd_bd)
#define BYTES_MSG_RTC    BYTES_MSG_PLAIN + sizeof (fer_rtc_bd)
#define BYTES_MSG_TIMER  sizeof (fer_rawMsg)

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

typedef uint8_t fer_cmd_bd[FER_CMD_BYTE_CT];
typedef uint8_t fer_rtc_bd[FER_PRG_BYTE_CT];
typedef uint8_t last_byte_data[FER_PRG_BYTE_CT];
typedef uint8_t fer_astro_bd[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT];
typedef uint8_t fer_wdtimer_bd[FPR_TIMER_HEIGHT][FER_PRG_BYTE_CT];

struct fer_rtc_sd {
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
    } __attribute__((__packed__)) bits;
  } __attribute__((__packed__)) flags;
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

union fer_wdtimer {
  fer_wdtimer_bd bd;

  struct fer_timer_row rows_arr[4];

  struct {
    struct fer_timer_row sun_mon, tue_wed, thu_fri, sat_daily;
  } __attribute__((__packed__)) rows;

  struct {
    struct fer_timer sun, mon;
    uint8_t cs0;
    struct fer_timer tue, wed;
    uint8_t cs1;
    struct fer_timer thu, fri;
    uint8_t cs2;
    struct fer_timer sat, daily;
    uint8_t cs3;
  } __attribute__((__packed__)) days;
};

union fer_astro_row {
  uint8_t bd[FER_PRG_BYTE_CT];
  struct {
    struct fer_time times[4];
    uint8_t checkSum;
  } __attribute__((__packed__)) sd;
};

union fer_astro {
  fer_astro_bd bd;
  union fer_astro_row rows[FPR_ASTRO_HEIGHT];
};

typedef uint8_t last_byte_data[FER_PRG_BYTE_CT];
typedef uint8_t astro_byte_data[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT];

typedef struct fer_raw_msg {
  union fer_cmd_row cmd;
  union fer_rtc rtc;
  union fer_wdtimer wdtimer;
  astro_byte_data astro;
  last_byte_data last;
} __attribute__((__packed__)) fer_rawMsg;

#endif /* USER_MAIN_FER_FRAME_H_ */
