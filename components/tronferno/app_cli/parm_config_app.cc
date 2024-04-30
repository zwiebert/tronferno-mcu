#include "parm_config.hh"
#include "cli_internal.hh"

#include "app_config/options.hh"
#include "app_uout/status_output.h"
#include "app_misc/opt_map.hh"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "app_misc/rtc.h"
#include "app_uout/status_output.h"
#ifdef CONFIG_APP_USE_MQTT
#include "app_mqtt/mqtt.h"
#endif

#include <fernotron/fer_main.h>
#include "fernotron/sep/set_endpos.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "fernotron_trx/astro.h"
#include <fernotron_trx/fer_trx_c_api.h>
#include <cc1101_ook/trx.hh>
#include "gpio/pin.h"

#include "main_loop/main_queue.hh"


#ifdef CONFIG_APP_USE_HTTP
#include "net_http_server/http_server_setup.h"
#endif
#ifdef CONFIG_APP_USE_NTP
#include "net/ntp_client_setup.hh"
#endif

#include <utils_misc/mutex.hh>

#include "utils_misc/bcd.h"
#include "utils_misc/stof.h"
#include "utils_misc/cstring_utils.h"
#include "stdint.h"
#include <utils_misc/int_macros.h>

#include <stdlib.h>
#include <string.h>
#include <iterator>
#include <algorithm>

static bool id_isValid(const char *s) {
  if (strlen(s) != 6)
    return false;
  if (!(s[0] == '1' || s[0] == '2'))
    return false;

  for (int i = 1; i < 6; ++i)
    if (!isxdigit(s[i]))
      return false;

  return true;
}

static bool ids_areValid(const char *ids) {
  const int len = strlen(ids);

  if (len % 6 != 0)
    return false;
  for (int i = 0; i < len; i += 6) {
    const char *s = ids + i;

    if (!(s[0] == '1' || s[0] == '2'))
      return false;

    for (int i = 1; i < 6; ++i)
      if (!isxdigit(s[i]))
        return false;
  }

  return true;
}

bool process_parmConfig_get_app(otok kt, const char *val, class UoutWriter &td) {
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

bool process_parmConfig_app(otok kt, const char *key, const char *val, class UoutWriter &td, int &errors, uint64_t &changed_mask) {
  switch (kt) {

  case otok::k_cu: {
    if (is_val("auto")) {
      fer_cuas_set(60);
      cli_replySuccess(td);
    } else {
      uint32_t cu = strtoul(val, NULL, 16);

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
      SET_BIT64(changed_mask, CB_RF_TRX);
    }
  }
    break;

#ifdef CONFIG_APP_USE_REPEATER
  case otok::k_rf_repeater: {
    NODEFAULT();

    if (val[0] == '+') {
      if (id_isValid(val + 1)) {

        char buf[80];
        if (config_read_item(CB_RF_REPEATER, buf, sizeof buf, "")) {
          if (!ids_areValid(buf)) {
            *buf = '\0';
          }

          strncat(buf, val + 1, sizeof buf - 1 - strlen(val));
          set_optStr(buf, CB_RF_REPEATER);
          soCfg_RF_REPEATER(td);
          SET_BIT64(changed_mask, CB_RF_REPEATER);
        } else {
          ++errors;
        }
      }
    } else if (val[0] == '-') {
      if (id_isValid(val + 1)) {

        char buf[80];
        if (config_read_item(CB_RF_REPEATER, buf, sizeof buf, "")) {
          for (int i = 0, len = strlen(buf); i < len; i += 6) {
            if (0 == strncasecmp(val + 1, buf + i, 6)) {
              memmove(buf + i, buf + i + 6, strlen(buf + i + 6) + 1);
              set_optStr(buf, CB_RF_REPEATER);
            }
          }
        }
        SET_BIT64(changed_mask, CB_RF_REPEATER);
        soCfg_RF_REPEATER(td);
      } else {
        ++errors;
      }
    } else {
      if (ids_areValid(val)) {
        set_optStr(val, CB_RF_REPEATER);
        SET_BIT64(changed_mask, CB_RF_REPEATER);
        soCfg_RF_REPEATER(td);
      } else {
        ++errors;
      }
    }
  }
    break;
#endif

#ifdef CONFIG_APP_USE_GPIO_PINS
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
#ifdef CONFIG_APP_USE_GPIO_PINS
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

  [[fallthrough]];
  default:
    return false;
  }
  return true;
}

void parmConfig_reconfig_app(uint64_t changed_mask) {

  if (GET_BIT64(changed_mask, CB_USED_MEMBERS)) {
    mainLoop_callFun([]() {
      const uint32_t usedMembers = config_read_item(CB_USED_MEMBERS, (unsigned long) CONFIG_APP_FER_GM_USE);
      fer_usedMemberMask.fromNibbleCounters(usedMembers);
    });
  }

  if (changed_mask & CBM_geo) {
    mainLoop_callFun(config_setup_astro);
  }
  if ((changed_mask & CBM_gpio) || (changed_mask & CBM_cc1101)) {
    mainLoop_callFun(config_setup_gpio);
    mainLoop_callFun(config_setup_cc1101);
  }
#ifdef CONFIG_APP_USE_REPEATER
  if (changed_mask & CBM_rf_repeater) {
    mainLoop_callFun (config_setup_repeater);
  }
#endif
}
