#include "app_config/proj_app_cfg.h"

#include "main.h"
#include <cli/cli.h>
#include <app_settings/config.h>
#include "utils_misc/int_types.h"
#include "utils_time/run_time.h"
#include "utils_time/ut_constants.hh"
#include "fernotron/auto/fau_tevent.h"
#include "fernotron/pos/positions_dynamic.h"
#include "fernotron/pos/positions_static.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron_trx/fer_trx_c_api.h"
#include "net/ipnet.h"
#include "gpio/pin.h"

#include "../app_private.h"

typedef void (*lfa_funT)(void);

const lfa_funT lfa_table[lf_Len] = {
#ifdef USE_NETWORK
    ipnet_connected,
    ipnet_disconnected,
#endif
#ifdef USE_NTP
    fer_am_updateTimerEvent,
#endif
#if defined USE_AP_FALLBACK || defined USE_WLAN_AP
    lfa_createWifiAp,
#endif
    cli_loop,
#ifdef FER_TRANSMITTER
    fer_tx_loop,
#endif
#ifdef FER_RECEIVER
    fer_rx_loop,
#endif
#ifdef USE_SEP
    fer_sep_loop,
#endif
    fer_pos_loop, fer_am_loop, fer_am_updateTimerEvent,
#ifdef USE_CUAS
    fer_cuas_set_check_timeout,
#endif
#ifdef USE_PAIRINGS
    fer_alias_auto_set_check_timeout,
#endif
    fer_statPos_loopAutoSave, fer_pos_loopCheckMoving,
    pin_notify_input_change,
    [] { mcu_delayedRestart(1500); },
    config_setup_gpio,
    config_setup_cc1101,
    config_setup_astro,
#ifdef USE_LAN
    config_setup_ethernet,
#endif
#ifdef USE_MQTT
    config_setup_mqttAppClient,
#endif
#ifdef USE_HTTP
    config_setup_httpServer,
#endif
    config_setup_txtio,
};

/**
 * \brief      Call function table entry related to index
 * \param fb   index
 */
void loop_fun_table_call(loop_flagbits fb) {
  if (lfa_table[fb])
    (lfa_table[fb])();
}

/**
 * \brief          Call function table entries according to bitmask
 * \param bitmask  bits set according to \ref loop_flagbits
 */
void loop_fun_table_calls(u32 bitmask) {
   for (int i = 0; bitmask; ++i, (bitmask >>= 1)) {
     if (GET_BIT(bitmask, 0))
       loop_fun_table_call(static_cast<enum loop_flagbits>(i));
   }
}


