/**
 * \file   app_config/host/sdkconfig.h
 * \brief  Default configuration values for host (tests)
 */

#define CONFIG_APP_FER_CENTRAL_UNIT_ID 0x801234
#define CONFIG_APP_FER_GM_USE 0x77777777
#define CONFIG_APP_GEO_TZ "CET-1CEST-2,M3.5.0,M10.5.0"
#define CONFIG_APP_HTTP_ENABLE 1
#define CONFIG_APP_HTTP_PASSWORD ""
#define CONFIG_APP_HTTP_USER ""
#define CONFIG_APP_MQTT_CLIENT_ID "tfmcu"
#define CONFIG_APP_MQTT_ENABLE 1
#define CONFIG_APP_MQTT_PASSWORD ""
#define CONFIG_APP_MQTT_ROOT_TOPIC ""
#define CONFIG_APP_MQTT_URL ""
#define CONFIG_APP_MQTT_USER ""
#define CONFIG_APP_NTP_SERVER "gateway"
#define CONFIG_APP_SETTINGS_PASSWORD ""
#define CONFIG_APP_UART_BAUD_RATE 115200
#define CONFIG_APP_USE_AP_FALLBACK 1
#define CONFIG_APP_USE_CLI_TASK 1
#define CONFIG_APP_USE_CUAS 1
#define CONFIG_APP_USE_EG 1
#define CONFIG_APP_USE_ESP_GET_TIME 1
#define CONFIG_APP_USE_FER_RECEIVER 1
#define CONFIG_APP_USE_FER_TRANSMITTER 1
#define CONFIG_APP_USE_FREERTOS 1
#define CONFIG_APP_USE_GPIO_PINS 1
#define CONFIG_APP_USE_HTTP 1
#define CONFIG_APP_USE_HTTP_CLIENT 1
#define CONFIG_APP_USE_JSON 1
#define CONFIG_APP_USE_LAN 1
#define CONFIG_APP_USE_MQTT 1
#define CONFIG_APP_USE_MUTEX 1
#define CONFIG_APP_USE_NETWORK 1
#define CONFIG_APP_USE_NTP 1
#define CONFIG_APP_USE_OTA 1
#define CONFIG_APP_USE_PAIRINGS 1
#define CONFIG_APP_USE_POSIX_TIME 1
#define CONFIG_APP_USE_SEP 1
#define CONFIG_APP_USE_TCPS_TASK 1
#define CONFIG_APP_USE_TIMER_DATA_KVS 1
#define CONFIG_APP_USE_TIMER_TASK 1
#define CONFIG_APP_USE_TXTIO_MUTEX 1
#define CONFIG_APP_USE_WDG 1
#define CONFIG_APP_USE_WLAN 1
#define CONFIG_APP_USE_WLAN_AP 1
#define CONFIG_APP_USE_WS 1
#define CONFIG_APP_WIFI_PASSWORD ""
#define CONFIG_APP_WIFI_SSID ""
#define CONFIG_APP_CFG_NAMESPACE "Tronferno"
#define CONFIG_APP_GPIO_NUMBER_OF_GPIOS 40
#define CONFIG_MAIN_LOOP_PERIOD_MS 100
#define CONFIG_NETWORK_CHECK_LOOP_PERIOD_S 15
#define CONFIG_APP_TCPS_ENABLE 1



#undef CONFIG_APP_USE_HTTP
#undef CONFIG_APP_USE_LAN
#undef CONFIG_APP_USE_WLAN
#undef CONFIG_APP_USE_WLAN_AP
#undef CONFIG_APP_USE_NTP
//#undef CONFIG_APP_USE_JSON
//#undef CONFIG_APP_USE_MQTT
//#undef CONFIG_APP_USE_POSIX_TIME
#undef CONFIG_APP_USE_HTTP_CLIENT
#undef CONFIG_APP_USE_OTA
#undef CONFIG_APP_USE_TCPS
#undef CONFIG_APP_USE_TCPS_TASK
#undef CONFIG_APP_USE_EG
#undef CONFIG_APP_USE_SEP
#undef CONFIG_APP_USE_WS
#undef CONFIG_APP_USE_MUTEX
#undef CONFIG_APP_USE_CLI_TASK
#undef CONFIG_APP_USE_TXTIO_MUTEX
#undef CONFIG_APP_USE_NETWORK
#undef CONFIG_APP_USE_AP_FALLBACK
#undef CONFIG_APP_USE_ESP_GET_TIME
//#undef CONFIG_APP_USE_FER_TRANSMITTER
//#undef CONFIG_APP_USE_FER_RECEIVER
#undef CONFIG_APP_USE_PAIRINGS
#undef CONFIG_APP_USE_GPIO_PINS
#undef CONFIG_APP_USE_TIMER_DATA_FS
//#undef CONFIG_APP_USE_TIMER_DATA_KVS
#undef CONFIG_APP_USE_FREERTOS
#undef CONFIG_APP_USE_MDR_TIME

