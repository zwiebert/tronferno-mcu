#include "app_config/proj_app_cfg.h"

#include "main.h"
#include "utils_misc/int_types.h"
#include "net/ipnet.h"
#include "net/tcp_cli_server.h"
#include "net/wifi_ap_setup.h"
#include "net_http_server/http_server_setup.h"
#include "net/tcp_cli_server_setup.hh"
#include "../app_private.h"

#include <esp_attr.h>
#include <esp32/rom/ets_sys.h>
#include <freertos/projdefs.h>
#include <freertos/task.h>
#include <cstdio>


i32 boot_counter;

void lfa_createWifiAp() {
  static bool wifi_ap_active;

  if (!wifi_ap_active) {
    wifi_ap_active = true;
    wifiAp_setup(WIFI_AP_SSID, WIFI_AP_PASSWD);

    struct cfg_tcps cfg_tcps = { .enable = true }; // XXX: user-flags not set
#ifdef USE_TCPS
 tcpCli_setup(&cfg_tcps);
#elif defined USE_TCPS_TASK
 tcpCli_setup_task(&cfg_tcps);
#endif

    struct cfg_http cfg_http = { .enable = true };
    hts_setup(&cfg_http);
  }
}

int timer_data_transition_fs_to_kvs();


extern "C" void app_main(void) {

  mcu_init();
  tmr_loopPeriodic_start();
#ifdef TD_FS_TO_KVS
  timer_data_transition_fs_to_kvs();
#endif
  while (1) {
    loop();
  }
}

void  mcu_delayedRestart(unsigned delay_ms) {
  printf("mcu_restart()\n");
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
  esp_restart();
  for (;;) {
  }
}

void mcu_restart() {
  lf_setBit(lf_mcuRestart);
}


void IRAM_ATTR lf_setBits_ISR(const EventBits_t uxBitsToSet, bool yield) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;

  xResult = xEventGroupSetBitsFromISR(
      loop_event_group,  // The event group being updated.
      uxBitsToSet, // The bits being set.
      &xHigherPriorityTaskWoken );

  // Was the message posted successfully?
  if (yield && xResult == pdPASS) {
    portYIELD_FROM_ISR();
  }
}

