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


typedef void (*voidFuncP)(const struct TargetDesc &td);

template<class T>
class so_object {
private:
  const struct TargetDesc *my_td;
  voidFuncP mEnd;
public:
  so_object(void (*begin)(const struct TargetDesc &td, T), T args, voidFuncP end, const struct TargetDesc &td) {
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
  const struct TargetDesc *my_td;
  voidFuncP mEnd;
public:
  so_object(voidFuncP begin, voidFuncP end, const struct TargetDesc &td)  {
    my_td = &td;
    mEnd = end;
    begin(td);
  }

  ~so_object() {
    mEnd(*my_td);
  }
};


#endif

