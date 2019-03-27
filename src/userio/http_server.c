/*
 * http_server.c
 *
 *  Created on: 26.03.2019
 *      Author: bertw
 */
#include "user_config.h"
#include "http_server.h"
#include <esp_system.h>
#include <string.h>
#include "cli/cli.h"

#define CMD_TAG "cmd="
#define CMD_TAG_LEN = (sizeof CMD_TAG - 1)

static bool startsWith(const char *s1, int s1_len, const char *s2) {
  size_t s2_len = strlen(s2);
  return s1_len >= s2_len && (0 == strncmp(s1, s2, s2_len));
}

static bool endsWith(const char *s1, int s1_len, const char *s2) {
  size_t s2_len = strlen(s2);
  return s1_len >= s2_len && (0 == strncmp(s1 + s1_len - s2_len, s2, s2_len));
}

void hts_query(hts_query_t qtype, const char *qstr, int qstr_len) {
  char *buf, *p;
#define cmd_len 20 // FIXME

  if (startsWith(qstr, qstr_len, CMD_TAG)) {
    qstr +=  CMD_TAG_LEN;
    qstr_len -= CMD_TAG_LEN;
    if ((p = buf = malloc(qstr_len + 1))) {
        memcpy(p, qstr, qstr_len);
        p += qstr_len;
        *p++ = '\0';
        ets_printf("query: %s\n", buf);
        for (p = buf; *p; ++p)
          if (*p == '&')
            *p = ' ';

        ets_printf("query: %s\n", buf);
        process_cmdline(buf);
        free(buf);
    }

  } else if ((buf = malloc(qstr_len + cmd_len + 2))) {
    p = strcpy(buf, "send ");
    p += strlen(p);
    memcpy(p, qstr, qstr_len);
    p += qstr_len;
    *p++ = '\0';
    ets_printf("query: %s\n", buf);
    for (p = buf; *p; ++p)
      if (*p == '&')
        *p = ' ';

    ets_printf("query: %s\n", buf);
    process_cmdline(buf);
    free(buf);
  }


}
