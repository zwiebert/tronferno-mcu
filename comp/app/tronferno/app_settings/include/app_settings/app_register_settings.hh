
/**
 * \file app_settings/app_register_settings.hh
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

template<typename settings_type>
constexpr void app_register_settings(settings_type &settings){
    settings.initField(CB_RECV, "C_RECEIVER", otok::k_receiver, CBT_i8);
    settings.initField(CB_TRANSM, "C_TRANSM", otok::k_transmitter, CBT_i8);
    settings.initField(CB_RF_TRX, "C_RF_TRX", otok::k_rf_trx, CBT_i8, soCfg_RF_TRX);
    settings.initField(CB_CUID, "C_CUID", otok::k_cu, CBT_u32, soCfg_CU);
    settings.initField(CB_USED_MEMBERS, "C_GMU", otok::k_gm_used, CBT_u32, soCfg_GM_USED, STF_direct_hex);
    settings.initField(CB_BAUD, "C_BAUD", otok::k_baud, CBT_u32, soCfg_BAUD, STF_direct);
    settings.initField(CB_GPIO, "C_GPIO", otok::k_gpio, CBT_str);
    settings.initField(CB_CFG_PASSWD, "C_CFG_PASSWD", otok::NONE, CBT_str, 0, STF_direct);
    settings.initField(CB_LONGITUDE, "C_LONGITUDE", otok::k_longitude, CBT_f, soCfg_LONGITUDE, STF_direct);
    settings.initField(CB_LATITUDE, "C_LATITUDE", otok::k_latitude, CBT_f, soCfg_LATITUDE, STF_direct);
#ifndef CONFIG_APP_USE_POSIX_TIME
#error
#else
    settings.initField(CB_TZ, "C_TZ", otok::k_tz, CBT_str, soCfg_TZ, STF_direct);
#endif
    settings.initField(CB_ASTRO_CORRECTION, "C_AST_COR", otok::k_astro_correction, CBT_i8, soCfg_ASTRO_CORRECTION, STF_direct);
#ifdef CONFIG_APP_USE_NETWORK
    settings.initField(CB_NETWORK_CONNECTION, "C_NW_CONN", otok::k_network, CBT_i8, soCfg_NETWORK);
#endif
    settings.initField(CB_RFOUT_GPIO, "C_RFOUTP", otok::k_rf_tx_pin, CBT_i8, soCfg_GPIO_RFOUT, STF_direct);
    settings.initField(CB_RFIN_GPIO, "C_RFINP", otok::k_rf_rx_pin, CBT_i8, soCfg_GPIO_RFIN, STF_direct);
    settings.initField(CB_SETBUTTON_GPIO, "C_SETBTNP", otok::k_set_button_pin, CBT_i8, soCfg_GPIO_SETBUTTON, STF_direct);
    settings.initField(CB_RFSCK_GPIO, "C_SETRFSCK", otok::k_rf_sclk_pin, CBT_i8, soCfg_GPIO_RFSCK, STF_direct);
    settings.initField(CB_RFMISO_GPIO, "C_SETRFMISO", otok::k_rf_miso_pin, CBT_i8, soCfg_GPIO_RFMISO, STF_direct);
    settings.initField(CB_RFMOSI_GPIO, "C_SETRFMOSI", otok::k_rf_mosi_pin, CBT_i8, soCfg_GPIO_RFMOSI, STF_direct);
    settings.initField(CB_RFSS_GPIO, "C_SETRFSS", otok::k_rf_ss_pin, CBT_i8, soCfg_GPIO_RFSS, STF_direct);
    settings.initField(CB_CC1101_CONFIG, "C_CC1101_CFG", otok::k_cc1101_config, CBT_str, soCfg_CC1101_CONFIG, STF_direct);
#ifdef CONFIG_APP_USE_REPEATER
    settings.initField(CB_RF_REPEATER, "C_RF_REP", otok::k_rf_repeater, CBT_str, soCfg_RF_REPEATER);
#endif
#ifdef CONFIG_APP_USE_RTC_AUTO_UPD
  settings.initField(CB_RTC_AUTO_UPD, "C_RTC_AUPD", otok::k_rtc_aupd, CBT_i8, soCfg_RTC_AUTO_UPD, STF_direct);
#endif
}
