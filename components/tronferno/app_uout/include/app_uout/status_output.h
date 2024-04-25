/**
 * \file app_uout/status_output.h
 *
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "uout/uo_types.h"
#include <app_uout/so_types.h>
#include <app_uout/so_msg.h>
#include <app_uout/so_config.h>


#define SO_CFG_size (SO_CFG_end - SO_CFG_begin - 1)

#ifdef __cplusplus


typedef void (*voidFuncP)(class UoutWriter &td);

template<class T>
class so_object {
private:
  class UoutWriter *my_td;
  voidFuncP mEnd;
public:
  so_object(void (*begin)(class UoutWriter &td, T), T args, voidFuncP end, class UoutWriter &td) {
    my_td = &td;
    mEnd = end;
    begin(td, args);
  }
  ~so_object() {
    mEnd(*my_td);
  }
};

template<>
class so_object<void> {
private:
  class UoutWriter *my_td;
  voidFuncP mEnd;
public:
  so_object(voidFuncP begin, voidFuncP end, class UoutWriter &td)  {
    my_td = &td;
    mEnd = end;
    begin(td);
  }

  ~so_object() {
    mEnd(*my_td);
  }
};


#endif

