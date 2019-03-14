/*
 * status_output.h
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_STATUS_OUTPUT_H_
#define USER_MAIN_STATUS_OUTPUT_H_

#include <stdint.h>
//#include <stdbool.h>

typedef enum {
  SO_NONE,

  SO_CUAS_START, SO_CUAS_TIMEOUT, SO_CUAS_DONE,

  SO_PRAS_START_LISTENING, SO_PRAS_STOP_LISTENING, SO_PRAS_TIMEOUT, SO_PRAS_DONE,

  SO_CFG_all, SO_CFG_begin,
  SO_CFG_CU, SO_CFG_BAUD, SO_CFG_RTC, SO_CFG_WLAN_SSID, SO_CFG_LONGITUDE, SO_CFG_LATITUDE,
  SO_CFG_TIMEZONE, SO_CFG_DST, SO_CFG_TZ, SO_CFG_VERBOSE,
  SO_CFG_end, // values between begin/end will be used for 'all=?'
  SO_CFG_GPIO_PIN,

  SO_CFGPASSWD_OK, SO_CFGPASSWD_WRONG, SO_CFGPASSWD_MISSING,

  SO_TIMER_EVENT_PRINT,

  SO_TIMER_PRINT,


} so_msg_t;

typedef struct {
  uint8_t g, m;
} so_arg_gm_t;

void so_output_message(so_msg_t mt, void *arg);

#endif /* USER_MAIN_STATUS_OUTPUT_H_ */
