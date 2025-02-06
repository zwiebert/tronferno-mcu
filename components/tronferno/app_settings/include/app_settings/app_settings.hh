/**
 * \file app_settings/app_settings.hh
 * \brief Settings for application components
 */
#pragma once

#include <app_config/options.hh>
#include <app_uout/so_config.h>
#include <utils_misc/int_macros.h>
#include "config.h"
#include "config_kvs/comp_settings.hh"
#include <config_kvs/settings_template.hh>
#include <assert.h>




enum configAppItem : int8_t {
  CBA_NONE = -1,
  CBA_start = CB_size - 1, //
  CB_RECV, CB_TRANSM, CB_CUID, CB_USED_MEMBERS, CB_BAUD, CB_GPIO, CB_CFG_PASSWD, CB_LONGITUDE, CB_LATITUDE,
  CB_RF_TRX,
#ifndef CONFIG_APP_USE_POSIX_TIME
CB_TIZO,
#else
  CB_TZ,
#endif
  CB_ASTRO_CORRECTION,
#ifdef CONFIG_APP_USE_NETWORK
  CB_NETWORK_CONNECTION,
#endif
  CB_RFOUT_GPIO, CB_RFIN_GPIO, CB_SETBUTTON_GPIO,
  CB_RFMISO_GPIO, CB_RFMOSI_GPIO, CB_RFSCK_GPIO, CB_RFSS_GPIO,
  CB_CC1101_CONFIG,
#ifdef CONFIG_APP_USE_REPEATER
  CB_RF_REPEATER,
#endif
#ifdef CONFIG_APP_USE_RTC_AUTO_UPD
  CB_RTC_AUTO_UPD,
#endif
//-----------
  CBA_size
};

extern const SettingsBase<configAppItem> &app_sett;


constexpr uint32_t CBM_gpio = BIT(CB_RFIN_GPIO) | BIT(CB_RFOUT_GPIO) | BIT(CB_SETBUTTON_GPIO) | BIT(CB_GPIO) | BIT(CB_RF_TRX);
constexpr uint32_t CBM_geo = BIT(CB_LONGITUDE) | BIT(CB_LATITUDE) | BIT(CB_TZ) | BIT(CB_ASTRO_CORRECTION);
constexpr uint64_t CBM_cc1101 = BIT64(CB_RF_TRX) | BIT64(CB_CC1101_CONFIG) | BIT64(CB_RF_TRX) | BIT64(CB_RFSCK_GPIO) | BIT64(CB_RFMISO_GPIO) | BIT64(CB_RFMOSI_GPIO) | BIT64(CB_RFSS_GPIO) | BIT(CB_RFIN_GPIO) | BIT(CB_RFOUT_GPIO);
#ifdef CONFIG_APP_USE_REPEATER
constexpr uint64_t CBM_rf_repeater = BIT64(CB_RF_REPEATER);
#endif


bool config_item_modified(enum configAppItem item);

