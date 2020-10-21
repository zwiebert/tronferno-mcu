/**
 * \file   fernotron_trx/raw/fer_msg_attachment.h
 * \brief  Extended messages
 * \author bertw
 */

#pragma once

#include <stdint.h>

#include "fer_msg_plain.h"
//#include "fer_radio_timings.h"
#include "fer_msg_attachment_enum.h"
#include <utils_misc/bcd.h>

#define FER_PRG_BYTE_CT          9
#define FER_PRG_PACK_CT         18   ///< 18 lines per program-frame
#define FER_RTC_PACK_CT          1   ///< 1 line per rtc-frame

#define FER_BYTES_MSG_PLAIN  sizeof (fer_cmd_bd)
#define FER_BYTES_MSG_RTC    FER_BYTES_MSG_PLAIN + sizeof (fer_rtc_bd)
#define FER_BYTES_MSG_TIMER  sizeof (fer_rawMsg)

// weekday by number in prg[0][fpr0_RTC_wday]
// values of high nibble of fpr0_RTC_wday (bit flags?)
#define FER_FLAG_TIMER_DATA 0x8

#define FER_FPR_RTC_START_ROW  0
#define FER_FPR_RTC_START_COL  0
#define FER_FPR_RTC_WIDTH      8 ///< nmb of bytes used for RTC data
#define FER_FPR_RTC_HEIGHT     1 ///< nmb of rows used for RTC data
#define FER_FPR_RTC_BYTE_CT    3 ///< nmb of bytes in data (6 for SS:MM:HH)

#define FER_FPR_ASTRO_START_ROW   5 ///< astro data start at this row in frame
#define FER_FPR_ASTRO_WIDTH       8 ///< nmb of bytes used for astro data each row
#define FER_FPR_ASTRO_HEIGHT     12 ///< nmb of rows used for astro data

#define FER_FPR_TIMER_START_ROW     1 ///< timer data start at this row in frame
#define FER_FPR_TIMER_WIDTH         8 ///< nmb of bytes used for timer data each row
#define FER_FPR_TIMER_STAMP_WIDTH   4 ///< nmb of bytes used for a time stamp (4 bytes for up MM:HH down MM:HH)
#define FER_FPR_TIMER_HEIGHT        4 ///< nmb of rows used for timer data

#define FER_FPR_WEEKLY_START_ROW  1
#define FER_FPR_WEEKLY_START_COL  0
#define FER_FPR_WEEKLY_TIMER_STAMP_CT 7 ///< nmb of time stamps in data

#define FER_FPR_DAILY_START_ROW  4
#define FER_FPR_DAILY_START_COL  4
#define FER_FPR_DAILY_TIMER_STAMP_CT 1 ///< nmb of time stamps in data

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

/**
 * \brief  Structured RTC message row (\link fer_fpr00 \endlink). Programs the devices internal RTC.
 */
struct __attribute__((__packed__)) fer_rtc_sd {
  uint8_t secs; ///< seconds
  uint8_t mint; ///< minutes
  uint8_t hour; ///< hour
  uint8_t wDayMask; ///< week-day bit-mask Bits defined in: \link fer_rtc_wdayMaskBits \endlink
  uint8_t days; ///<day of month 1..31
  uint8_t mont; ///< month of year 1..12
  uint8_t wday; ///< week-day. Number defined in: \link fer_rtc_wday \endlink (XXX: separate low and high nibble)
  union {
    uint8_t bd; ///<  Flag-bits as byte value
    struct {
      uint8_t random :1; ///<  Enable random timer
      uint8_t unused_1 :1; ///<
      uint8_t dst :1; ///<  Enable daylight saving time
      uint8_t unused_3 :1; ///<
      uint8_t unused_4 :1; ///<
      uint8_t unused_5 :1; ///<
      uint8_t unused_6 :1; ///<
      uint8_t sunAuto :1; ///<  Enable sun-automatic (sun-sensor)
    }  bits; ///< Flag-bits as bitfield
  }  flags; ///< Flags (\link fer_fpr00_FlagBitsValues \endlink)
  uint8_t checkSum; ///< Sum of all previous bytes in message
};

/// \brief RTC row
union fer_rtc {
  fer_rtc_bd bd; ///< RTC row as byte data
  struct fer_rtc_sd sd; ///< RTC row as structured data
};

/// \brief  Minute/Hour pair used in timers
struct fer_time {
  uint8_t mint, hour;
};

/// \brief Up/Down pair used in timers
struct fer_timer {
  struct fer_time up; ///<
  struct fer_time down; ///<
};

/// \brief A row containing 2 timers
struct fer_timer_row {
  struct fer_timer timers[2]; ///< 2 timers
  uint8_t checkSum; ///< Sum of all previous bytes in message
};

/**
 * \brief Block of weekly/daily timer rows
 */
union __attribute__((__packed__)) fer_wdtimer {
  fer_wdtimer_bd bd; ///< Weekly/Daily timers as byte data

  struct fer_timer_row rows_arr[4]; ///< Weekly/Daily timers as row-array

  struct  {
    struct fer_timer_row sun_mon, tue_wed, thu_fri, sat_daily; ///<
  }  rows; ///< Weekly/Daily timer rows as structured data

  struct {
    struct fer_timer sun, mon; ///<  2 Weekday timers
    uint8_t cs0; ///< Sum of all previous bytes in message
    struct fer_timer tue, wed; ///<  2 Weekday timers
    uint8_t cs1; ///< Sum of all previous bytes in message
    struct fer_timer thu, fri; ///<  2 Weekday timers
    uint8_t cs2; ///< Sum of all previous bytes in message
    struct fer_timer sat, daily; ///<  Weekday and daily timer
    uint8_t cs3; ///< Sum of all previous bytes in message
  }  days; ///< Weekly/Daily timers as structured data
};

/// \brief Astro row
union __attribute__((__packed__)) fer_astro_row {
  uint8_t bd[FER_PRG_BYTE_CT]; ///<  Astro row as byte data
  struct  {
    struct fer_time times[4]; ///<  Each row contains 4 wall times
    uint8_t checkSum; ///< Sum of all previous bytes in message
  }  sd; ///< Astro row as structured data
};

/// \brief Block of astro rows
union fer_astro {
  fer_astro_bd bd; ///< Astro block as byte data
  union fer_astro_row rows[FER_FPR_ASTRO_HEIGHT]; ///< Astro block as row array
};

typedef uint8_t fer_last_byte_data[FER_PRG_BYTE_CT]; ///<
typedef uint8_t fer_astro_byte_data[FER_FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT]; ///<

/**
 * \brief  Full size raw message
 */
typedef struct __attribute__((__packed__)) fer_raw_msg {
  union fer_cmd_row cmd; ///<  Plain message
  union fer_rtc rtc; ///< RTC Message
  union fer_wdtimer wdtimer; ///<  Weekly/Daily timers
  fer_astro_byte_data astro; ///<  Astro timers
  fer_last_byte_data last; ///< Last row \link fer_fpr17 \endlink (special/unknown values)
}  fer_rawMsg;

#ifdef __cplusplus
static_assert(sizeof(fer_raw_msg) == FER_CMD_BYTE_CT + FER_PRG_PACK_CT * FER_PRG_BYTE_CT, "wrong msg size");
#endif

