/**
 * \file fernotron_uout/fer_uo_publish.h
 * \brief publish fernotron_app state
 */
#pragma once
#include <uout/uo_callbacks.h>

#include <type_traits>
#include <stdint.h>
#include <stddef.h>

#define uo_evt_flag_PRAS uo_evt_flag_F
#define uo_evt_flag_CUAS uo_evt_flag_E




typedef struct {
  uint8_t g, m;
  struct shutter_timings *st;
} so_arg_gmt_t;


typedef struct {
  uint8_t g, m;
} so_arg_gm_t;

typedef struct {
  const struct Fer_GmSet *mm;
  uint8_t p;
} so_arg_mmp_t;

typedef struct {
  uint8_t g, m, p;
} so_arg_gmp_t;

typedef struct {
  uint32_t a;
  uint8_t g, m;
} so_arg_agm_t;

typedef struct {
  uint32_t a;
  struct Fer_GmSet *mm;
} so_arg_amm_t;

typedef struct {
  const char *key;
  struct Fer_GmSet *mm;
} so_arg_kmm_t;

typedef struct {
  uint32_t a;
  bool scanning:1;
  bool success:1;
  bool timeout:1; ///< true if timeout reached
  bool pairing:1; ///< true for pairing, false for unpairing
} so_arg_pras_t;

typedef struct {
  uint32_t a;
  bool scanning:1;
  bool success:1;
  bool timeout:1; ///< true if timeout reached
} so_arg_cuas_t;



void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a, uo_flagsT tgtFlags = {});
void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a[], size_t len, uo_flagsT tgtFlags = {});
void uoApp_publish_timer_json(const char *json, bool fragment = true);
void uoApp_publish_fer_msgReceived(const struct Fer_MsgPlainCmd *msg);
void uoApp_publish_fer_prasState(const so_arg_pras_t args);
void uoApp_publish_fer_cuasState(const so_arg_cuas_t args);



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
