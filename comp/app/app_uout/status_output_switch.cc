#include "app/config/proj_app_cfg.h"
#include "app/config/options.hh"
#include "so_out.h"
#include "so_print.h"
#include "app/settings/config.h"
#include "net/ipnet.h"
#include "net/mqtt/mqtt.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "app/uout/status_output.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#define D(x)

bool so_output_message2(const struct TargetDesc &td, so_msg_t mt, const void *arg) {
  char buf[64];
  switch (mt) {
#ifdef USE_LAN
  case SO_CFG_LAN_PHY:
    td.so().print(gk(mt), cfg_args_lanPhy[config_read_lan_phy()]);
    break;
  case SO_CFG_LAN_PWR_GPIO:
    td.so().print(gk(mt), config_read_lan_pwr_gpio());
    break;
#else
  case SO_CFG_LAN_PHY:
  case SO_CFG_LAN_PWR_GPIO:
    break;
#endif
#ifdef USE_WLAN
  case SO_CFG_WLAN_SSID:
    td.so().print(gk(mt), config_read_wifi_ssid(buf, sizeof buf));
    break;
  case SO_CFG_WLAN_PASSWORD:
    td.so().print(gk(mt), config_read_wifi_passwd(buf, sizeof buf) ? "*" : "");
    break;
#endif
#ifdef USE_NTP
  case SO_CFG_NTP_SERVER:
    td.so().print(gk(mt), config_read_ntp_server(buf, sizeof buf));
    break;
#endif
#ifdef USE_MQTT
  case SO_CFG_MQTT_ENABLE:
    td.so().print(gk(mt), config_read_mqtt_enable());
    break;
  case SO_CFG_MQTT_URL:
    td.so().print(gk(mt), config_read_mqtt_url(buf, sizeof buf));
    break;
  case SO_CFG_MQTT_USER:
    td.so().print(gk(mt), config_read_mqtt_user(buf, sizeof buf));
    break;
  case SO_CFG_MQTT_PASSWORD:
    td.so().print(gk(mt), *config_read_mqtt_passwd(buf, sizeof buf) ? "*" : "");
    break;
  case SO_CFG_MQTT_CLIENT_ID:
    td.so().print(gk(mt), config_read_mqtt_client_id(buf, sizeof buf));
    break;
#else
  case SO_CFG_MQTT_ENABLE:
  case SO_CFG_MQTT_URL:
  case SO_CFG_MQTT_USER:
  case SO_CFG_MQTT_PASSWORD:
  case SO_CFG_MQTT_CLIENT_ID:
    break;
#endif

#ifdef USE_HTTP
  case SO_CFG_HTTP_ENABLE:
    td.so().print(gk(mt), config_read_http_enable());
    break;
  case SO_CFG_HTTP_USER:
    td.so().print(gk(mt), config_read_http_user(buf, sizeof buf));
    break;
  case SO_CFG_HTTP_PASSWORD:
    td.so().print(gk(mt), *config_read_http_passwd(buf, sizeof buf) ? "*" : "");
    break;
#else
  case SO_CFG_HTTP_ENABLE:
  case SO_CFG_HTTP_USER:
  case SO_CFG_HTTP_PASSWORD:
    break;
#endif

  case SO_CFG_VERBOSE:
    td.so().print(gk(mt), config_read_verbose());
    break;

    default:
     return false;
  }
  return true;
}
