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

class AppSettings final: public Settings<configAppItem, (int8_t)CBA_size - (int8_t)CB_size, CB_size> {
public:
  using Base = Settings<configAppItem, (int8_t)CBA_size - (int8_t)CB_size, CB_size>;
  using Item = configAppItem;
  using storeFunT = void (*)(configAppItem item, const char *val);
public:
  constexpr AppSettings() {
    initField(CB_RECV, "C_RECEIVER", otok::k_receiver, CBT_i8);
    initField(CB_TRANSM, "C_TRANSM", otok::k_transmitter, CBT_i8);
    initField(CB_RF_TRX, "C_RF_TRX", otok::k_rf_trx, CBT_i8, soCfg_RF_TRX);
    initField(CB_CUID, "C_CUID", otok::k_cu, CBT_u32, soCfg_CU);
    initField(CB_USED_MEMBERS, "C_GMU", otok::k_gm_used, CBT_u32, soCfg_GM_USED, STF_direct_hex);
    initField(CB_BAUD, "C_BAUD", otok::k_baud, CBT_u32, soCfg_BAUD, STF_direct);
    initField(CB_GPIO, "C_GPIO", otok::k_gpio, CBT_str);
    initField(CB_CFG_PASSWD, "C_CFG_PASSWD", otok::NONE, CBT_str, 0, STF_direct);
    initField(CB_LONGITUDE, "C_LONGITUDE", otok::k_longitude, CBT_f, soCfg_LONGITUDE, STF_direct);
    initField(CB_LATITUDE, "C_LATITUDE", otok::k_latitude, CBT_f, soCfg_LATITUDE, STF_direct);
#ifndef CONFIG_APP_USE_POSIX_TIME
#error
#else
    initField(CB_TZ, "C_TZ", otok::k_tz, CBT_str, soCfg_TZ, STF_direct);
#endif
    initField(CB_ASTRO_CORRECTION, "C_AST_COR", otok::k_astro_correction, CBT_i8, soCfg_ASTRO_CORRECTION, STF_direct);
#ifdef CONFIG_APP_USE_NETWORK
    initField(CB_NETWORK_CONNECTION, "C_NW_CONN", otok::k_network, CBT_i8, soCfg_NETWORK);
#endif
    initField(CB_RFOUT_GPIO, "C_RFOUTP", otok::k_rf_tx_pin, CBT_i8, soCfg_GPIO_RFOUT, STF_direct);
    initField(CB_RFIN_GPIO, "C_RFINP", otok::k_rf_rx_pin, CBT_i8, soCfg_GPIO_RFIN, STF_direct);
    initField(CB_SETBUTTON_GPIO, "C_SETBTNP", otok::k_set_button_pin, CBT_i8, soCfg_GPIO_SETBUTTON, STF_direct);
    initField(CB_RFSCK_GPIO, "C_SETRFSCK", otok::k_rf_sclk_pin, CBT_i8, soCfg_GPIO_RFSCK, STF_direct);
    initField(CB_RFMISO_GPIO, "C_SETRFMISO", otok::k_rf_miso_pin, CBT_i8, soCfg_GPIO_RFMISO, STF_direct);
    initField(CB_RFMOSI_GPIO, "C_SETRFMOSI", otok::k_rf_mosi_pin, CBT_i8, soCfg_GPIO_RFMOSI, STF_direct);
    initField(CB_RFSS_GPIO, "C_SETRFSS", otok::k_rf_ss_pin, CBT_i8, soCfg_GPIO_RFSS, STF_direct);
    initField(CB_CC1101_CONFIG, "C_CC1101_CFG", otok::k_cc1101_config, CBT_str, soCfg_CC1101_CONFIG, STF_direct);
    initField(CB_RF_REPEATER, "C_RF_REP", otok::k_rf_repeater, CBT_str, soCfg_RF_REPEATER);
  }

};

static AppSettings app_settings_obj;
SettingsBase<configAppItem> &app_settings = app_settings_obj;

