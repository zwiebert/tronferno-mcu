#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "../main/pin.h"
#include "../main/wifistation.h"
#include "main/common.h"
#include "main/config.h"
#include "main/fer.h"
#include "main/inout.h"



void setup_tcp_server(void);
void tcps_loop(void);
void setup_timer(void);
void setup_ntp(void);
void setup_storage(void);

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


static int es_io_putc(char c) {
  putchar(c);
  return 1;
}

static int  es_io_getc(void) {
 return getchar();
}

static void
mcu_init() {

  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );



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
#ifdef USE_NTP
  setup_ntp();
#endif
  setup_pin();
  setup_wifistation();
  setup_tcp_server();
  //setup_udp();
  setup_timer();
  setup_storage();
  main_setup();

  //system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
  //system_os_post(user_procTaskPrio, 0, 0);
#if 0
  system_os_task(io_procTask, io_procTaskPrio, io_procTaskQueue, io_procTaskQueueLen);
  system_os_post(io_procTaskPrio, 0, 0 );
#endif

  //setup_timer();

#ifdef DEBUG
  //gdbstub_init();
#endif
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

