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
#include "fernotron_trx/raw/fer_radio_trx.h"
#include "fernotron_trx/fer_trx_api.hh"
#include "fernotron/fer_main.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "key_value_store/kvs_wrapper.h"
#include "net/ipnet.h"
#include "storage/storage.h"
#include "txtio/inout.h"
#include <esp_attr.h>
#include <esp_err.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp32/rom/ets_sys.h>
#include <freertos/projdefs.h>
#include <freertos/timers.h>
#include <ctime>
#include "config_kvs/config.h"
#include "app_uout/status_output.h"
#include "uout/uo_callbacks.h"
#include "../app_private.h"
#include "fernotron/types.h"
#include "app_main/loop.h"
#include "main_loop/main_queue.hh"

#ifdef FER_TRANSMITTER

void tmr_setBit_txLoop_start(u32 interval_ms) {
  const int interval = pdMS_TO_TICKS(interval_ms);
  TimerHandle_t tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, nullptr, [](TimerHandle_t xTimer) {
    mainLoop_callFun(fer_tx_loop);
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("CheckNetworkTimer start error");
  }
}

void loop_setBit_txLoop(u32 when_to_transmit_ts) {
  u32 now_ts = get_now_time_ts();
  if (now_ts >= when_to_transmit_ts) {
    mainLoop_callFun(fer_tx_loop);
    return;
  }

  // message sent time lies in future

  u32 delay_ms = (when_to_transmit_ts - now_ts) * 100;
  tmr_setBit_txLoop_start(delay_ms);
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

void mcu_init() {
  loop_eventBits_setup();
  kvs_setup();
  config_setup_txtio();

  config_setup_global();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  cliApp_setup();
#ifdef USE_CLI_TASK
  cli_setup_task(true);
#else
  lfPer100ms_setBit(lf_loopCli);
#endif
  fer_am_updateTimerEvent();
  lfPer100ms_setBit(lf_loopFerTimerState);


  fer_pos_POSITIONS_MOVE_cb = [](bool enable) {
    lfPer100ms_putBit(lf_loopPosCheckMoving, enable);
  };
  fer_pos_POSITIONS_SAVE_cb  = [](bool enable) {
    lfPer100ms_putBit(lf_loopPosAutoSave, enable);
  };
  fer_tx_READY_TO_TRANSMIT_cb = loop_setBit_txLoop;
  fer_au_TIMER_DATA_CHANGE_cb = [] {
      lf_setBit(lf_loopFauTimerDataHasChanged);
    };
#ifdef USE_GPIO_PINS
  //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
    struct pin_change_cb {static void IRAM_ATTR cb() {
      lf_setBit_ISR(lf_gpio_input_intr, true);}
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
    lfPer100ms_putBit(lf_loopFerSep, enable);
  };
#endif
#ifdef USE_PAIRINGS
  fer_alias_enable_disable_cb = [] (bool enable) {
    lfPer100ms_putBit(lf_checkPairingTimeout, enable);
  };
#endif
#ifdef USE_CUAS
  fer_cuas_enable_disable_cb = [] (bool enable, uint32_t cu) {
    lfPer100ms_putBit(lf_checkCuasTimeout, enable);
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
