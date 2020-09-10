#pragma once
#include <stdint.h>

typedef int8_t otokBaseT;

enum class otok : otokBaseT  {
  NONE = -1,  // not existing

      //// config keys ///
      cu, baud, rtc,
      network, wlan_ssid, wlan_password, ntp_server, lan_phy, lan_pwr_gpio,
      longitude, latitude, timezone, dst, tz, verbose,
      mqtt_enable, mqtt_url, mqtt_user, mqtt_password, mqtt_client_id,
      http_enable, http_user, http_password,
      gm_used,
      astro_correction,
      rf_tx_pin, rf_rx_pin, set_button_pin, gpio,
      /// end of config keys ///

  a, g, m, mm, c, //
  restart, all, cuas, set_pw, receiver, transmitter, //




  SIZE // key array size
};

typedef const char *const_cstringT;

constexpr const_cstringT otok_strings[static_cast<otokBaseT>(otok::SIZE)] = {
  //// config keys ///
  "cu", "baud", "rtc",
  "network", "wlan-ssid", "wlan-password", "ntp-server", "lan-phy", "lan-pwr-gpio",
  "longitude", "latitude", "timezone", "dst", "tz", "verbose",
  "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-client-id",
  "http-enable", "http-user", "http-password",
  "gm-used",
  "astro-correction",
  "rf-tx-pin", "rf-rx-pin", "set-button-pin", "gpio",
  /// end of config keys ///
  "a", "g", "m", "mm", "c",//
  "restart", "all", "cuas", "set-pw", "receiver", "transmitter", //
};

otok optMap_findToken(const char *key);
