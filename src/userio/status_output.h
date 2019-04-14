/*
 * status_output.h
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_STATUS_OUTPUT_H_
#define USER_MAIN_STATUS_OUTPUT_H_

#include <stdint.h>
#include <stdbool.h>
#include "positions/shutter_state.h"

// to select target for output messages
typedef enum {
  SO_TGT_NONE = 0,
  SO_TGT_CLI_USB = (1<<0),
  SO_TGT_CLI_TCP = (1<<1),
  SO_TGT_CLI = (SO_TGT_CLI_USB|SO_TGT_CLI_TCP),
  SO_TGT_HTTP = (1<<2),
  SO_TGT_MQTT = (1<<3),
  SO_TGT_ANY = (~0)
} so_target_bits;

extern uint8_t so_target;

#define so_tgt_set(bitmask) (so_target = (bitmask))
#define so_tgt_default() (so_tgt_set(SO_TGT_ANY))
#define so_tgt_test(bitmask) (so_target & (bitmask))

#ifndef DISTRIBUTION
//#define DEV_CLI_JSON
#endif

#ifdef DEV_CLI_JSON
#define so_tgt_test_cli_json() ((so_target & SO_TGT_CLI))
#define so_tgt_test_cli_text() false
#else
#define so_tgt_test_cli_text() ((so_target & SO_TGT_CLI))
#define so_tgt_test_cli_json() false
#endif

typedef enum {
  SO_NONE,

  SO_FW_START_MSG_PRINT,

  SO_SEP_ENABLE, SO_SEP_DISABLE, SO_SEP_BUTTON_PRESSED_ERROR,

  SO_CUAS_START, SO_CUAS_TIMEOUT, SO_CUAS_DONE,

  SO_PRAS_START_LISTENING, SO_PRAS_STOP_LISTENING, SO_PRAS_TIMEOUT, SO_PRAS_DONE,

  SO_CFG_all, SO_CFG_begin,
  SO_CFG_CU, SO_CFG_BAUD, SO_CFG_RTC, SO_CFG_WLAN_SSID, SO_CFG_WLAN_PASSWORD, SO_CFG_LONGITUDE, SO_CFG_LATITUDE,
  SO_CFG_TIMEZONE, SO_CFG_DST, SO_CFG_TZ, SO_CFG_VERBOSE,
  SO_CFG_MQTT_ENABLE, SO_CFG_MQTT_URL, SO_CFG_MQTT_USER, SO_CFG_MQTT_PASSWORD,
  SO_CFG_HTTP_ENABLE, SO_CFG_HTTP_USER, SO_CFG_HTTP_PASSWORD,
  SO_CFG_GM_USED,
  SO_CFG_end, // values between begin/end will be used for 'all=?'
  SO_CFG_GPIO_PIN,

  SO_CFGPASSWD_OK, SO_CFGPASSWD_WRONG, SO_CFGPASSWD_MISSING,

  SO_TIMER_EVENT_PRINT,
  SO_TIMER_PRINT,

  SO_POS_begin, SO_POS_PRINT_GMP, SO_POS_PRINT, SO_POS_PRINT_MMP, SO_POS_end,

  SO_PAIR_PRINT_AMM,

  SO_INET_PRINT_ADDRESS,


} so_msg_t;

typedef struct {
  uint8_t g, m;
} so_arg_gm_t;

typedef struct {
  uint8_t *mm; //gm_bitmask_t *
  uint8_t p;
} so_arg_mmp_t;

typedef struct {
  uint8_t g, m, p;
} so_arg_gmp_t;

typedef struct {
  uint32_t a;
  uint8_t *mm; //gm_bitmask_t *
} so_arg_amm_t;

void so_output_message(so_msg_t mt, void *arg);
so_msg_t so_parse_config_key(const char *k);

#endif /* USER_MAIN_STATUS_OUTPUT_H_ */
