#include "user_config.h"
#ifdef USE_WLAN
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "esp32/rom/uart.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "userio/ipnet.h"
#include "userio/inout.h"
#include "main/common.h"
#include "config/config.h"

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif

// WIFI Station ////////////////////////////////////////

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

extern esp_ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;


void
user_set_station_config(void) {
  esp_err_t esp_err;

  wifi_config_t sta_config = { };

  strncpy((char*) sta_config.sta.ssid, C.wifi_SSID, sizeof sta_config.sta.ssid - 1);
  strncpy((char*) sta_config.sta.password, C.wifi_password, sizeof sta_config.sta.password - 1);
  sta_config.sta.bssid_set = false;
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}

static int s_retry_num = 0;
const char *TAG = "wifistation";

//#define RETRY_RECONNECT (s_retry_num < 255)
#define RETRY_RECONNECT (1)

volatile static bool wifistation_connected;
volatile static bool wifistation_disconnected;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
        i32 event_id, void* event_data) {

    switch (event_id) {

    case WIFI_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAG, "wifi sta start");
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "wifi sta disconnected");
        wifistation_connected = false;
        wifistation_disconnected = true;
        {
            if (RETRY_RECONNECT) {
                ip4_address.addr = 0;
                esp_wifi_connect();
                xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                s_retry_num++;
                // ESP_LOGI(TAG, "retry to connect to the AP");
            }
            break;
        }

    default:
        break;
    }
}

/** Event handler for IP_EVENT_STA_GOT_IP */
static void got_ip_event_handler(void* arg, esp_event_base_t event_base,
        i32 event_id, void* event_data) {

    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "STA Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "NETMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "GW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");

    ip4_address = ip_info->ip;
    ip4_gateway_address = ip_info->gw;
    ip4_netmask = ip_info->netmask;

    wifistation_disconnected = false;
    wifistation_connected = true;
}

void
wifistation_setup(void) {
  s_wifi_event_group = xEventGroupCreate();

  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &got_ip_event_handler, NULL));

  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  user_set_station_config();
}

void wifistation_loop() {
  if (wifistation_connected) {
      ipnet_connected();
      wifistation_connected = false;
  } else if (wifistation_disconnected) {
      ipnet_disconnected();
      wifistation_disconnected = false;
  }
}

#endif //USE_WLAN
