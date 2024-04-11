#include <app_net/network_connection.hh>
#include <net/comp_glue.hh>
#include <debug/log.h>

#define logtag "main"

nwConnection network_default, network_fallback;

inline void nwc_wifi_sta_setup() {
  config_setup_wifiStation();
}
inline void nwc_wifi_sta_setdown() {
  wifistation_setdown();
}
inline void nwc_wifi_ap_setup() {
  config_setup_wifiAp();
}
inline void nwc_wifi_ap_setdown() {
  wifiAp_setdown();
}
inline void nwc_ethernet_setup() {
  config_setup_ethernet();
}
inline void nwc_ethernet_setdown() {
  ethernet_setdown();
}


static struct {
  nwConnection current_connection;
} state;

#include <esp_netif.h>
static bool nwc_init() {
  static int once;

  if (once || once++)
    return true;

  if (auto res = esp_netif_init(); res != ESP_OK) {
      db_loge(logtag, "%s: failed", __func__);
      return false;
  }
  return true;
}

bool nwc_connect_default(nwConnection nw) {
  if (nw == state.current_connection)
    return true;
  if (!nwc_connect(nw, true))
    return false;
 network_default = nw;
 return true;
}

bool nwc_connect_fallback(nwConnection nw) {
  if (nw == state.current_connection)
    return true;
  if (!nwc_connect(nw, true))
    return false;
 network_fallback = nw;
 return true;
}

bool nwc_connect(nwConnection nw, bool force) {

  if (state.current_connection != nwNone && !force)
    return false;
  if (!nwc_init())
    return false;
  if (!nwc_disconnect())
    return false;

  switch(nw) {
  case nwNone:
    nwc_disconnect();
    break;
#ifdef CONFIG_APP_USE_WLAN_AP
  case nwWlanAp:
    nwc_wifi_ap_setup();
    break;
#endif
#ifdef CONFIG_APP_USE_WLAN
  case nwWlanSta:
    nwc_wifi_sta_setup();
    break;
#endif
#ifdef CONFIG_APP_USE_LAN
  case nwLan:
    nwc_ethernet_setup();
    break;
#endif
  default:
    abort();
    break;
  }

  state.current_connection = nw;

  return true;
}

bool nwc_disconnect() {
  switch(state.current_connection) {
  case nwNone:
    break;
  case nwWlanAp:
    nwc_wifi_ap_setdown();
    break;
  case nwWlanSta:
    nwc_wifi_sta_setdown();
    break;
  case nwLan:
    nwc_ethernet_setdown();
    break;
  default:
    abort();
    break;
  }

  state.current_connection = nwNone;
  return true;
}
