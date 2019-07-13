/*
 * status_json.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */


#include "user_config.h"
#include "status_json.h"

#include "automatic/timer_data.h"
#include "userio/status_output.h"

#include <string.h>
#include <stdio.h>

#include "automatic/timer_state.h"
#include "cli/cli_imp.h" // FIXME?
#include "config/config.h"
#include "main/common.h"
#include "main/pairings.h"
#include "main/rtc.h"
#include "misc/int_macros.h"
#include "userio/inout.h"
#include "userio/mqtt.h"

#define D(x)
#ifdef MCU_ESP8266
#define sprintf(...) (-1)  //FIXME: or should we use it?
#endif

void (*s_json_config_out)(const char *s);

#define JSON_BUF_SIZE 128
char json_buf[JSON_BUF_SIZE];
int json_idx;

char *ext_buf;
uint16_t ext_buf_size;

#define BUF (ext_buf ? ext_buf : json_buf)
#define BUF_SIZE (ext_buf ? ext_buf_size : JSON_BUF_SIZE)
#define USE_CALLBACK (!ext_buf && s_json_config_out)
#define DO_CALLBACK() s_json_config_out(BUF);

void  ICACHE_FLASH_ATTR sj_set_buf(char *dst, uint16_t dst_size) {
  if (dst) {
    *dst = '\0';
    ext_buf = dst;
    ext_buf_size = dst_size;
    json_idx = 0;
  } else {
    ext_buf = 0;
    ext_buf_size = 0;
  }
}

static const char *Obj_tag="";

void  ICACHE_FLASH_ATTR so_json_set_x(const char *tag) {
  Obj_tag = tag;
}

void  ICACHE_FLASH_ATTR so_json_x_reply(const char *key, const char *val, bool is_number) {
  D(ets_printf("so_json(): %s, %s, %d\n", key, val, is_number));

  if (key == 0 || ((json_idx + strlen(key) + strlen(val) + 6)) > BUF_SIZE) {
    D(ets_printf("json buffer overflow: idx=%u, buf_size=%u\n", json_idx, BUF_SIZE));
    if (json_idx > 0) {
      if (BUF[json_idx - 1] == ',') { // remove trailing comma...
        --json_idx;
      }
      strcpy(BUF + json_idx, "}}");
      if (USE_CALLBACK)
        DO_CALLBACK();
      cli_print_json(BUF);
    }
    json_idx = 0;
    if (key == 0)
      return; // ...if done then exit
    //...if overflow then continue
  }



  if (json_idx == 0) {
    sprintf(BUF, "{\"from\":\"tfmcu\",\"%s\":{", Obj_tag);
    json_idx = strlen(BUF);
  }

  const char *quote = is_number ? "" : "\"";
  sprintf(BUF + json_idx, "\"%s\":%s%s%s,", key, quote, val, quote);
  json_idx += strlen(BUF+json_idx);
  D(ets_printf("json_idx: %u, buf: %s\n", json_idx, BUF));
}

int ICACHE_FLASH_ATTR sj_config2json_buf(char *dst, uint16_t dst_size, so_msg_t key) {

  sj_set_buf(dst,dst_size);
  so_output_message(SO_CFG_all, "j");
  sj_set_buf(0,0);

  return strlen(dst); //XXX
}

void ICACHE_FLASH_ATTR sj_timer2json_buf(char *dst, uint16_t dst_size, uint8_t g, uint8_t m, bool wildcard) {
  timer_data_t tdr;
  extern gm_bitmask_t manual_bits; //FIXME
  // read_gm_bitmask("MANU", &manual_bits, 1); //FIXME: not needed
  bool f_manual = GET_BIT(manual_bits[g], m);
  uint8_t g_res = g, m_res = m;

  if (read_timer_data(&tdr, &g_res, &m_res, wildcard)) {
    char *dp = dst;
    {
      char flags[10], *p = flags;

      *p++ = f_manual ? 'M' : 'm';
      *p++ = td_is_random(&tdr) ? 'R' : 'r';
      *p++ = td_is_sun_auto(&tdr) ? 'S' : 's';
      *p++ = td_is_astro(&tdr) ? 'A' : 'a';
      *p++ = td_is_daily(&tdr) ? 'D' : 'd';
      *p++ = td_is_weekly(&tdr) ? 'W' : 'w';
      *p++ = '\0';
#ifdef SJ_AUTO_VERBOSE
      sprintf(dp, "{\"from\":\"tfmcu\",\"auto%d%d\":{\"f\":\"%s\",\"random\": %d,\"sun-auto\":%d,\"manual\":%d",
              g, m, flags, (td_is_random(&tdr)), (td_is_sun_auto(&tdr)), f_manual);
#else
      sprintf(dp, "{\"from\":\"tfmcu\",\"auto%d%d\":{\"f\":\"%s\"",
              g, m, flags);
#endif

      dp += strlen(dp);
    }

    if (td_is_daily(&tdr)) {
      sprintf(dp, ",\"daily\":\"%s\"", tdr.daily);
      dp += strlen(dp);
    }
    if (td_is_weekly(&tdr)) {
      sprintf(dp, ",\"weekly\":\"%s\"", tdr.weekly);
      dp += strlen(dp);
    }

    if (td_is_astro(&tdr)) {
      sprintf(dp, ",\"astro\":%d", tdr.astro);
      dp += strlen(dp);
    }

    strcat(dp, "}}");
  }
}

const char *ICACHE_FLASH_ATTR sj_timer2json(uint8_t g, uint8_t m) {
  sj_timer2json_buf(BUF, BUF_SIZE, g, m, true);
  return json_buf;
}

char *sj_gmp2json_buf(char *dst, uint16_t dst_size, so_arg_gmp_t *gmp) {
  sprintf(dst, "{\"from\":\"tfmcu\",\"position\":{\"g\":%u,\"m\":%u,\"p\":%u}}", gmp->g, gmp->m, gmp->p);
  return dst;
}

const char *sj_gmp2json(so_arg_gmp_t *gmp) {
  return sj_gmp2json_buf(BUF, BUF_SIZE, gmp);
}


