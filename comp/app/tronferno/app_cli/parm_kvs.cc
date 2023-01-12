/*
 * cli_kvs.c
 *
 *  Created on: 12.06.2019
 *      Author: bertw
 */


#include "app_uout/status_output.h"
#include "app_settings/config.h"
#include "app_misc/rtc.h"
#include <uout/cli_out.h>
#include "cli_internal.hh"
#include "include/app_cli/cli_app.hh"
#include "../app_misc/include/app_misc/kvstore.hh"
#include "debug/dbg.h"
#include <ctype.h>
#include <string.h>

#define ENABLE_RESTART 1 // allow software reset

const char cli_help_parmKvs[] = "'kvs' stores general key value pairs\n\n"
    "key=(value|?)      Set or get value of key\n"
    ;

int process_parmKvs(clpar p[], int len, const struct TargetDesc &td) {
  int arg_idx;
  int errors = 0;

  soMsg_KVS_begin(td);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    if (key == NULL || val == NULL) {  // don't allow any default values
      ++errors;
      continue;
    }

    if (*val == '?') {
      if (!soMsg_KVS_print(td, key)) {
        td.so().print(key, "");
      }
      continue;
    }
    if (*val) {
      if (!kvs_store_string(key, val)) {
        ++errors;
      } else {
        if (!soMsg_KVS_print(td, key)) {
          td.so().print(key, "");
        }
      }
    } else {
      kvs_erase_key(key);
      td.so().print(key, "");
    }
    continue;

  }

  soMsg_KVS_end(td);
  cli_replyResult(td, errors == 0);
  return 0;
}

