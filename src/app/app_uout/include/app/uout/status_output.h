/*
 * status_output.h
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "uout/status_output.h"
#include <app/uout/so_types.h>
#include <app/uout/so_msg.h>
#include <app/uout/so_config.h>


#define SO_CFG_size (SO_CFG_end - SO_CFG_begin - 1)



void so_broadcast_message(so_msg_t mt, void *arg);
void so_output_message(so_msg_t mt, const void *arg);
void so_output_message_tgt(so_msg_t mt, void *arg, so_target_bits tgt);

so_msg_t so_parse_config_key(const char *k);


#ifdef __cplusplus


typedef void (*voidFuncP)(void);

template<class T>
class so_object {
private:
  voidFuncP mEnd;
public:
  so_object(void (*begin)(T), T args, voidFuncP end) {
    mEnd = end;
    begin(args);
  }
  so_object(voidFuncP begin, voidFuncP end) {
    mEnd = end;
    begin();
  }
  ~so_object() {
    mEnd();
  }
};

template<>
class so_object<void> {
private:
  voidFuncP mEnd;
public:
  so_object(voidFuncP begin, voidFuncP end) {
    mEnd = end;
    begin();
  }

  ~so_object() {
    mEnd();
  }
};


#endif

