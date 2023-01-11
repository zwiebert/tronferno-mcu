#include "parm_config.h"


#include <app_settings/config.h>
#include <app_settings/app_settings.hh>
#include <app_misc/rtc.h>
#include "cli_imp.h"
#include <app_uout/so_config.h>
#include <config_kvs/comp_settings.hh>
#include <config_kvs/config.h>
#include <uout/cli_out.h>

#ifdef CONFIG_APP_USE_MQTT
#include <app_mqtt/mqtt.h>
#endif
#ifdef CONFIG_APP_USE_HTTP
#include <net_http_server/http_server_setup.h>
#endif
#ifdef CONFIG_APP_USE_NTP
#include <net/ntp_client_setup.hh>
#endif

#include "main_loop/main_queue.hh"

#include <utils_misc/int_types.h>
#include <utils_misc/int_types.h>
#include <utils_misc/mutex.hh>
#include <utils_misc/int_macros.h>

#include <stdlib.h>
#include <iterator>
#include <algorithm>

bool process_parmConfig_get_comp(otok kt, const char *val, const struct TargetDesc &td) {
  switch (kt) {

  case otok::k_all: {
    if (is_val("?")) {
      soCfg_all(td);
      return true;
    } else if (is_val("net?")) {
      soCfg_all_net(td);
      return true;
    } else if (strlen(val) == 5 && val[0] == 'p' && val[2] == 's' && val[4] == '?') {
      const int part_num = val[1] - '0';
      const int part_size = val[3] - '0';
      if (IS_IN_RANGE(0, part_num, 9) && IS_IN_RANGE(1, part_size, 9)) {
        soCfg_all_part(td, part_num, part_size);
        return true;
      }
    }
  }
    return false;

  case otok::k_rtc:
    soCfg_RTC(td);
    return true;

  default: {
    if (auto fun = settings_get_soCfgFun(kt)) {
      fun(td);
      return true;
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
  } else if (storeFun == STF_direct_bool) {
    switch (kvsType) {
    case CBT_i8:
      if (!config_save_item_n_i8(kvsKey, *val == '1'))
        return -1;
      return 1;
    default:
      break;
    }
  }
  return 0;
}

SettData get_settingsData(otok kt, u64 &changed_mask) {
  SettData settData { };
  if (auto item = comp_settings.get_item(kt); item != CBC_NONE) {
    settData = settings_getData(comp_settings, item);
    SET_BIT64(changed_mask, item);
  } else if (auto appItem = app_settings.get_item(kt); appItem != CBA_NONE) {
    settData = settings_getData(app_settings, appItem);
    SET_BIT64(changed_mask, appItem);
  }
  return settData;
}

bool process_parmConfig_comp(otok kt, const char *key, const char *val, const struct TargetDesc &td, int &errors, u64 &changed_mask) {
  switch (kt) {
#if ENABLE_RESTART
  case otok::k_restart:
    mainLoop_callFun([]() { mainLoop_mcuRestart(1500); });
    break;
#endif

    ////////////////////////////////////////////////////////////
#ifdef CONFIG_APP_USE_LAN
  case otok::k_lan_phy: {
    NODEFAULT();
    if (auto it = std::find_if(std::begin(cfg_args_lanPhy), std::end(cfg_args_lanPhy), [&val](const char *cs) {
      return strcmp(val, cs) == 0;
    }); it != std::end(cfg_args_lanPhy)) {
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

    ////////////////////////////////////////////////////////////////

  case otok::k_rtc: {
    cli_replyResult(td, val ? rtc_set_by_string(val) : false);
  }
    break;

#ifdef CONFIG_APP_USE_NETWORK
  case otok::k_network: {
    int i;
    NODEFAULT();
    bool success = false;
    for (i = 0; i < nwLEN; ++i) {
#ifndef CONFIG_APP_USE_LAN
    if (i == nwLan)
    {
      continue;
    }
#endif
#ifndef CONFIG_APP_USE_WLAN
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

void parmConfig_reconfig_comp(u64 changed_mask) {
  if (changed_mask & BIT(CB_TZ)) {
    rtc_setup();
  }

#ifdef CONFIG_APP_USE_LAN
  if (changed_mask & CMB_lan) {
    mainLoop_callFun(config_setup_ethernet);
  }
#endif
#ifdef CONFIG_APP_USE_MQTT
  if (changed_mask & CBM_mqttClient) {
    mainLoop_callFun(config_setup_mqttAppClient);
  }
#endif
#ifdef CONFIG_APP_USE_HTTP
  if (changed_mask & CBM_httpServer) {
    mainLoop_callFun(config_setup_httpServer);
  }
#endif
  if (changed_mask & CBM_txtio) {
    mainLoop_callFun(config_ext_setup_txtio);
  }

}
