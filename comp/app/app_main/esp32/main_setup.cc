#include "app_config/proj_app_cfg.h"
#include "main.h"
#include "app_cli/cli_app.h"
#include "app_settings/config.h"
#include "utils_time/run_time.h"
#include "key_value_store/kvs_wrapper.h"
#include "app_http_server/setup.h"
#include "utils_misc/int_types.h"
#include "app_misc/timer.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "fernotron/auto/fau_tevent.h"
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
#include "../app_private.h"
#include "fernotron/types.h"
#include "main_loop/main_queue.hh"

#ifdef USE_WLAN_AP
void tmr_checkNetwork_start() {
  if (!mainLoop_callFun([]() {
    if (!ipnet_isConnected())
      lfa_createWifiAp();
  }, pdMS_TO_TICKS(1000 * CHECK_NETWORK_INTERVAL))) {
    printf("CheckNetworkTimer start error");
  }
}
#endif

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    ets_printf("ntp synced: %ld\n", time(0));
    mainLoop_callFun(fer_am_updateTimerEvent);
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
#ifdef USE_NTP
    ntpApp_setup();
#endif
#ifdef USE_MQTT
    config_setup_mqttAppClient();
#endif
#ifdef USE_TCPS_TASK
    config_setup_cliTcpServer();
#endif
#ifdef USE_HTTP
  config_setup_httpServer();
#endif

  }
}

void lfPer100ms_mainFun() {
  if (mainLoop_PeriodicFlags.flagbits) {
#ifdef USE_SEP
    if (mainLoop_PeriodicFlags.flags.lf_loopFerSep) {
      fer_sep_loop();
    }
#endif
#ifdef USE_CUAS
   if (mainLoop_PeriodicFlags.flags.lf_checkCuasTimeout) {
     fer_cuas_set_check_timeout();
   }
#endif
  }
}

void mcu_init() {
  mainLoop_setup(32);
  kvs_setup();
  config_setup_txtio();

  config_setup_global();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  cliApp_setup();
#ifdef USE_CLI_TASK
  cli_setup_task(true);
#else
#error
#endif
  fer_am_updateTimerEvent();
  mainLoop_callFun(fer_am_loop, 1000, true);

#ifdef USE_GPIO_PINS
  //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
    struct pin_change_cb {static void IRAM_ATTR cb() {
      mainLoop_callFun_fromISR(pin_notify_input_change);}
    };
    gpio_INPUT_PIN_CHANGED_ISR_cb = pin_change_cb::cb;
#endif

  #ifdef FER_RECEIVER
    //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
    struct msg_received_cb { static void IRAM_ATTR cb() {
      mainLoop_callFun_fromISR(fer_rx_loop);}
    };
    Fer_Trx_API::register_callback_msgReceived_ISR(msg_received_cb::cb);
  #endif

  #ifdef FER_TRANSMITTER
  //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
  struct msg_transmitted_cb { static void IRAM_ATTR cb() {
    mainLoop_callFun_fromISR(fer_tx_loop);}
  };
  Fer_Trx_API::register_callback_msgTransmitted_ISR(msg_transmitted_cb::cb);
  #endif
#ifdef USE_SEP
  fer_sep_enable_disable_cb = [] (bool enable) {
    mainLoop_PeriodicFlags.flags.lf_loopFerSep = enable;
  };
#endif
#ifdef USE_CUAS
  fer_cuas_enable_disable_cb = [] (bool enable, uint32_t cu) {
    mainLoop_PeriodicFlags.flags.lf_checkCuasTimeout = enable;
    config_save_item_n_u32(settings_get_kvsKey(CB_CUID), cu);
    config_item_modified(CB_CUID);
};
#endif

#ifdef USE_NETWORK
  ipnet_CONNECTED_cb = main_setup_ip_dependent;
  enum nwConnection network = config_read_network_connection();

#ifdef USE_AP_FALLBACK
  if (network != nwNone)
#endif
      {
    esp_netif_init();
#ifdef USE_HTTP
    hts_setup_content();
#endif

    switch (network) {
#ifdef USE_WLAN
    case nwWlanSta:
      config_setup_wifiStation();
      break;
#endif
#ifdef USE_WLAN_AP
    case nwWlanAp:
      lfa_createWifiAp(); // XXX: Create the fall-back AP. Should we have a regular configured AP also?
      break;
#endif
#ifdef USE_LAN
    case nwLan:
      config_setup_ethernet();
#endif
      break;
    default:
      break;
    }

  }

#endif // USE_NETWORK

   if (auto smCt = app_safeMode_increment(); smCt > 6) {
     app_safe_mode = true;
   }


  config_setup_gpio();

#ifdef USE_AP_FALLBACK
  //if (network != nwWlanAp) //XXX this crashes TODO FIXME
  if (network == nwLan || network == nwWlanSta)
    tmr_checkNetwork_start();
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
