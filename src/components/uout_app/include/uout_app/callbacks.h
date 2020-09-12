#pragma once
#include <stdint.h>
#include <uout_app/status_output.h>
#include <uout/callbacks.h>
#include <type_traits>

void uoApp_publish_pinChange(const so_arg_pch_t *args);

inline const so_arg_pch_t *uoCb_pchFromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.pin_change && msg.flags.fmt.obj)
    return static_cast<const so_arg_pch_t *>(msg.cv_ptr);
  return nullptr;
}
