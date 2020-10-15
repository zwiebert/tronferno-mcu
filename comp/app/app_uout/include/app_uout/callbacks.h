/**
 * \file app_uout/callbacks.h
 */
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <app_uout/status_output.h>
#include <uout/uo_callbacks.h>
#include <type_traits>



inline const so_arg_pch_t *uoCb_pchFromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.pin_change && msg.flags.fmt.raw)
    return static_cast<const so_arg_pch_t *>(msg.cptr);
  return nullptr;
}
inline const so_arg_gmp_t *uoCb_gmpFromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.pct_change && msg.flags.fmt.raw)
    return static_cast<const so_arg_gmp_t *>(msg.cptr);
  return nullptr;
}

void uoApp_publish_pctChange_json(const char *json, bool fragment = true);
void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a, uo_flagsT tgtFlags = {});
void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a[], size_t len, uo_flagsT tgtFlags = {});

void uoApp_publish_timer_json(const char *json, bool fragment = true);

void uoApp_publish_fer_msgReceived(const struct Fer_MsgPlainCmd *msg);
