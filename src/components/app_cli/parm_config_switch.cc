#include "app_config/proj_app_cfg.h"
#include "config_kvs/config.h"
#include "cli_imp.h"
#include "app/cli/cli_config.h"
#include "cli/cli.h"
#include "net/mqtt/mqtt.h"
#include "net/http/server/http_server.h"
#include "app/uout/status_output.h"
#include "misc/int_macros.h"
#include "misc/int_types.h"
#include <string.h>

#define isValid_optStr(cfg, new) true
#define set_optStr(v, cb) if (config_save_item_s(cb, v)) has_changed(cb)
#define set_optBlob(v, cb) if (config_save_item_b(cb, &v, sizeof v)) has_changed(cb)
#define set_opt(t, v, cb) if (config_save_item_##t(cb,v)) has_changed(cb)
#define set_optN(t, v, cb) if (config_save_item_n_##t(cb,v)) has_changed(cb)

#define has_changed(cb) SET_BIT(*changed_mask, cb)



bool process_parmKvsConfig(so_msg_t so_key, const char *val, u32 *changed_mask) {
  bool result = true;
  bool flag_isValid = true;

  switch (so_key) {
  case SO_CFG_VERBOSE: {
    NODEFAULT();
    set_opt(i8, val, CB_VERBOSE);
  }
    break;
#ifdef USE_WLAN
  case SO_CFG_WLAN_SSID: {
    set_optStr(val, CB_WIFI_SSID);

    if (!flag_isValid)
      cli_replyFailure();
  }
    break;

  case SO_CFG_WLAN_PASSWORD: {
    set_optStr(val, CB_WIFI_PASSWD);

    if (!flag_isValid)
      cli_replyFailure();
  }
    break;
#endif // USE_WLAN
#ifdef USE_LAN
  case SO_CFG_LAN_PHY: {
    NODEFAULT();
    bool success = false;
    u8 i;
    for (i = 0; i < lanPhyLEN; ++i) {
      if (strcasecmp(val, cfg_args_lanPhy[i]) == 0) {
        set_optN(i8, i, CB_LAN_PHY);
        success = true;
        break;
      }
    }
  }
    break;
  case SO_CFG_LAN_PWR_GPIO: {
    NODEFAULT();
    set_opt(i8, val, CB_LAN_PWR_GPIO);

  }
    break;
#endif // USE_LAN

#ifdef USE_NTP
  case SO_CFG_NTP_SERVER: {
    set_optStr(val, CB_NTP_SERVER);
  }
    break;
#endif

#ifdef USE_MQTT
  case SO_CFG_MQTT_ENABLE: {
    set_optN(i8, (*val == '1'), CB_MQTT_ENABLE);
  }
    break;

  case SO_CFG_MQTT_PASSWORD: {
    set_optStr(val, CB_MQTT_PASSWD);
  }
    break;

  case SO_CFG_MQTT_USER:

    set_optStr(val, CB_MQTT_USER);

    break;

  case SO_CFG_MQTT_URL:

    set_optStr(val, CB_MQTT_URL);

    break;

  case SO_CFG_MQTT_CLIENT_ID: {
    set_optStr(val, CB_MQTT_CLIENT_ID);
  }
    break;
#endif //USE_MQTT

#ifdef USE_HTTP
  case SO_CFG_HTTP_ENABLE: {
    set_optN(i8, (*val == '1'), CB_HTTP_ENABLE);

  }
    break;

  case SO_CFG_HTTP_PASSWORD:

    set_optStr(val, CB_HTTP_PASSWD);

    break;

  case SO_CFG_HTTP_USER:

    set_optStr(val, CB_HTTP_USER);

    break;
#endif //USE_HTTP

  default:
    result = false;
  }

  return result;
}
