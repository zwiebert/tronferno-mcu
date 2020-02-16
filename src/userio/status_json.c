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

void (*sj_callback_onClose_ifNotEmpty)(const char *s);

#define JSON_BUF_SIZE 256
char json_buf[JSON_BUF_SIZE];
int json_idx;

char *ext_buf;
u16 ext_buf_size;

#define BUF (ext_buf ? ext_buf : json_buf)
#define BUF_SIZE (ext_buf ? ext_buf_size : JSON_BUF_SIZE)
#define USE_CALLBACK (!ext_buf && sj_callback_onClose_ifNotEmpty)
#define DO_CALLBACK() sj_callback_onClose_ifNotEmpty(BUF);

char *sj_get_json() { return BUF; }

bool sj_alloc_buffer(size_t buf_size) {
  precond(!ext_buf);
  ext_buf_size = buf_size;
  ext_buf = malloc(buf_size);
  json_idx = 0;
  if (!ext_buf)
    return false;
  ext_buf[0] = '\0';
  return true;;
}

void sj_free_buffer() {
  precond(ext_buf);
  free(ext_buf);
  ext_buf = 0;
  ext_buf_size = 0;
}

static void start_json(const char *dictionary_name) {
  sprintf(BUF, "{\"from\":\"tfmcu\",\"%s\":{", dictionary_name);
  json_idx = strlen(BUF);
}

static void terminate_json() {
  precond(json_idx >0);
  if (BUF[json_idx - 1] == ',') { // remove trailing comma...
    --json_idx;
  }
  strcpy(BUF + json_idx, "}}");
  json_idx = 0;
}

void   sj_open_dictionary(const char *dictionary_name) {
  start_json(dictionary_name);
}

void sj_close_dictionary() {
  precond(json_idx);
  terminate_json();

  if (USE_CALLBACK)
    DO_CALLBACK();

sj_callback_onClose_ifNotEmpty = 0;

}



bool   sj_append_to_dictionary(const char *key, const char *val, bool is_number) {
  D(ets_printf("so_json(): %s, %s, %d\n", key, val, is_number));
  precond(json_idx > 0);
  precond(key);

  //precond(sj_callback_onClose_ifNotEmpty || ext_buf);

  if ((json_idx + strlen(key) + strlen(val) + 6) > BUF_SIZE) {
    D(ets_printf("json buffer overflow: idx=%u, buf_size=%u\n", json_idx, BUF_SIZE));
    return false;
  }


  const char *quote = is_number ? "" : "\"";
  sprintf(BUF + json_idx, "\"%s\":%s%s%s,", key, quote, val, quote);
  json_idx += strlen(BUF+json_idx);
  D(ets_printf("json_idx: %u, buf: %s\n", json_idx, BUF));
  return true;
}

void  sj_fillBuf_with_automaticData(char *dst, u16 dst_size, u8 g, u8 m, bool wildcard) {
  timer_data_t tdr;
  extern gm_bitmask_t manual_bits; //FIXME
  // read_gm_bitmask("MANU", &manual_bits, 1); //FIXME: not needed
  bool f_manual = GET_BIT(manual_bits[g], m);
  u8 g_res = g, m_res = m;

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
      timer_minutes_t tmi;
      get_timer_minutes(&tmi, &g_res, &m_res, false);
      sprintf(dp, ",\"astro\":%d,\"asmin\":%d", tdr.astro, tmi.minutes[ASTRO_MINTS]);
      dp += strlen(dp);
    }

    strcat(dp, "}}");
  }
}

const char * sj_json_from_automaticData(u8 g, u8 m) {
  sj_fillBuf_with_automaticData(BUF, BUF_SIZE, g, m, true);
  return json_buf;
}

char *sj_fillBuf_with_positionData(char *dst, u16 dst_size, so_arg_gmp_t *gmp) {
  sprintf(dst, "{\"from\":\"tfmcu\",\"position\":{\"g\":%u,\"m\":%u,\"p\":%u}}", gmp->g, gmp->m, gmp->p);
  return dst;
}

const char *sj_json_from_postionData(so_arg_gmp_t *gmp) {
  return sj_fillBuf_with_positionData(BUF, BUF_SIZE, gmp);
}


