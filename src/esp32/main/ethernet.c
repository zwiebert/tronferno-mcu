/* ethernet Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "user_config.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp32/rom/gpio.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"

#include "userio/ipnet.h"

extern esp_ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;

#ifdef OLIMEX_ESP32_POE

#define USE_PHY_LAN_8720
#define PHY_ADDR 0
#define CONFIG_PHY_POWER_PIN GPIO_NUM_12

#elif defined OLIMEX_ESP32_GATEWAY

#define USE_PHY_LAN_8720
#define PHY_ADDR 0
#define CONFIG_PHY_POWER_PIN GPIO_NUM_5

#else
#define USE_PHY_LAN_8720
#define CONFIG_PHY_POWER_PIN GPIO_NUM_5
#endif


volatile static bool ethernet_connected;
volatile static bool ethernet_disconnected;


static esp_eth_handle_t s_eth_handle = NULL;

static const char *TAG = "eth_example";

#define PIN_PHY_POWER CONFIG_PHY_POWER_PIN

#ifdef CONFIG_PHY_POWER_PIN

static esp_err_t (*orig_pwrctl)(esp_eth_phy_t *phy, bool enable);

/**
 * @brief re-define power enable func for phy
 *
 * @param enable true to enable, false to disable
 *
 * @note This function replaces the default PHY power on/off function.
 * If this GPIO is not connected on your device (and PHY is always powered),
 * you can use the default PHY-specific power on/off function.
 */
static esp_err_t phy_device_power_enable_via_gpio(esp_eth_phy_t *phy, bool enable) {
  esp_err_t result = ESP_OK;

  gpio_pad_select_gpio(PIN_PHY_POWER);
  gpio_set_direction(PIN_PHY_POWER, GPIO_MODE_OUTPUT);

  if (enable) {
    gpio_set_level(PIN_PHY_POWER, 1);
    ESP_LOGI(TAG, "Power On Ethernet PHY");
    vTaskDelay(1); // Allow the power up/down to take effect, min 300us
    if (orig_pwrctl) {
      result = orig_pwrctl(phy, enable);
    }

  } else {
    if (orig_pwrctl) {
      result = orig_pwrctl(phy, enable);
    }
    gpio_set_level(PIN_PHY_POWER, 0);
    ESP_LOGI(TAG, "Power Off Ethernet PHY");
  }

  return result;
}
#endif

/**
 * @note RMII data pins are fixed in esp32:
 * TXD0 <=> GPIO19
 * TXD1 <=> GPIO22
 * TX_EN <=> GPIO21
 * RXD0 <=> GPIO25
 * RXD1 <=> GPIO26
 * CLK <=> GPIO0
 *
 */

/** Event handler for Ethernet events */
static void eth_event_handler(void* arg, esp_event_base_t event_base,
                              i32 event_id, void* event_data)
{
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Up");
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        ethernet_connected = false;
        ethernet_disconnected = true;
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void* arg, esp_event_base_t event_base,
                                 i32 event_id, void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    const esp_netif_ip_info_t* ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");

    ip4_address = ip_info->ip;
    ip4_gateway_address = ip_info->gw;
    ip4_netmask = ip_info->netmask;

    ethernet_disconnected = false;
    ethernet_connected = true;
}

void ethernet_setup() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&cfg);
    // Set default handlers to process TCP/IP stuffs
    ESP_ERROR_CHECK(esp_eth_set_default_handlers(eth_netif));

    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

#ifdef CONFIG_PHY_POWER_PIN
    gpio_pad_select_gpio(PIN_PHY_POWER);
    gpio_set_direction(PIN_PHY_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_PHY_POWER, 1);
#endif

    // Setup MAC
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);

    // Setup PHY
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
#ifdef PHY_ADDR
    phy_config.phy_addr = PHY_ADDR;
#endif
#ifdef USE_PHY_LAN_8720
    esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
#endif
#ifdef CONFIG_PHY_POWER_PIN
    orig_pwrctl = phy->pwrctl;
    phy->pwrctl = phy_device_power_enable_via_gpio;
#endif


    // Ethernet Driver
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &s_eth_handle));

    /* attach Ethernet driver to TCP/IP stack */
      ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(s_eth_handle)));
      /* start Ethernet driver state machine */
      ESP_ERROR_CHECK(esp_eth_start(s_eth_handle));
}

void ethernet_loop() {
  if (ethernet_connected) {
      ipnet_connected();
      ethernet_connected = false;
  } else if (ethernet_disconnected) {
      ipnet_disconnected();
      ethernet_disconnected = false;
  }
}




