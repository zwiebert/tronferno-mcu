#pragma once
#include "proj_app_cfg.h"
#include <stdint.h>

typedef const char *const_cstringT;

#ifdef ACCESS_GPIO
constexpr const_cstringT pin_mode_args = "diqQoO";
constexpr const_cstringT pin_level_args = "lh";
constexpr const_cstringT pin_state_args = "?01t";
#endif

#ifdef USE_NETWORK
using cfg_args_networkT = const char *[4];
constexpr cfg_args_networkT cfg_args_network = { "none", "wlan", "ap", "lan" };
#endif

#ifdef USE_LAN
using cfg_args_lanPhyT = const char *[4];
constexpr cfg_args_lanPhyT cfg_args_lanPhy = { "none", "lan8270", "rtl8201", "ip101", };
#endif


typedef int8_t otokBaseT;
enum class otok : otokBaseT  {
  NONE = -1,  // not existing
      //// config keys ///
      cu, baud, rtc, //
      network, wlan_ssid, wlan_password, ntp_server, lan_phy, lan_pwr_gpio, //
      longitude, latitude, timezone, dst, tz, verbose, //
      mqtt_enable, mqtt_url, mqtt_user, mqtt_password, mqtt_client_id, //
      http_enable, http_user, http_password, //
      gm_used, //
      astro_correction, //
      rf_tx_pin, rf_rx_pin, set_button_pin, gpio, //
      /// end of config keys ///

  a, g, m, mm, c, //
  restart, all, cuas, set_pw, receiver, transmitter, // config
  r, p, SEP, // cmd
  boot_count, print, kvs_pk, tm, am, stack, te, dbp, cs, up_time, version, ota, // mcu

  weekly, daily, astro, rtc_only, random, sun_auto, f, // auto

  SIZE // key array size
};



constexpr const_cstringT otok_strings[static_cast<otokBaseT>(otok::SIZE)] = {
//// config keys ///
    "cu", "baud", "rtc", //
    "network", "wlan-ssid", "wlan-password", "ntp-server", "lan-phy", "lan-pwr-gpio", //
    "longitude", "latitude", "timezone", "dst", "tz", "verbose", //
    "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-client-id", //
    "http-enable", "http-user", "http-password", //
    "gm-used", //
    "astro-correction", //
    "rf-tx-pin", "rf-rx-pin", "set-button-pin", "gpio", //
    /// end of config keys ///
    "a", "g", "m", "mm", "c",  //
    "restart", "all", "cuas", "set-pw", "receiver", "transmitter", // config
    "r", "p", "SEP", // cmd
    "boot-count", "print", "kvs-pk", "tm", "am", "stack", "te", "dbp", "cs", "up-time", "version", "ota", // mcu

    "weekly", "daily", "astro", "rtc-only", "random", "sun-auto", "f" // auto

    };

