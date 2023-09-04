
#include "main.hh"
#include <app_cli/cli_app.hh>
#include "app_settings/config.h"
#include "utils_time/run_time.h"
#include "key_value_store/kvs_wrapper.h"
#ifdef CONFIG_APP_USE_HTTP
#include "app_http_server/setup.hh"
#endif
#include "utils_misc/int_types.h"
#include "app_misc/timer.h"
#include "app_misc/rtc.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "fernotron/auto/fau_next_event.hh"
#include "fernotron/alias/pairings.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/positions_static.h"
#include "fernotron/pos/positions_dynamic.h"
#include "fernotron_trx/fer_trx_api.hh"
#include "fernotron/fer_main.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "key_value_store/kvs_wrapper.h"
#include "net/ipnet.h"
#include "storage/storage.h"
#include "txtio/inout.h"

#include <ctime>
#include "config_kvs/config.h"
#include "app_uout/status_output.h"
#include "uout/uo_callbacks.h"
#include "../app_private.hh"
#include "fernotron/fer_pct.h"
#include "main_loop/main_queue.hh"



void tmr_checkNetwork_Sta_start() {
  static void *tmr;
  mainLoop_stopFun(tmr);

  if (!(tmr = mainLoop_callFunByTimer([]() {
    if (!ipnet_isConnected())
      config_setup_wifiStation();
  }, pdMS_TO_TICKS(1000 * 5)))) {
    printf("CheckNetworkTimer_Sta start error");
  }
}

#ifdef CONFIG_APP_USE_WLAN_AP
void tmr_checkNetwork_Ap_start() {
  static void *tmr;
  mainLoop_stopFun(tmr);

  if (!(tmr = mainLoop_callFunByTimer([]() {
    if (!ipnet_isConnected())
      lfa_createWifiAp();
  }, pdMS_TO_TICKS(1000 * CONFIG_NETWORK_CHECK_LOOP_PERIOD_S)))) {
    printf("CheckNetworkTimer_Ap start error");
  }
}
#endif

void tmr_checkNetwork_start(bool fallback_to_wlan_station, bool fallback_to_wlan_ap) {
  if (fallback_to_wlan_station)
    tmr_checkNetwork_Sta_start();

#ifdef CONFIG_APP_USE_WLAN_AP
  if (fallback_to_wlan_ap)
    tmr_checkNetwork_Ap_start();
#endif
}


void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    ets_printf("ntp synced: %llu\n", (long unsigned long) time(NULL));
    mainLoop_callFun([]() {
      next_event_te.fer_am_updateTimerEvent(time(NULL));
      dst_init();
    });
  });
  config_setup_ntpClient();
}



void main_setup_ip_dependent() {
  static int once;
  {
    char buf[20];
    ipnet_addr_as_string(buf, 20);
    uoCb_publish_ipAddress(buf);
  }
  if (!once) {
    once = 1;
#ifdef CONFIG_APP_USE_NTP
    ntpApp_setup();
#endif
#ifdef CONFIG_APP_USE_MQTT
    config_setup_mqttAppClient();
#endif
#ifdef CONFIG_APP_USE_TCPS_TASK
    config_ext_setup_cliTcpServer();
#endif
#ifdef CONFIG_APP_USE_HTTP
  config_setup_httpServer();
#endif

  }
}

void lfPer100ms_mainFun() {
  if (mainLoop_PeriodicFlags.flagbits) {
#ifdef CONFIG_APP_USE_SEP
    if (mainLoop_PeriodicFlags.flags.lf_loopFerSep) {
      fer_sep_loop();
    }
#endif
#ifdef CONFIG_APP_USE_CUAS
   if (mainLoop_PeriodicFlags.flags.lf_checkCuasTimeout) {
     fer_cuas_set_check_timeout();
   }
#endif
  }
}

void mcu_init() {
  mainLoop_setup(32);
  kvs_setup();
  config_ext_setup_txtio();

  config_setup_global();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  cliApp_setup();
#ifdef CONFIG_APP_USE_CLI_TASK
  cli_setup_task(true);
#else
#error "currently unsupported" // FIXME: support CLI without its own task, or remove that APP_USE_CLI_TASK kconfig option
#endif
  next_event_te.fer_am_updateTimerEvent(time(NULL));
  mainLoop_callFunByTimer(fer_am_loop, 1000, true);

#ifdef CONFIG_APP_USE_GPIO_PINS
  //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
    struct pin_change_cb {static void IRAM_ATTR cb() {
      mainLoop_callFun_fromISR(pin_notify_input_change);}
    };
    gpio_INPUT_PIN_CHANGED_ISR_cb = pin_change_cb::cb;
#endif

  #ifdef CONFIG_APP_USE_FER_RECEIVER
    //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
    struct msg_received_cb { static void IRAM_ATTR cb() {
      mainLoop_callFun_fromISR(fer_rx_loop);}
    };
    Fer_Trx_API::register_callback_msgReceived_ISR(msg_received_cb::cb);
  #endif

  #ifdef CONFIG_APP_USE_FER_TRANSMITTER
  //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
  struct msg_transmitted_cb { static void IRAM_ATTR cb() {
    mainLoop_callFun_fromISR(fer_tx_loop);}
  };
  Fer_Trx_API::register_callback_msgTransmitted_ISR(msg_transmitted_cb::cb);
  #endif
#ifdef CONFIG_APP_USE_SEP
  fer_sep_enable_disable_cb = [] (bool enable) {
    mainLoop_PeriodicFlags.flags.lf_loopFerSep = enable;
  };
#endif
#ifdef CONFIG_APP_USE_CUAS
  fer_cuas_enable_disable_cb = [] (bool enable, uint32_t cu) {
    mainLoop_PeriodicFlags.flags.lf_checkCuasTimeout = enable;
    config_save_item_n_u32(settings_get_kvsKey(CB_CUID), cu);
    config_item_modified(CB_CUID);
};
#endif

#ifdef CONFIG_APP_USE_NETWORK
  ipnet_CONNECTED_cb = main_setup_ip_dependent;
  enum nwConnection network = config_read_network_connection();

#ifdef CONFIG_APP_USE_AP_FALLBACK
  if (network != nwNone)
#endif
      {
    esp_netif_init();
#ifdef CONFIG_APP_USE_HTTP
    hts_setup_content();
#endif

    switch (network) {
#ifdef CONFIG_APP_USE_WLAN
    case nwWlanSta:
      config_setup_wifiStation();
      break;
#endif
#ifdef CONFIG_APP_USE_WLAN_AP
    case nwWlanAp:
      lfa_createWifiAp(); // XXX: Create the fall-back AP. Should we have a regular configured AP also?
      break;
#endif
#ifdef CONFIG_APP_USE_LAN
    case nwLan:
    case nwLanOrWlanSta:
      config_setup_ethernet();
#endif
      break;
    default:
      break;
    }

  }

#endif // CONFIG_APP_USE_NETWORK

   if (auto smCt = app_safeMode_increment(); smCt > 6) {
     app_safe_mode = true;
   }


  config_setup_gpio();


  //if (network != nwWlanAp) //XXX this crashes TODO FIXME
  if (network == nwLan || network == nwWlanSta || network == nwLanOrWlanSta)
#ifdef CONFIG_APP_USE_AP_FALLBACK
    tmr_checkNetwork_start(network == nwLanOrWlanSta, true);
#else
  tmr_checkNetwork_start(network == nwLanOrWlanSta, false);
#endif
  app_timerISR_setup();
  stor_setup();
  main_setup();

  if (kvshT h = kvs_open("misc", kvs_READ_WRITE)) {
    boot_counter = kvs_get_i32(h, "boot_ct", 0, 0) + 1;
    kvs_set_i32(h, "boot_ct", boot_counter);
    kvs_commit(h);
    kvs_close(h);
  }
}
