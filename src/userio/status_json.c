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

void (*s_json_config_out)(const char *s);

#define JSON_BUF_SIZE 128
char json_buf[JSON_BUF_SIZE];
int json_idx;

void so_json_config_reply(const char *key, const char *val, bool is_number) {
  D(ets_printf("so_json(): %s, %s, %d\n", key, val, is_number));
  if (key == 0 || (json_idx + strlen(key) + strlen(val) + 6) > JSON_BUF_SIZE) {
    if (json_idx > 0) {
      strcpy(json_buf + json_idx, " } }");
      if (s_json_config_out)
        s_json_config_out(json_buf);
    }
    json_idx = 0;
    return;
  }

  if (json_idx == 0) {
    strcpy(json_buf, "{ \"name\": \"tfmcu\", \"config\": { ");
    json_idx = strlen(json_buf);
  }

  const char *quote = is_number ? "" : "\"";
  sprintf(json_buf + json_idx, "\"%s\": %s%s%s,", key, quote, val, quote);
  json_idx += strlen(json_buf+json_idx);
}

static void ICACHE_FLASH_ATTR sj_timer2json_buf(char *dst, uint8_t dst_size, uint8_t g, uint8_t m, bool wildcard) {
  timer_data_t tdr;
  extern gm_bitmask_t manual_bits; //FIXME
  // read_gm_bitmask("MANU", &manual_bits, 1); //FIXME: not needed
  bool f_manual = GET_BIT(manual_bits[g], m);

  if (read_timer_data(&tdr, &g, &m, wildcard)) {
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
      sprintf(dp, "{ \"name\": \"automatic\", \"auto%d%d\" :{ \"f\": \"%s\", \"random\": %d, \"sun-auto\": %d, \"manual\": %d",
              g, m, flags, (td_is_random(&tdr)), (td_is_sun_auto(&tdr)), f_manual);
#else
      sprintf(dp, "{ \"name\": \"automatic\", \"auto%d%d\" :{ \"f\": \"%s\"",
              g, m, flags);
#endif

      dp += strlen(dp);
    }

    if (td_is_daily(&tdr)) {
      sprintf(dp, ", \"daily\": \"%s\"", tdr.daily);
      dp += strlen(dp);
    }
    if (td_is_weekly(&tdr)) {
      sprintf(dp, ", \"weekly\": \"%s\"", tdr.weekly);
      dp += strlen(dp);
    }

    if (td_is_astro(&tdr)) {
      sprintf(dp, ", \"astro\": %d", tdr.astro);
      dp += strlen(dp);
    }

    strcat(dp, " } }");
  }
}

const char *ICACHE_FLASH_ATTR sj_timer2json(uint8_t g, uint8_t m) {
  sj_timer2json_buf(json_buf, JSON_BUF_SIZE, g, m, true);
  return json_buf;
}
