#include "parm_config.hh"


#include "cli_internal.hh"

#include <utils_misc/mutex.hh>
#include <utils_misc/int_macros.h>
#include <utils_misc/cstring_utils.h>
#include <app_uout/status_output.h>
#include <app_misc/opt_map.hh>
#include "app_settings/all_settings.hh"

int process_parmConfig(clpar p[], int len, class UoutWriter &td) {
  // static RecMutex settings_mutex;
  // std::lock_guard<RecMutex> lock(settings_mutex);

  int arg_idx;
  int errors = 0;
  u64 changed_mask = 0;
  u64 processed_keys_mask = 0;
  {
    so_object<void> cfgObj(soCfg_begin, soCfg_end, td);

    for (int pass = 0; pass < 2; ++pass) {
      for (arg_idx = 1; arg_idx < len; ++arg_idx) {
        const char *key = p[arg_idx].key, *val = p[arg_idx].val;

        if (key == NULL || val == NULL) { // don't allow any default values in config
          return cli_replyFailure(td);
        }

        if (GET_BIT64(processed_keys_mask, arg_idx))
          continue;

        bool endsWithQuestionMark = csu_endsWith(val, "?");

        if (pass == 0 && endsWithQuestionMark)
          continue;
        if (pass == 1 && !endsWithQuestionMark)
          continue;

        otok kt = optMap_findToken(key);

        if (kt != otok::NONE) {
          if (endsWithQuestionMark) {
            if (process_parmConfig_get_app(kt, val, td))
              continue;
            if (process_parmConfig_get_comp(kt, val, td))
              continue;
          } else {
            if (auto sd = all_settings.get_SettingsData(kt); sd && sd->store_fun != STF_none) {
              SET_BIT64(changed_mask, sd->id_bit);
              if (int res = process_parmConfig_assign(sd->kvs_type, sd->kvs_key, sd->store_fun, val)) {
                if (res < 0)
                  ++errors;
                continue;
              }

            }
          }
        }

        SET_BIT64(processed_keys_mask, arg_idx);

        if (process_parmConfig_comp(kt, key, val, td, errors, changed_mask))
          continue;
        else if (process_parmConfig_app(kt, key, val, td, errors, changed_mask))
          continue;
        else {
          ++errors;
          cli_warning_optionUnknown(td, key);
        }
      }

      if (pass == 0 && changed_mask) {
        parmConfig_reconfig_comp(changed_mask);
        parmConfig_reconfig_app(changed_mask);
      }
    }
  }
  cli_replyResult(td, errors == 0);
  return 0;
}

