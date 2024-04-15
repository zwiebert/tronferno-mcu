
#include "main.hh"
#include "utils_misc/int_types.h"
#include "net/ipnet.h"
#include "net/tcp_cli_server.h"
#include "net/comp_glue.hh"
#include "net/wifi_ap_setup.h"
#ifdef CONFIG_APP_USE_HTTP
#include "net_http_server/http_server_setup.h"
#endif
#include "net/tcp_cli_server_setup.hh"
#include "../app_private.hh"
#include "cc1101_ook/spi.hh"
#include "main_loop/main_queue.hh"
#include <fernotron_uout/fer_uo_publish.h>
#include <net/network_connection.hh>

#include <esp_attr.h>
#include <esp32/rom/ets_sys.h>

#include "freertos/FreeRTOS.h"
#include <freertos/projdefs.h>
#include <freertos/task.h>

#include <cstdio>

int32_t boot_counter;

void lfa_createWifiAp() {
  if (wifiAp_is_active())
    return;
  nwc_connect_fallback(nwWlanAp);

#ifdef CONFIG_APP_USE_TCPS_TASK
  struct cfg_tcps cfg_tcps = { .enable = true }; // XXX: user-flags not set
  cfg_tcps.flags.evt.uo_evt_flag_pctChange = true;
  cfg_tcps.flags.evt.uo_evt_flag_rfMsgReceived = true;
  tcpCli_setup_task(&cfg_tcps);
#endif
#ifdef CONFIG_APP_USE_HTTP
  struct cfg_http cfg_http = { .enable = true };
  hts_setup(&cfg_http);
#endif
}

int timer_data_transition_fs_to_kvs();

extern "C" void app_main(void) {

  mcu_init();
  tmr_loopPeriodic100ms_start();
  while (1) {
    loop();
  }
}

void mcu_delayedRestart(unsigned delay_ms) {
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

