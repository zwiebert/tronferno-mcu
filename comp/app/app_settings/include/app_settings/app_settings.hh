#pragma once
#include "app_config/proj_app_cfg.h"
#include <app_config/options.hh>
#include "config.h"
#include <config_kvs/comp_settings.hh>
#include <assert.h>

enum configAppItem {
  CBA_start = CB_size - 1, //
  CB_RECV, CB_TRANSM, CB_CUID, CB_USED_MEMBERS, CB_BAUD, CB_GPIO, CB_CFG_PASSWD, CB_LONGITUDE, CB_LATITUDE,
#ifndef USE_POSIX_TIME
CB_TIZO,
#else
  CB_TZ,
#endif
#ifdef MDR_TIME
  CB_DST,
#endif
  CB_ASTRO_CORRECTION,
#ifdef USE_NETWORK
  CB_NETWORK_CONNECTION,
#endif
  CB_RFOUT_GPIO, CB_RFIN_GPIO, CB_SETBUTTON_GPIO,

//-----------
  CBA_size
};

class AppSettings: CompSettings {

  constexpr void initField(const configAppItem ai, const char *const kvsKey, const otok optKey, const KvsType kvsType) {
    unsigned idx = ai - CB_size;
    as_optKeys[idx] = optKey;
    as_kvsKeys[idx] = kvsKey;
    as_kvsTypes[idx] = kvsType;
  }
public:
  constexpr AppSettings() {
    initField(CB_RECV, "C_RECEIVER", otok::k_receiver, CBT_i8);
    initField(CB_TRANSM, "C_TRANSM", otok::k_transmitter, CBT_i8);
    initField(CB_CUID, "C_CUID", otok::k_cu, CBT_u32);
    initField(CB_USED_MEMBERS, "C_GMU", otok::k_gm_used, CBT_u32);
    initField(CB_BAUD, "C_BAUD", otok::k_baud, CBT_u32);
    initField(CB_GPIO, "C_GPIO", otok::k_gpio, CBT_str);
    initField(CB_CFG_PASSWD, "C_CFG_PASSWD", otok::NONE, CBT_str);
    initField(CB_LONGITUDE, "C_LONGITUDE", otok::k_longitude, CBT_f);
    initField(CB_LATITUDE, "C_LATITUDE", otok::k_latitude, CBT_f);
#ifndef USE_POSIX_TIME
#error
#else
    initField(CB_TZ, "C_TZ", otok::k_tz, CBT_str);
#endif
    initField(CB_ASTRO_CORRECTION, "C_AST_COR", otok::k_astro_correction, CBT_i8);
#ifdef USE_NETWORK
    initField(CB_NETWORK_CONNECTION, "C_NW_CONN", otok::k_network, CBT_i8);
#endif
    initField(CB_RFOUT_GPIO, "C_RFOUTP", otok::k_rf_tx_pin, CBT_i8);
    initField(CB_RFIN_GPIO, "C_RFINP", otok::k_rf_rx_pin, CBT_i8);
    initField(CB_SETBUTTON_GPIO, "C_SETBTNP", otok::k_set_button_pin, CBT_i8);
  }

public:

  constexpr const char* get_kvsKey(configAppItem idx) const {
    return as_kvsKeys[idx - CB_size];
  }
  constexpr KvsType get_kvsType(configAppItem idx) const {
    return as_kvsTypes[idx - CB_size];
  }

  constexpr otok get_optKey(configAppItem idx) const {
    return as_optKeys[idx - CB_size];
  }
  constexpr const char* get_optKeyStr(configAppItem idx) const {
    return otok_strings[static_cast<otokBaseT>(as_optKeys[idx - CB_size])];
  }

public:

  constexpr const char* get_kvsKey(configItem idx) const {
    return CompSettings::get_kvsKey(idx);
  }
  constexpr KvsType get_kvsType(configItem idx) const {
    return CompSettings::get_kvsType(idx);
  }

  constexpr otok get_optKey(configItem idx) const {
    return CompSettings::get_optKey(idx);
  }
  constexpr const char* get_optKeyStr(configItem idx) const {
    return CompSettings::get_optKeyStr(idx);
  }

public:
  const char *as_kvsKeys[CBA_size - CB_size] { };
  otok as_optKeys[CBA_size - CB_size] { };
  KvsType as_kvsTypes[CBA_size - CB_size] { };
};

constexpr AppSettings app_settings;

template<typename Kvs_Type>
constexpr Kvs_Type config_read_item(const configAppItem item, Kvs_Type def) {
  switch (app_settings.get_kvsType(item)) {
  case CBT_u32:
    return static_cast<Kvs_Type>(config_read_item_u32(app_settings.get_kvsKey(item), (u32) def));
  case CBT_i8:
    return static_cast<Kvs_Type>(config_read_item_i8(app_settings.get_kvsKey(item), (i8) def));
  case CBT_f:
    return static_cast<Kvs_Type>(config_read_item_f(app_settings.get_kvsKey(item), (float) def));
  default:
    assert(!"unhandled type");
  }
  return def;
}

template<typename Kvs_Type>
constexpr const Kvs_Type* config_read_item(configAppItem item, Kvs_Type *d, size_t d_size, const Kvs_Type *def) {
  switch (app_settings.get_kvsType(item)) {
  case CBT_str:
    return config_read_item_s(app_settings.get_kvsKey(item), d, d_size, (const char*) def);
  default:
    assert(!"unhandled type");
  }
  return def;
}

inline const char* cfg_key(configAppItem item) {
  return app_settings.get_kvsKey(item);
}
inline const char* cfg_optStr(configAppItem item) {
  return app_settings.get_optKeyStr(item);
}
bool config_item_modified(enum configAppItem item);
