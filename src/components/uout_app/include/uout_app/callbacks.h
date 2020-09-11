#pragma once
#include <stdint.h>
#include <uout_app/status_output.h>
#include <uout/callbacks.h>
#include <type_traits>

enum class uoAppCb_msgTypes : uoCb_msgTypeT { NONE, pch = UOCB_MT(END), END };
enum class uoAppCB_msgFlagBits : uoCb_flagT { pch = UOCB_MFB(END), END };

#define UOACB_MT(v) static_cast<std::underlying_type<uoAppCb_msgTypes>::type>(uoAppCb_msgTypes:: v)
#define UOACB_MFB(v) static_cast<std::underlying_type<uoAppCB_msgFlagBits>::type>(uoAppCB_msgFlagBits:: v)

inline bool operator==(uoAppCb_msgTypes lhs, uoCb_msgTypeT rhs) {
  return static_cast<uoCb_msgTypeT>(lhs) == rhs;
}

void uoApp_event_pinChange(const so_arg_pch_t *args);

inline const so_arg_pch_t *uoCb_pchFromMsg(const uoCb_msgT *msg) {
  if (UOACB_MT(pch) == msg->msg_type)
    return static_cast<const so_arg_pch_t *>(msg->cv_ptr);
  return nullptr;
}
