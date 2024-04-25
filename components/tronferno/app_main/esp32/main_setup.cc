#include "main.hh"
#include "../app_private.hh"
#include "main_loop_periodic.h"

#ifdef CONFIG_APP_USE_HTTP
#include <app_http_server/setup.hh>
#endif
#include <app_cli/cli_app.hh>
#include <app_misc/rtc.h>
#include <app_misc/timer.h>
#include <app_settings/app_settings.hh>
#include <app_settings/config.h>
#include <app_uout/status_output.h>
#include <fernotron/alias/pairings.h>
#include <fernotron/auto/fau_next_event.hh>
#include <fernotron/auto/fau_tdata_store.h>
#include <fernotron/cuas/cuid_auto_set.h>
#include <fernotron/fer_main.h>
#include <fernotron/fer_pct.h>
#include <fernotron/pos/positions_dynamic.h>
#include <fernotron/pos/positions_static.h>
#include <fernotron/sep/set_endpos.h>
#include <fernotron_trx/fer_trx_api.hh>
#include <kvs/kvs_wrapper.h>
#include <main_loop/main_queue.hh>
#include <net/ipnet.h>
#include <storage/storage.h>
#include <uout/uo_callbacks.h>
#include <utils_time/run_time.h>
#include <stdint.h>
#include <config_kvs/register_settings.hh>
#include <debug/log.h>
#include <net/network_connection.hh>

#include <esp_sntp.h>
#include <esp_event.h>

#include <freertos/FreeRTOS.h>
#include <freertos/projdefs.h>

#include <ctime>

#define logtag "main"

#define CONFIG_NETWORK_WAIT_FOR_AP_LOGINS_S  120

void publish_ipAddress() {
  char buf[20];
  ipnet_addr_as_string(buf, 20);
  uoCb_publish_ipAddress(buf);
}

void nwc_retryDefaultConnection_later(unsigned delay_secs) {
  static void *tmr;
  mainLoop_stopFun(tmr);

  if (!(tmr = mainLoop_callFunByTimer([]() {
    if (network_fallback == nwWlanAp && wifiAp_is_active() && wifiAp_get_connections_since_ap_start())
      return;
    if (ipnet_isConnected())
      return;

    nwc_connect_default();

  }, pdMS_TO_TICKS(1000 * delay_secs)))) {
    db_loge(logtag, "%s:could not get timer for delayed function call", __func__);
  }
}

void nwc_tryFallback_later() {
  static void *tmr;
  mainLoop_stopFun(tmr);

  if (!(tmr = mainLoop_callFunByTimer([]() {
    if (network_default == nwWlanAp && wifiAp_is_active() && wifiAp_get_connections_since_ap_start())
      return;
    if (ipnet_isConnected())
      return;

    nwc_connect_fallback();
    nwc_retryDefaultConnection_later(network_fallback == nwWlanAp ? CONFIG_NETWORK_WAIT_FOR_AP_LOGINS_S : 15);
  }, pdMS_TO_TICKS(1000 * 5)))) {
    db_loge(logtag, "%s:could not get timer for delayed function call", __func__);
  }
}

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    db_logi(logtag, "ntp synced: %llu\n", (long unsigned long) time(NULL));
    mainLoop_callFun([]() {
      next_event_te.fer_am_updateTimerEvent(time(NULL));
      dst_init();
    });
  });
  config_setup_ntpClient();
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

void init_gpio() {
#ifdef CONFIG_APP_USE_GPIO_PINS
  //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
  struct pin_change_cb {
    static void IRAM_ATTR cb() {
      mainLoop_callFun_fromISR(pin_notify_input_change);
    }
  };
  gpio_INPUT_PIN_CHANGED_ISR_cb = pin_change_cb::cb;
#endif
  config_setup_gpio();
}

void init_fernotron() {
#ifdef CONFIG_APP_USE_FER_RECEIVER
  //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
  struct msg_received_cb {
    static void IRAM_ATTR cb() {
      mainLoop_callFun_fromISR(fer_rx_loop);
    }
  };
  Fer_Trx_API::register_callback_msgReceived_ISR(msg_received_cb::cb);
#endif

#ifdef CONFIG_APP_USE_FER_TRANSMITTER
  //  No lambda here, because section attributes (IRAM_ATTR) do not work on it
  struct msg_transmitted_cb {
    static void IRAM_ATTR cb() {
      mainLoop_callFun_fromISR(fer_tx_loop);
    }
  };
  Fer_Trx_API::register_callback_msgTransmitted_ISR(msg_transmitted_cb::cb);
#endif
#ifdef CONFIG_APP_USE_SEP
  fer_sep_enable_disable_cb = [](bool enable) {
    mainLoop_PeriodicFlags.flags.lf_loopFerSep = enable;
  };
#endif
#ifdef CONFIG_APP_USE_CUAS
  fer_cuas_enable_disable_cb = [](bool enable, uint32_t cu) {
    mainLoop_PeriodicFlags.flags.lf_checkCuasTimeout = enable;
    config_save_item_n_u32(comp_sett.get_kvsKey(CB_CUID), cu);
    config_item_modified(CB_CUID);
  };
#endif

  next_event_te.fer_am_updateTimerEvent(time(NULL));
  mainLoop_callFunByTimer(fer_am_loop, 1000, true);
}

void init_noisr_callbacks() {

}

void init_network_connection() {
#ifdef CONFIG_APP_USE_NETWORK
  ipnet_CONNECTED_cb = main_setup_ip_dependent;

  enum nwConnection network = config_read_network_connection();

#ifdef CONFIG_APP_USE_AP_FALLBACK
  if (network != nwNone)
#endif
      {
#ifdef CONFIG_APP_USE_HTTP
    hts_setup_content();
#endif

    switch (network) {
    case nwWlanSta:
    case nwLan:
#ifdef CONFIG_APP_USE_AP_FALLBACK
      network_fallback = nwWlanAp;
      [[fallthrough]];
#endif
    case nwWlanAp:
      nwc_connect_default(network);
      break;
    case nwLanOrWlanSta:
      network_fallback = nwWlanSta;
      nwc_connect_default(nwLan);
      break;
    default:
      break;
    }

  }

  if (network_fallback != nwNone)
    nwc_tryFallback_later();
#endif // CONFIG_APP_USE_NETWORK
}

void mcu_init() {
  if (esp_event_loop_create_default() != ESP_OK)
    abort();
  if (!mainLoop_setup(32))
    abort();


  // init early required components like kvs, txtio
  kvs_setup();
  config_ext_setup_txtio();
  fputs("\r\n\r\n", stdout);
  config_setup_global();


  // CLI
  cliApp_setup();
#ifdef CONFIG_APP_USE_CLI_TASK
  cli_setup_task(true);
#else
#error "currently unsupported" // FIXME: support CLI without its own task, or remove that APP_USE_CLI_TASK kconfig option
#endif

  // Fernotron
  init_fernotron();

  // try to open default or fall-back connection
  init_network_connection();

  if (auto smCt = app_safeMode_increment(); smCt > 6) {
    app_safe_mode = true;
  }

  // User defined/controllable GPIOs
  init_gpio();
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
