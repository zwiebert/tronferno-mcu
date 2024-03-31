/**
 * \file app_settings/app_settings.hh
 * \brief Settings for application components
 */
#include "app_settings/app_settings.hh"
#include <app_config/options.hh>
#include <app_uout/so_config.h>
#include <utils_misc/int_macros.h>
#include "config_kvs/config.h"
#include "config_kvs/comp_settings.hh"
#include <config_kvs/settings.hh>
#include <assert.h>
#include "app_settings/app_register_settings.hh"

class AppSettings final: public Settings<configAppItem, (int8_t)CBA_size - (int8_t)CB_size, CB_size> {
public:
  using Base = Settings<configAppItem, (int8_t)CBA_size - (int8_t)CB_size, CB_size>;
  using Item = configAppItem;
  using storeFunT = void (*)(configAppItem item, const char *val);
public:
   constexpr AppSettings() {
     app_register_settings(*this);
  }
};

#define DEFINE_COMP_SETTINGS_OBJECT
#include <config_kvs/register_settings.hh>

static constexpr AppSettings app_settings_obj;
const SettingsBase<configAppItem> &app_sett = app_settings_obj;
