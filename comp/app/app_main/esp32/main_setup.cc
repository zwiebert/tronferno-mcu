#include "app/config/proj_app_cfg.h"
#include "main.h"
#include "app/cli/cli_app.h"
#include "misc/time/run_time.h"
#include "key_value_store/kvs_wrapper.h"
#include "net/http/server/content/setup.h"
#include "app/common.h"
#include "app/timer.h"
#include "app/settings/config.h"
#include "fernotron/auto/fau_tevent.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/positions_static.h"
#include "fernotron/pos/positions_dynamic.h"
#include "fernotron/fer_msg_tx.h"
#include "fernotron/fer_radio_trx.h"
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
#include "app/uout/status_output.h"
#include "../app_private.h"
#include "fernotron/types.h"

#ifdef FER_TRANSMITTER

void tmr_setBit_txLoop_start(u32 interval_ms) {
  const int interval = pdMS_TO_TICKS(interval_ms);
  TimerHandle_t tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, nullptr, [](TimerHandle_t xTimer) {
    lf_setBit(lf_loopFerTx);
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("CheckNetworkTimer start error");
  }
}

void loop_setBit_txLoop(u32 time_ts) {
  u32 now_ts = get_now_time_ts();
  if (now_ts >= time_ts) {
    lf_setBit(lf_loopFerTx);
    return;
  }
  u32 delay_ms = (time_ts - now_ts) * 100;
  tmr_setBit_txLoop_start(delay_ms);
}
#endif


void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    ets_printf("ntp synced: %ld\n", time(0));
    lf_setBit(lf_ntpSyncTime);
  });
  config_setup_ntpClient();
}

extern "C" void main_setup_ip_dependent() { //XXX called from library
  static int once;
  soMsg_inet_print_address();
  if (!once) {
    once = 1;
#ifdef USE_NTP
    ntpApp_setup();
#endif
#ifdef USE_MQTT
    config_setup_mqttAppClient();
#endif
#if defined USE_TCPS || defined USE_TCPS_TASK
    config_setup_cliTcpServer();
#endif
#ifdef USE_HTTP
  config_setup_httpServer();
#endif

  fpos_POSITIONS_MOVE_cb = [](bool enable) {
    lfPer_putBit(lf_loopPosCheckMoving, enable);
  };
  fpos_POSITIONS_SAVE_cb  = [](bool enable) {
    lfPer_putBit(lf_loopPosAutoSave, enable);
  };
  ftx_READY_TO_TRANSMIT_cb = loop_setBit_txLoop;
  fau_TIMER_DATA_CHANGE_cb = [] {
      lf_setBit(lf_loopFauTimerDataHasChanged);
    };
#ifdef ACCESS_GPIO
  gpio_INPUT_PIN_CHANGED_ISR_cb = [] () IRAM_ATTR {
      lf_setBit_ISR(lf_gpio_input_intr, true);
    };
#endif
  mcu_restart_cb = mcu_restart;

  #ifdef FER_RECEIVER
  frx_MSG_RECEIVED_ISR_cb = [] () IRAM_ATTR {
      lf_setBit_ISR(lf_loopFerRx, true);
    };
  #endif

  #ifdef FER_TRANSMITTER
  ftx_MSG_TRANSMITTED_ISR_cb = [] () IRAM_ATTR {
    lf_setBit_ISR(lf_loopFerTx, true);
  };
  #endif
#ifdef USE_SEP
  sep_enable_disable_cb = [] (bool enable) {
    lfPer_putBit(lf_loopFerSep, enable);
  };
#endif
#ifdef USE_PAIRINGS
  pair_enable_disable_cb = [] (bool enable) {
    lfPer_putBit(lf_checkPairingTimeout, enable);
  };
#endif
#ifdef USE_CUAS
  cuas_enable_disable_cb = [] (bool enable) {
    lfPer_putBit(lf_checkCuasTimeout, enable);
};
#endif
  }
}



void mcu_init() {
#ifdef USE_EG
  loop_eventBits_setup();
#endif
  kvs_setup();
  config_setup_txtio();

  config_setup_global();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());
#ifdef USE_CLI_TASK
  cli_setup_task(true);
#else
  lfPer_setBit(lf_loopCli);
#endif
#ifdef USE_TCPS
  lfPer_setBit(lf_loopTcpServer);
#endif
#ifdef USE_TCPS_TASK
#endif
  fam_updateTimerEvent();
  lfPer_setBit(lf_loopFerTimerState);


#ifdef USE_NETWORK
  enum nwConnection network = config_read_network_connection();
#ifdef USE_AP_FALLBACK
  esp_netif_init();
#else
  if (network != nwNone)
    esp_netif_init();
#endif

  ipnet_gotIpAddr_cb = [] {
    lf_setBit(lf_gotIpAddr);
  };
  ipnet_lostIpAddr_cb = [] {
    lf_setBit(lf_lostIpAddr);
  };

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
#ifdef USE_HTTP
    hts_setup_content();
#endif
#endif

  config_setup_gpio();

#ifdef USE_AP_FALLBACK
  if (network != nwWlanAp)
    tmr_checkNetwork_start();
#endif
  intTimer_setup();
  stor_setup();
  main_setup();

  kvshT h;
  if ((h = kvs_open("misc", kvs_READ_WRITE))) {
    boot_counter = kvs_get_i32(h, "boot_ct", 0, 0) + 1;
    kvs_set_i32(h, "boot_ct", boot_counter);
    kvs_close(h);
  }
}
