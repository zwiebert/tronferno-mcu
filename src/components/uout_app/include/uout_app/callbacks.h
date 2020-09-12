#pragma once
#include <stdint.h>
#include <uout_app/status_output.h>
#include <uout/callbacks.h>
#include <type_traits>

void uoApp_publish_pinChange(const so_arg_pch_t args);

inline const so_arg_pch_t *uoCb_pchFromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.pin_change && msg.flags.fmt.obj)
    return static_cast<const so_arg_pch_t *>(msg.cv_ptr);
  return nullptr;
}
inline const so_arg_gmp_t *uoCb_gmpFromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.pct_change && msg.flags.fmt.obj)
    return static_cast<const so_arg_gmp_t *>(msg.cv_ptr);
  return nullptr;
}

void uoApp_publish_gmpJson(const char *json);
void uoApp_publish_gmpObj(const so_arg_gmp_t a);
void uoApp_publish_timerJson(const char *json);
