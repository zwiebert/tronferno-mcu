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
#define uo_evt_flag_msgSent uo_evt_flag_D
#define uo_evt_flag_SEP uo_evt_flag_C
#define uo_evt_flag_pctChange uo_evt_flag_B
#define uo_evt_flag_rfMsgReceived uo_evt_flag_A
#define uo_evt_flag_timerChange uo_evt_flag_10





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

typedef struct {
  bool auth_success:1; ///< true if user pressed hardware button successful
  bool auth_button_timeout:1; ///< true if timeout before pressing hardware button
  bool auth_button_error:1; ///< true if timeout before pressing hardware button
  bool auth_timeout:1; ///< true if authentication timed out
  bool auth_terminated: 1; ///< authentication terminated
  bool ui_timeout:1;  ///< true if timeout after user interaction
} so_arg_sep_t;


#include "so_msg.h"

void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a, uo_flagsT tgtFlags = {});
void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a[], size_t len, uo_flagsT tgtFlags = {});
void uoApp_publish_timer_json(const char *json, bool fragment = true);
struct Fer_TimerData;
void uoApp_publish_timer_json(uint8_t g, uint8_t m, struct Fer_TimerData *tda);
void uoApp_publish_fer_msgReceived(const struct Fer_MsgPlainCmd *msg);
void uoApp_publish_fer_msgRtcReceived(const struct Fer_MsgPlainCmd &c, const struct fer_raw_msg &m);
void uoApp_publish_fer_msgAutoReceived(const struct Fer_MsgPlainCmd &c, const struct fer_raw_msg &m);
void uoApp_publish_fer_msgSent(const struct Fer_MsgPlainCmd *msg);
void uoApp_publish_fer_msgRtcSent(const struct Fer_MsgPlainCmd &c, const struct fer_raw_msg &m);
void uoApp_publish_fer_msgAutoSent(const struct Fer_MsgPlainCmd &c, const struct fer_raw_msg &m);
void uoApp_publish_fer_prasState(const so_arg_pras_t args);
void uoApp_publish_fer_cuasState(const so_arg_cuas_t args);
void uoApp_publish_fer_sepState(const so_arg_sep_t args, char tag = '\0');

inline const so_arg_pch_t *uoCb_pchFromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.pin_change && msg.flags.fmt.raw)
    return static_cast<const so_arg_pch_t *>(msg.cptr);
  return nullptr;
}
inline const so_arg_gmp_t *uoCb_gmpFromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.uo_evt_flag_pctChange && msg.flags.fmt.raw)
    return static_cast<const so_arg_gmp_t *>(msg.cptr);
  return nullptr;
}

