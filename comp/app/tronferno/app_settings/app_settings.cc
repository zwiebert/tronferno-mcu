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

#include <config_kvs/register_settings.hh>
#include <config_kvs/comp_settings.hh>
class CompSettings final : public Settings<configItem, CBA_size> {
public:
  using Base = Settings<configItem, CB_size>;
public:
  constexpr CompSettings() {
    register_settings(*this);
    register_app_settings(*this);
  }
};
static constexpr CompSettings comp_settings_obj;
const SettingsBase<configItem> &comp_sett = comp_settings_obj;

