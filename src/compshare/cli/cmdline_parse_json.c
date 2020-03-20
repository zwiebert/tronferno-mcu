/*
 * cli_json.c
 *
 *  Created on: 28.03.2019
 *      Author: bertw
 */

#include "cli/cli_json.h"
#include "cli/cli.h"

#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "string.h"

#include <ctype.h>
#include "debug/debug.h"
#include <stdlib.h>
#include "userio/status_json.h"
#include "cli/mutex.h"


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

char *json_get_command_object(char *s, char **ret_name, char **next) {
  int key, key_len, obj, obj_len;

  int idx = parse_json_find_next_obj(s, &key, &key_len, &obj, &obj_len);

  if (idx >= 0 && obj_len > 0) {
    char *name = &s[key + 1]; // set to name after quote sign
    s[key + key_len - 1] = '\0'; // terminate name excluding quote sign;
    char *block = &s[obj + 1]; // set to char after '{';
    s[obj + obj_len - 1] = '\0'; // terminate obj excluding '}'

    if (next)
      *next = &s[obj + obj_len];

    if (ret_name)
      *ret_name = name;
    return block;
  }
  return NULL;
}

int
parse_json(char *name, char *s) {
  int p;
  cli_msgid = 0;
  int start = 0, i, k, n;
  int result = 1;
  cli_par[0].key = name;
  cli_par[0].val = "";

  for (p = 1; p < MAX_PAR; ++p) {
    cli_par[p].key = 0;
    cli_par[p].val = 0;
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

              cli_par[p].key = &s[q1 + 1];
              s[q2] = '\0';
              cli_par[p].val = &s[is_quoted ? v1 + 1 : v1];
              s[v2] = '\0';

              if (strcmp(cli_par[p].key, "mid") == 0) {
                cli_msgid = atoi(cli_par[p].val);
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


void 
cli_print_json(const char *json) {
    io_puts(json), io_putlf();
}

