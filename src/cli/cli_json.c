/*
 * cli_json.c
 *
 *  Created on: 28.03.2019
 *      Author: bertw
 */

#ifndef TEST_MODULE
#include "user_config.h"
#include "config/config.h"
#include "string.h"
#include "cli_imp.h"
#include <ctype.h>
#include "debug/debug.h"
#include <stdlib.h>
#include "userio/status_output.h"

#else
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define postcond(x) assert((x))
#define ICACHE_FLASH_ATTR
int msgid;

typedef struct {
  char *key;
  char *val;
} clpar;
#define MAX_PAR 20
clpar par[MAX_PAR];
#endif

// like get command line:
static int parse_json_find_next_obj(const char *s, int *key, int *key_len, int *obj, int *obj_len) {
  int start = 0, i, k, n, e, c;

  restart: // jump-label to avoid avoid recursion
  {
    int q1 = -1, q2 = -1;

    for (n = start; s[n]; ++n) { // scan for object key
      if (s[n] != '"' && (n == 0 || s[n - 1] != '\\'))
        continue;
      if (q1 == -1) {
        q1 = n;
        continue;
      }
      q2 = n;

      for (i = n; s[i]; ++i) { // scan for key/val separator ':'
        if (s[i] != ':')
          continue;

        for (k = i + 1; s[k]; ++k) { // scan for object start '{'
          if (isspace((int )s[k]))
            continue;
          if (s[k] == '{') {
            if (key)
              *key = q1;
            if (key_len)
              *key_len = 1 + (q2 - q1);
            if (obj)
              *obj = k;
            if (obj_len) {
              *obj_len = -1;
              for (e = k + 1; s[e]; ++e) {
                if (s[e] == '}') { // FIXME: should ignore bracket in quotes. Should check for nested opening brackets too.
                  *obj_len = 1 + (e - k);
                  break;
                }
              }
            }
            postcond(!key || s[*key] == '"');
            postcond(!key || !key_len || s[*key + *key_len - 1] == '"');
            postcond(!obj || s[*obj] == '{');
            postcond(!obj || !obj_len || s[*obj + *obj_len - 1] == '}');
            return n; // return index of object key
          } else {
            for (c = k; s[c]; ++c) {
              if (s[c] == ',') { // FIXME: should ignore comma in quotes
                start = c + 1;
                goto restart;
              }
            }
          }
        }
      }
    }
  }
  return -1;
}

char *json_get_command_object(char *s, char **ret_name) {
  int key, key_len, obj, obj_len;

  int idx = parse_json_find_next_obj(s, &key, &key_len, &obj, &obj_len);

  if (idx >= 0 && obj_len > 0) {
    char *name = &s[key + 1]; // set to name after quote sign
    s[key + key_len - 1] = '\0'; // terminate name excluding quote sign;
    char *block = &s[obj + 1]; // set to char after '{';
    s[obj + obj_len - 1] = '\0'; // terminate obj excluding '}'

    if (ret_name)
      *ret_name = name;
    return block;
  }
  return NULL;
}

int ICACHE_FLASH_ATTR
parse_json(char *name, char *s) {
  int p;
  msgid = 0;
  int start = 0, i, k, n;
  int result = 1;
  par[0].key = name;
  par[0].val = "";

  for (p = 1; p < MAX_PAR; ++p) {
    par[p].key = 0;
    par[p].val = 0;
    int q1 = -1, q2 = -1;
    int v1 = -1, v2 = -1;

    for (n = start; s[n]; ++n) { // scan for object key
      if (s[n] != '"' && (n == 0 || s[n - 1] != '\\'))
        continue;
      if (q1 == -1) {
        q1 = n;
        continue;
      }
      q2 = n;

      for (i = n; s[i]; ++i) { // scan for key/val separator ':'
        if (s[i] != ':')
          continue;

        for (k = i + 1; s[k]; ++k) { // scan for objectval start
          if (isspace((int )s[k]))
            continue;

          bool is_quoted = (s[k] == '"');
          v1 = k;
          for (k += 1;; ++k) {

            if (s[k] == '\0'
                || (is_quoted && s[k] == '"' && s[k - 1] != '\\')
                || (!is_quoted && (isspace((int)s[k]) || s[k] == ','))) {
              v2 = k;

              for (start = k; s[start]; ++start) {
                if (s[start] == ',') {
                  ++start;
                  break;
                }
              }

              par[p].key = &s[q1 + 1];
              s[q2] = '\0';
              par[p].val = &s[is_quoted ? v1 + 1 : v1];
              s[v2] = '\0';

              if (strcmp(par[p].key, "mid") == 0) {
                msgid = atoi(par[p].val);
                --p;
              } else {
                ++result;
              }
              goto next_par;
            }
            if (s[k] == '\0')
              return result;
          }
        }
      }
    }
    next_par: ;

  }
  return result;
}

#ifndef TEST_MODULE

void ICACHE_FLASH_ATTR
cli_process_json(char *json) {
  dbg_vpf(db_printf("process_json: %s\n", json));
  {
    char *name;
    char *cmd_obj = json_get_command_object(json, &name);

    if (cmd_obj) {
      int n = parse_json(name, cmd_obj);
      if (n < 0) {
        reply_failure();
      } else {
        process_parm(par, n);
      }
    }
  }
}

void ICACHE_FLASH_ATTR
cli_print_json(const char *json) {
  if (so_tgt_test_cli_json())
    io_puts(json), io_putlf();
}

#else

//char json[] = "{ \"to\": \"tfmcu\", \"config\": { \"cu\": \"801234\",\"baud\": 115200,\"longitude\": 13.5,\"latitude\": 52.6,\"timezone\": 1.0,\"tz\": \"\",\"verbose\": 5,\"mqtt-enable\": 1,\"mqtt-url\": \"mqtt://192.168.1.42:7777\",\"mqtt-user\": \"mqusr\",\"mqtt-password\": \"mqpw\" } }";

char json[] = "{\"to\":\"tfmcu\",\"timer\":{\"g\":1,\"m\":0,\"f\":\"imDwArs\",\"daily\":\"0700\",\"astro\":0}}";
int main() {
  char *name;
  char *cmd_obj = json_get_command_object(json, &name);

  printf("name: %s, obj: %s\n", name, cmd_obj);

  if (cmd_obj) {
    int len = parse_json(name, cmd_obj);
    for (int i = 0; i < len; ++i)
    printf("key: %s, val: %s\n", par[i].key, par[i].val);

  }

  return 0;
}
#endif
