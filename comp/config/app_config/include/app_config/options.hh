/**
 * \file      app_config/options.hh
 * \brief     defines strings used in user interface
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

typedef const char *const_cstringT;

#ifdef CONFIG_APP_USE_GPIO_PINS
constexpr const_cstringT pin_mode_args = "diqQoO";
constexpr const_cstringT pin_level_args = "lh";
constexpr const_cstringT pin_state_args = "?01t";
#endif

#ifdef CONFIG_APP_USE_NETWORK
using cfg_args_networkT = const char *[5];
constexpr cfg_args_networkT cfg_args_network = { "none", "wlan", "ap", "lan", "lan-wlan" };
#endif

#ifdef CONFIG_APP_USE_LAN
using cfg_args_lanPhyT = const char *[4];
constexpr cfg_args_lanPhyT cfg_args_lanPhy = { "none", "lan8720", "rtl8201", "ip101", };
#endif

using cfg_args_rfTrxT = const char *[4];
constexpr cfg_args_rfTrxT cfg_args_rfTrx = { "none", "rx-tx", "rx-itx", "cc1101", };

typedef int8_t otokBaseT;
#if 1
/// \brief options as enum (matching the string options in \link otok_strings \endlink )
enum class otok : otokBaseT  {
  NONE = -1,  // not existing
      /////////// begin of config keys //////////////////
      k_cu, k_baud, k_rtc, //
      k_network, k_wlan_ssid, k_wlan_password, k_ntp_server, k_lan_phy, k_lan_pwr_gpio, //
      k_longitude, k_latitude, k_timezone, k_dst, k_tz, k_verbose, //
      k_mqtt_enable, k_mqtt_url, k_mqtt_user, k_mqtt_password, k_mqtt_client_id, k_mqtt_root_topic, //
      k_http_enable, k_http_user, k_http_password, //
      k_gm_used, //
      k_astro_correction, //
      k_rf_tx_pin, k_rf_rx_pin, k_set_button_pin, k_gpio, //
      k_rf_miso_pin, k_rf_mosi_pin, k_rf_sclk_pin, k_rf_ss_pin, // CC1101 SPI
      k_cc1101_config,
      k_rf_repeater,
      k_rtc_aupd,
      ///////////// end of config keys /////////////////

  k_a, k_g, k_m, k_mm, k_c, //
  k_restart, k_all, k_cuas, k_set_pw, k_receiver, k_transmitter, k_rf_trx, // config
  k_r, k_p, k_SEP, // cmd
  k_boot_count, k_print, k_kvs_pk, k_tm, k_am, k_stack, k_te, k_dbp, k_cs, k_up_time, k_version, k_ota, // mcu

  k_weekly, k_daily, k_astro, k_rtc_only, k_random, k_sun_auto, k_f, // auto

  k_id_list, k_id_add, k_id_rm, // repeater

  k_enable, k_timeout, k_request_auth, k_auth_key, // SEP
  SIZE // key array size
};


/// \brief options as null terminated strings (matching the enum options in \link otok \endlink )
constexpr const_cstringT otok_strings[] = {
     /////////// begin of config keys //////////////////
    "cu", "baud", "rtc", //
    "network", "wlan-ssid", "wlan-password", "ntp-server", "lan-phy", "lan-pwr-gpio", //
    "longitude", "latitude", "timezone", "dst", "tz", "verbose", //
    "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-client-id", "mqtt-root-topic", //
    "http-enable", "http-user", "http-password", //
    "gm-used", //
    "astro-correction", //
    "rf-tx-pin", "rf-rx-pin", "set-button-pin", "gpio", //
    "rf-miso-pin", "rf-mosi-pin", "rf-sclk-pin", "rf-ss-pin", //CC1101 SPI
    "cc1101-config",
    "rf-repeater" ,
    "rtc-auto-upd-enable",
    ///////////// end of config keys /////////////////
    "a", "g", "m", "mm", "c",  //
    "restart", "all", "cuas", "set-pw", "receiver", "transmitter", "rf-trx", // config
    "r", "p", "SEP", // cmd
    "boot-count", "print", "kvs-pk", "tm", "am", "stack", "te", "dbp", "cs", "up-time", "version", "ota", // mcu

    "weekly", "daily", "astro", "rtc-only", "random", "sun-auto", "f", // auto

    "id-list", "id-add", "id-rm", // repeater

    "enable", "timeout", "request-auth", "auth-key", // SEP
    };
#else
#include "generated_opts.hh"
#endif
static_assert(!((sizeof otok_strings / sizeof otok_strings[0]) > (size_t)otok::SIZE)); // too many strings
static_assert(!((sizeof otok_strings / sizeof otok_strings[0]) < (size_t)otok::SIZE)); // not enought strings (missing comma?)
