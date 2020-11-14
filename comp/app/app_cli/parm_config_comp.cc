#include "parm_config.h"
#include <app_config/proj_app_cfg.h>

#include <app_settings/config.h>
#include <app_settings/app_settings.hh>
#include <app_misc/rtc.h>
#include "cli_imp.h"
#include <app_uout/so_config.h>
#include <config_kvs/comp_settings.hh>
#include <config_kvs/config.h>
#include <uout/cli_out.h>

#ifdef USE_MQTT
#include <app_mqtt/mqtt.h>
#endif
#ifdef USE_HTTP
#include <net_http_server/http_server_setup.h>
#endif
#ifdef USE_NTP
#include <net/ntp_client_setup.h>
#endif

#include <utils_misc/int_types.h>
#include <utils_misc/int_types.h>
#include <utils_misc/mutex.hh>
#include <utils_misc/int_macros.h>

#include <stdlib.h>
#include <iterator>
#include <algorithm>


void (*mcu_restart_cb)();

so_msg_t so_soMsg_from_otok(otok kt) {
  so_msg_t result = static_cast<so_msg_t>(SO_CFG_begin + 1 + static_cast<otokBaseT>(kt));
  if (result >= SO_CFG_last || result == SO_CFG_GPIO_MODES)
    return SO_NONE;
  return result;
}

bool process_parmConfig_get_comp(otok kt, const char *val, const struct TargetDesc &td) {
  switch (kt) {

  case otok::k_all: {
    if (is_val("?")) {
      soCfg_all(td);
      return true;
    } else if (is_val("net?")) {
      soCfg_all_net(td);
      return true;
    }
  }
    return false;

  case otok::k_rtc:
    soCfg_RTC(td);
    return true;

  default: {
    if (auto so_key = so_soMsg_from_otok(kt); so_key != SO_NONE) {
      if (auto fun = settings_get_soCfgFun(kt)) {
        fun(td);
        return true;
      }
    }
    return false;
  }
  }
}

int process_parmConfig_assign(KvsType kvsType, const char *kvsKey, StoreFun storeFun, const char *val) {
  if (storeFun == STF_direct) {
    switch (kvsType) {
    case CBT_str:
      if (!config_save_item_s(kvsKey, val))
        return -1;
      return 1;

    case CBT_i8:
      if (!config_save_item_i8(kvsKey, val))
        return -1;
      return 1;

    case CBT_u32:
      if (!config_save_item_u32(kvsKey, val))
        return -1;
      return 1;

    case CBT_f:
      if (!config_save_item_f(kvsKey, val))
        return -1;
      return 1;

    default:
      return false;
    }
  } else if (storeFun == STF_direct_hex) {
    switch (kvsType) {
    case CBT_u32:
      if (!config_save_item_u32(kvsKey, val, 16))
        return -1;
      return 1;
    default:
      break;
    }
  }
  return 0;
}

SettData get_settingsData(otok kt, u32 &changed_mask) {
  SettData settData { };
  if (auto item = comp_settings.get_item(kt); item != CBC_NONE) {
    settData = settings_getData(comp_settings, item);
    SET_BIT(changed_mask, item);
  } else if (auto appItem = app_settings.get_item(kt); appItem != CBA_NONE) {
    settData = settings_getData(app_settings, appItem);
    SET_BIT(changed_mask, appItem);
  }
  return settData;
}

bool process_parmConfig_comp(otok kt, const char *key, const char *val, const struct TargetDesc &td, int &errors, u32 &changed_mask) {
  switch (kt) {
#if ENABLE_RESTART
  case otok::k_restart:
    if (mcu_restart_cb)
      mcu_restart_cb();
    break;
#endif

    ////////////////////////////////////////////////////////////
#ifdef USE_LAN
  case otok::k_lan_phy: {
    NODEFAULT();
    if (auto it = std::find(std::begin(cfg_args_lanPhy), std::end(cfg_args_lanPhy), val); it != std::end(cfg_args_lanPhy)) {
      int idx = std::distance(std::begin(cfg_args_lanPhy), it);
      set_optN(i8, idx, CB_LAN_PHY);
    }
  }
    break;
  case otok::k_lan_pwr_gpio: {
    NODEFAULT();
    set_opt(i8, val, CB_LAN_PWR_GPIO);
  }
    break;
#endif // USE_LAN

#ifdef USE_MQTT
  case otok::k_mqtt_enable:
    set_optN(i8, (*val == '1'), CB_MQTT_ENABLE);
    break;
#endif //USE_MQTT
#ifdef USE_HTTP
  case otok::k_http_enable:
    set_optN(i8, (*val == '1'), CB_HTTP_ENABLE);
    break;
#endif
    ////////////////////////////////////////////////////////////////

  case otok::k_rtc: {
    cli_replyResult(td, val ? rtc_set_by_string(val) : false);
  }
    break;

#ifdef USE_NETWORK
  case otok::k_network: {
    int i;
    NODEFAULT();
    bool success = false;
    for (i = 0; i < nwLEN; ++i) {
#ifndef USE_LAN
    if (i == nwLan)
    {
      continue;
    }
#endif
#ifndef USE_WLAN
    if (i == nwWlanSta || i == nwWlanAp)
    {
      continue;
    }
#endif
      if (is_val(cfg_args_network[i])) {
        (void) (set_optN(i8, i, CB_NETWORK_CONNECTION));
        success = true;
        break;
      }
    }
    if (!success)
      cli_replyFailure(td);
  }
    break;
#endif

  case otok::NONE:
  default:
    return false;
  }
  return true;
}

void parmConfig_reconfig_comp(uint32_t changed_mask) {
  if (changed_mask & BIT(CB_TZ)) {
    rtc_setup();
  }

#ifdef USE_LAN
  if (changed_mask & CMB_lan) {
    config_setup_ethernet();
  }
#endif
#ifdef USE_MQTT
  if (changed_mask & CBM_mqttClient) {
    config_setup_mqttAppClient();
  }
#endif
#ifdef USE_HTTP
  if (changed_mask & CBM_httpServer) {
    config_setup_httpServer();
  }
#endif
  if (changed_mask & CBM_txtio) {
    config_setup_txtio();
  }
}
