/*
 * fer_frame.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_FRAME_H_
#define USER_MAIN_FER_FRAME_H_

#include "fer_frame.h"

#include <stdint.h>
#include "fer_code.h"
#include "fer_timings.h"

typedef enum {
  MSG_TYPE_NONE, MSG_TYPE_PLAIN, MSG_TYPE_RTC, MSG_TYPE_TIMER
} fmsg_type;

typedef u8 cmd_byte_data[FER_CMD_BYTE_CT];
typedef u8 rtc_byte_data[FER_PRG_BYTE_CT];
typedef u8 last_byte_data[FER_PRG_BYTE_CT];
typedef u8 astro_byte_data[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT];
typedef u8 wdtimer_byte_data[FPR_TIMER_HEIGHT][FER_PRG_BYTE_CT];

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
