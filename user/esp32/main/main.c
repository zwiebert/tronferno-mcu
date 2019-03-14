#include "freertos/FreeRTOS.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "../../userio/inout.h"
#include "../main/pin.h"
#include "../main/wifistation.h"
#include "main/common.h"
#include "config/config.h"
#include "fernotron/fer.h"



void setup_tcp_server(void);
void tcps_loop(void);
void setup_timer(void);
void setup_ntp(void);
void setup_storage(void);

static int es_io_putc(char c) {
  putchar(c);
  return 1;
}

static int  es_io_getc(void) {
 return getchar();
}

static void
mcu_init() {

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK( err );

  io_putc_fun = es_io_putc;
  io_getc_fun = es_io_getc;
  io_printf_fun = ets_printf;


#if ENABLE_SPIFFS
  setup_spiffs();
#endif
  read_config(~0);
 // setup_serial(C.mcu_serialBaud);
  io_puts("\r\n\r\n");

  //setup_notImplemented();
  setup_pin();
  setup_wifistation();
#ifdef USE_NTP
  setup_ntp();
#endif
  setup_tcp_server();
  //setup_udp();
  setup_timer();
  setup_storage();
  main_setup();
}

void app_main(void) {

  mcu_init();

  while (1) {
    tcps_loop();
    loop();
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

void  mcu_restart(void) {
  printf("mcu_restart()\n");
  ets_delay_us(10000);
  esp_restart();
  for (;;) {
  };
}

