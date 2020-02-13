#include "../../user_config.h"
#include "freertos/FreeRTOS.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "../../userio/inout.h"
#include "../gpio/pin.h"
#include "../network/wifistation.h"
#include "config/config.h"
#include "fernotron/fer.h"



void setup_tcp_server(void);
void tcps_loop(void);
void setup_timer(void);
void setup_ntp(void);
void setup_storage(void);
void setup_mqtt(void);

static int es_io_putc(char c) {
  putchar(c);
  return 1;
}

static int  es_io_getc(void) {
 return getchar();
}

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;
#ifdef USE_NTP
    setup_ntp();
#endif
    setup_tcp_server();
#ifdef USE_MQTT
    setup_mqtt();
#endif
  }
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
  void config_setup(void);
  config_setup();


 // setup_serial(C.mcu_serialBaud);
  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

#ifdef USE_NETWORK
  void ethernet_setup(void);
  void wifistation_setup(void);
  void wifiAp_setup(void);
  switch (C.network) {
#ifdef USE_WLAN
  case nwWlanSta:
    esp_netif_init();
    wifistation_setup();
    break;
#endif
#ifdef USE_WLAN_AP
  case nwWlanAp:
    esp_netif_init();
    wifiAp_setup();
    break;
#endif
#ifdef USE_LAN
  case nwLan:
    esp_netif_init();
    ethernet_setup();
#endif
    break;
  default:
    break;
  }
#else
  wifistation_setup();
#endif
  setup_pin();

#ifdef USE_MUTEX
  void mutex_setup(void);
  mutex_setup();
#endif
  //setup_udp();
  setup_timer();
  setup_storage();
  main_setup();

}

void app_main(void) {

  mcu_init();

  while (1) {
#ifdef USE_LAN
    void ethernet_loop(void);
    ethernet_loop();
#endif
#ifdef USE_WLAN
    void wifistation_loop(void);
    wifistation_loop();
#endif
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

