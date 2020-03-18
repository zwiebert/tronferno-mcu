#include "app/proj_app_cfg.h"
#include "freertos/FreeRTOS.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "txtio/inout.h"
#include "gpio/pin.h"
#include "net/wifistation.h"
#include "config/config.h"
#include "fernotron/callbacks.h"
#include "fernotron_txtio/fer_print.h"
#include "key_value_store/kvs_wrapper.h"
#include "config/config.h"
#include "storage/storage.h"
#include "net/tcp_server.h"
#include "net/mqtt.h"
#include "net/wifistation.h"
#include "net/ethernet.h"
#include "app/timer.h"

void setup_ntp(void);




void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;
#ifdef USE_NTP
    setup_ntp();
#endif
    tcps_startServer();
#ifdef USE_MQTT
    io_mqtt_setup();
#endif
  }
}



void
mcu_init() {

  kvs_setup();
  txtio_setup();
  config_setup();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

#ifdef USE_NETWORK
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
  intTimer_setup();
  stor_setup();
  main_setup();

}

void appEsp32_main(void) {

  mcu_init();

  while (1) {
#ifdef USE_LAN
    ethernet_loop();
#endif
#ifdef USE_WLAN
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

