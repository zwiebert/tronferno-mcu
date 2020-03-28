
#include "main.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <lwip/apps/sntp.h>
#include <lwip/apps/sntp_opts.h>
#include <esp_attr.h>

SemaphoreHandle_t uart_mutex;
i32 boot_counter;
bool wifi_ap_active;

void lfa_createWifiAp() {
  esp_netif_init();
  wifiAp_setup("rv", "12345678");
  wifi_ap_active = true;
}

void lfa_gotIpAddr(void) {
  ipnet_connected();
}
void lfa_lostIpAddr(void) {
  ipnet_disconnected();
}

void appEsp32_main(void) {

  mcu_init();

  tmr_loopPeriodic_start();
  while (1) {
    loop();
#ifndef USE_EG
    vTaskDelay(pdMS_TO_TICKS(LOOP_INTERVAL_MS));
#endif
  }
}

void  mcu_restart(void) {
  printf("mcu_restart()\n");
  ets_delay_us(10000);
  esp_restart();
  for (;;) {
  };
}

#ifdef USE_EG
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
#endif
