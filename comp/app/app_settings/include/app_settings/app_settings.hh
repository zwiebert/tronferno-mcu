/**
 * \file app_settings/app_settings.hh
 * \brief Settings for application components
 */
#pragma once
#include "app_config/proj_app_cfg.h"
#include <app_config/options.hh>
#include <app_uout/so_config.h>
#include <utils_misc/int_macros.h>
#include "config.h"
#include <config_kvs/comp_settings.hh>
#include <config_kvs/settings.hh>
#include <assert.h>

enum configAppItem : i8 {
  CBA_NONE = -1,
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

constexpr u32 CBM_gpio = BIT(CB_RFIN_GPIO) | BIT(CB_RFOUT_GPIO) | BIT(CB_SETBUTTON_GPIO) | BIT(CB_GPIO);
constexpr u32 CBM_geo = BIT(CB_LONGITUDE) | BIT(CB_LATITUDE) | BIT(CB_TZ) | BIT(CB_ASTRO_CORRECTION);

class AppSettings: public Settings<configAppItem, CBA_size - CB_size, CB_size> {
public:
  using Base = Settings<configAppItem, CBA_size - CB_size, CB_size>;
  using Item = configAppItem;
  using storeFunT = void (*)(configAppItem item, const char *val);
public:
  constexpr AppSettings() {
    initField(CB_RECV, "C_RECEIVER", otok::k_receiver, CBT_i8);
    initField(CB_TRANSM, "C_TRANSM", otok::k_transmitter, CBT_i8);
    initField(CB_CUID, "C_CUID", otok::k_cu, CBT_u32, soCfg_CU);
    initField(CB_USED_MEMBERS, "C_GMU", otok::k_gm_used, CBT_u32, soCfg_GM_USED, STF_direct_hex);
    initField(CB_BAUD, "C_BAUD", otok::k_baud, CBT_u32, soCfg_BAUD, STF_direct);
    initField(CB_GPIO, "C_GPIO", otok::k_gpio, CBT_str);
    initField(CB_CFG_PASSWD, "C_CFG_PASSWD", otok::NONE, CBT_str, 0, STF_direct);
    initField(CB_LONGITUDE, "C_LONGITUDE", otok::k_longitude, CBT_f, soCfg_LONGITUDE, STF_direct);
    initField(CB_LATITUDE, "C_LATITUDE", otok::k_latitude, CBT_f, soCfg_LATITUDE, STF_direct);
#ifndef USE_POSIX_TIME
#error
#else
    initField(CB_TZ, "C_TZ", otok::k_tz, CBT_str, soCfg_TZ, STF_direct);
#endif
    initField(CB_ASTRO_CORRECTION, "C_AST_COR", otok::k_astro_correction, CBT_i8, soCfg_ASTRO_CORRECTION, STF_direct);
#ifdef USE_NETWORK
    initField(CB_NETWORK_CONNECTION, "C_NW_CONN", otok::k_network, CBT_i8, soCfg_NETWORK);
#endif
    initField(CB_RFOUT_GPIO, "C_RFOUTP", otok::k_rf_tx_pin, CBT_i8, soCfg_GPIO_RFOUT, STF_direct);
    initField(CB_RFIN_GPIO, "C_RFINP", otok::k_rf_rx_pin, CBT_i8, soCfg_GPIO_RFIN, STF_direct);
    initField(CB_SETBUTTON_GPIO, "C_SETBTNP", otok::k_set_button_pin, CBT_i8, soCfg_GPIO_SETBUTTON, STF_direct);
  }

};

constexpr AppSettings app_settings;

constexpr const char* settings_get_kvsKey(configAppItem item) {
  return app_settings.get_kvsKey(item);
}
constexpr KvsType settings_get_kvsType(configAppItem item) {
  return app_settings.get_kvsType(item);
}
constexpr otok settings_get_optKey(configAppItem item) {
  return app_settings.get_optKey(item);
}
constexpr const char* settings_get_optKeyStr(configAppItem item) {
  return app_settings.get_optKeyStr(item);
}
constexpr AppSettings::Base::soCfgFunT settings_get_soCfgFun(configAppItem item) {
  return app_settings.get_soCfgFun(item);
}
constexpr AppSettings::Base::soCfgFunT settings_get_soCfgFun(otok opt_key) {
  if (auto res = app_settings.get_soCfgFun(opt_key))
    return res;
  return comp_settings.get_soCfgFun(opt_key);
}
constexpr StoreFun settings_get_storeFunOk(otok opt_key) {
  if (auto res = app_settings.get_storeFun(opt_key))
    return res;
  return comp_settings.get_storeFun(opt_key);
}
bool config_item_modified(enum configAppItem item);
