#include "app_config/proj_app_cfg.h"

#include "main.h"
#include "utils_misc/int_types.h"
#include "net/ipnet.h"
#include "net/tcp_cli_server.h"
#include "net/wifi_ap_setup.h"
#include "net_http_server/http_server_setup.h"
#include "net/tcp_cli_server_setup.hh"
#include "../app_private.h"
#include "cc1101_ook/spi.hh"
#include "main_loop/main_queue.hh"
#include <fernotron_uout/fer_uo_publish.h>

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
    wifiAp_setup(CONFIG_APP_WIFI_AP_SSID, CONFIG_APP_WIFI_AP_PASSWORD);

#ifdef CONFIG_APP_USE_TCPS_TASK
    struct cfg_tcps cfg_tcps = { .enable = true }; // XXX: user-flags not set
    cfg_tcps.flags.evt.uo_evt_flag_pctChange = true;
    cfg_tcps.flags.evt.uo_evt_flag_rfMsgReceived = true;
    tcpCli_setup_task(&cfg_tcps);
#endif

    struct cfg_http cfg_http = { .enable = true };
    hts_setup(&cfg_http);
  }
}

int timer_data_transition_fs_to_kvs();


extern "C" void app_main(void) {

  mcu_init();
  tmr_loopPeriodic100ms_start();
#ifdef TD_FS_TO_KVS
  timer_data_transition_fs_to_kvs();
#endif
  while (1) {
    loop();
  }
}

void  mcu_delayedRestart(unsigned delay_ms) {
  printf("mcu_restart()\n");
  cc1101_ook_spi_disable();
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
  esp_restart();
  for (;;) {
  }
}

void mcu_restart() {
  mainLoop_mcuRestart(1000);
}


