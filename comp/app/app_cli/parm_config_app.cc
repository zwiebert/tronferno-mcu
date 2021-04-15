#include "parm_config.h"

#include "app_config/proj_app_cfg.h"

#include "cli_config.h"
#include "app_misc/opt_map.hh"
#include <string.h>

#include "fernotron/sep/set_endpos.h"
#include "app_uout/status_output.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "app_misc/rtc.h"
#include "utils_misc/bcd.h"
#include "cli_imp.h"
#include "utils_misc/stof.h"
#include "utils_misc/cstring_utils.h"
#ifdef USE_MQTT
#include "app_mqtt/mqtt.h"
#endif
#ifdef USE_HTTP
#include "net_http_server/http_server_setup.h"
#endif
#ifdef USE_NTP
#include "net/ntp_client_setup.h"
#endif
#include "fernotron_trx/astro.h"
#include <fernotron_trx/fer_trx_c_api.h>
#include <cc1101_ook/trx.hh>

#include "utils_misc/int_types.h"
#include "utils_misc/int_types.h"
#include "gpio/pin.h"
#include <utils_misc/mutex.hh>
#include <utils_misc/int_macros.h>

#include <stdlib.h>
#include <iterator>
#include <algorithm>


bool process_parmConfig_get_app(otok kt, const char *val, const struct TargetDesc &td) {
  switch (kt) {

  case otok::k_all: {

    if (is_val("gpio?")) {
      soCfg_all_gpio(td);
      return true;
    }
  }
    return false;

  case otok::k_cuas:
    soMsg_cuas_state(td, fer_cuas_getState());
    return true;

  default:
    return false;
  }
}

bool process_parmConfig_app(otok kt, const char *key, const char *val, const struct TargetDesc &td, int &errors, u64 &changed_mask) {
  switch (kt) {

  case otok::k_cu: {
    if (is_val("auto")) {
      fer_cuas_set(cli_msgid, 60);
      cli_replySuccess(td);
    } else {
      u32 cu = strtoul(val, NULL, 16);

      if (!(GET_BYTE_2(cu) == FER_ADDR_TYPE_CentralUnit && GET_BYTE_3(cu) == 0)) {
        return cli_replyFailure(td);
      }
      (void) set_optN(u32, cu, CB_CUID);
    }
  }
    break;
  case otok::k_receiver:
    cli_replyResult(td, config_receiver(val));
    break;

  case otok::k_transmitter:
    cli_replyResult(td, config_transmitter(val));
    break;
  case otok::k_rf_trx: {
    NODEFAULT();
    if (auto it = std::find_if(std::begin(cfg_args_rfTrx), std::end(cfg_args_rfTrx), [&val](const char *cs) {
      return strcmp(val, cs) == 0;
    }); it != std::end(cfg_args_rfTrx)) {
      int idx = std::distance(std::begin(cfg_args_rfTrx), it);
      set_optN(i8, idx, CB_RF_TRX);
    }
  }
    break;

#ifdef USE_GPIO_PINS
case otok::k_gpio:
{
  if (is_val("?"))
  {
    soCfg_GPIO_MODES(td);
  }
  else if (*val == '$')
  {
    soCfg_GPIO_MODES_AS_STRING(td);
  }
}
break;
#endif

  case otok::NONE:
#ifdef USE_GPIO_PINS
  if (strncmp(key, "gpio", 4) == 0)
  {
    int gpio_number = atoi(key + 4);

    if (is_val("?"))
    {
      soCfg_GPIO_PIN(td, gpio_number);
    }
    else if (!is_gpio_number_usable(gpio_number, true))
    {
      reply_message(td, "gpio:error", "gpio number cannot be used");
      ++errors;
    }
    else
    {
      const char *error = "unknown gpio config";

      for (int i = 0; pin_mode_args[i]; ++i)
      {
        auto ps = static_cast<mcu_pin_mode>(i);
        if (pin_mode_args[ps] == *val)
        {
          mcu_pin_level pl = val[1] == 'h' ? PIN_HIGH : val[1] == 'l' ? PIN_LOW : val[1] == 'm' ? PIN_HIGH_LOW : PIN_FLOATING;
          error = pin_set_mode(gpio_number, ps, pl);
          config_gpio_setPinMode(gpio_number, ps, pl);
          SET_BIT64(changed_mask, CB_GPIO);
          break;
        }
      }
      if (error)
      {
        ++errors;
        reply_message(td, "gpio:failure", error);
      }
    }
    break;
  }
#endif

  default:
    return false;
  }
  return true;
}


void parmConfig_reconfig_app(uint64_t changed_mask) {
  if (changed_mask & CBM_geo) {
    cli_run_main_loop(mainLoop_configAstro);
  }
  if (changed_mask & CBM_gpio) {
    cli_run_main_loop(mainLoop_configGPIO);
  }
  if (changed_mask & CBM_cc1101) {
    cli_run_main_loop(mainLoop_configCC1101);
  }
}
