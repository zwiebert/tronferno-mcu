#
# Automatically generated file. DO NOT EDIT.
# Espressif IoT Development Framework (ESP-IDF) Configuration cmake include file
#
set(CONFIG_APP_USE_NETWORK "")
set(CONFIG_APP_USE_MQTT "y")
set(CONFIG_APP_MQTT_CLIENT_ID "zb")
set(CONFIG_APP_MQTT_ROOT_TOPIC "tfmcu")
set(CONFIG_APP_USE_POSIX_TIME "y")
set(CONFIG_APP_USE_MUTEX "")
set(CONFIG_APP_USE_ESP_GET_TIME "")
set(CONFIG_APP_USE_GPIO_PINS "")
set(CONFIG_APP_USE_TIMER_TASK "")
set(CONFIG_APP_GEO_TZ "CET-1CEST-2,M3.5.0,M10.5.0")
set(CONFIG_APP_SETTINGS_PASSWORD "")
set(CONFIG_APP_FER_CENTRAL_UNIT_ID "0x801234")
set(CONFIG_APP_FER_GM_USE "0x7777777")
set(CONFIG_APP_WIFI_AP_SSID "tronferno")
set(CONFIG_APP_WIFI_AP_PASSWORD "tronferno")
set(CONFIG_APP_CFG_NAMESPACE "Settings")
set(CONFIG_APP_GPIO_NUMBER_OF_GPIOS "40")
set(CONFIG_MAIN_LOOP_PERIOD_MS "100")
set(CONFIG_NETWORK_CHECK_LOOP_PERIOD_S "15")
set(CONFIG_UOUT_HAVE_COMPONENT "y")
set(CONFIG_CLI_JSON_ROOT_OBJECT "y")
set(CONFIG_CLI_CMDLINE_ALLOW_LF "")
set(CONFIG_CLI_INTERACTIVE_CONSOLE "y")
set(CONFIG_APP_USE_CLI_TASK "y")
set(CONFIG_CLI_DEBUG "")
set(CONFIG_CONFIG_KVS_HAVE_COMPONENT "y")
set(CONFIG_APP_CFG_NAMESPACE "Settings")
set(CONFIG_APP_OTA_USE_CERT_BUNDLE "y")
set(CONFIG_TXTIO_HAVE_COMPONENT "y")
set(CONFIG_APP_UART_BAUD_RATE "115200")
set(CONFIG_WEATHER_OWM_URL_STRING "http://api.openweathermap.org/data/2.5/weather?q=Berlin&APPID=12345678901234567890123456789012")
set(CONFIG_WEATHER_TEST_DEBUG "")
set(CONFIG_NET_HTTP_SERVER_HAVE_COMPONENT "y")
set(CONFIG_APP_HTTP_ENABLE "0")
set(CONFIG_APP_HTTP_USER "")
set(CONFIG_APP_HTTP_PASSWORD "")
set(CONFIG_NET_HTTP_SERVER_DEBUG "")
set(CONFIG_NET_HAVE_COMPONENT "y")
set(CONFIG_APP_TCPS_ENABLE "1")
set(CONFIG_APP_TCPS_PORT "7777")
set(CONFIG_APP_TCPS_PORT_IA "0")
set(CONFIG_APP_TCPS_CONNECTIONS_MAX "-1")
set(CONFIG_NET_HTTP_CLIENT_DEBUG "")
set(CONFIG_NET_NTP_CLIENT_DEBUG "")
set(CONFIG_NET_TCP_CLI_CLIENT_DEBUG "")
set(CONFIG_NET_NETWORK_DEBUG "")
set(CONFIG_NET_MQTT_CLIENT_HAVE_COMPONENT "y")
set(CONFIG_APP_MQTT_CLIENT_ID "zb")
set(CONFIG_APP_MQTT_ROOT_TOPIC "tfmcu")
set(CONFIG_APP_MQTT_URL "")
set(CONFIG_APP_MQTT_ENABLE "0")
set(CONFIG_APP_MQTT_USER "")
set(CONFIG_APP_MQTT_PASSWORD "")
set(CONFIG_NET_MQTT_CLIENT_DEBUG "")
set(CONFIG_UOUT_HAVE_COMPONENT "y")
set(CONFIG_UOUT_PUBLISH_LOG_AS_TXT "")
set(CONFIG_UOUT_PUBLISH_IP_ADDR_AS_TXT "y")
set(CONFIG_REQUIRE_ROOT_OBJECT "y")
set(CONFIG_UOUT_DEBUG "")
set(CONFIG_APP_USE_FER_TRANSMITTER "y")
set(CONFIG_APP_USE_FER_RECEIVER "y")
set(CONFIG_TRONFERNO_TRX_DEBUG "")
set(CONFIG_APP_USE_CUAS "y")
set(CONFIG_APP_USE_SEP "y")
set(CONFIG_APP_USE_TIMER_DATA_FS "")
set(CONFIG_APP_USE_TIMER_DATA_KVS "y")
set(CONFIG_APP_USE_PAIRINGS "y")
set(CONFIG_APP_USE_REPEATER "y")
set(CONFIG_APP_USE_RTC_AUTO_UPD "y")
set(CONFIG_FERNOTRON_APP_DEBUG "")
set(CONFIG_TRONFERNO_PUBLISH_PCT_AS_TXT "")
set(CONFIG_TRONFERNO_PUBLISH_TRX_AS_TXT "")
set(CONFIG_FERNOTRON_UOUT_DEBUG "")
set(CONFIG_UTILS_MISC_MUTEX_DEBUG "")
set(CONFIG_APP_USE_ESP_GET_TIME "")
set(CONFIGS_LIST CONFIG_APP_USE_NETWORK;CONFIG_APP_USE_MQTT;CONFIG_APP_MQTT_CLIENT_ID;CONFIG_APP_MQTT_ROOT_TOPIC;CONFIG_APP_USE_POSIX_TIME;CONFIG_APP_USE_MUTEX;CONFIG_APP_USE_ESP_GET_TIME;CONFIG_APP_USE_GPIO_PINS;CONFIG_APP_USE_TIMER_TASK;CONFIG_APP_GEO_TZ;CONFIG_APP_SETTINGS_PASSWORD;CONFIG_APP_FER_CENTRAL_UNIT_ID;CONFIG_APP_FER_GM_USE;CONFIG_APP_WIFI_AP_SSID;CONFIG_APP_WIFI_AP_PASSWORD;CONFIG_APP_CFG_NAMESPACE;CONFIG_APP_GPIO_NUMBER_OF_GPIOS;CONFIG_MAIN_LOOP_PERIOD_MS;CONFIG_NETWORK_CHECK_LOOP_PERIOD_S;CONFIG_UOUT_HAVE_COMPONENT;CONFIG_CLI_JSON_ROOT_OBJECT;CONFIG_CLI_CMDLINE_ALLOW_LF;CONFIG_CLI_INTERACTIVE_CONSOLE;CONFIG_APP_USE_CLI_TASK;CONFIG_CLI_DEBUG;CONFIG_CONFIG_KVS_HAVE_COMPONENT;CONFIG_APP_CFG_NAMESPACE;CONFIG_APP_OTA_USE_CERT_BUNDLE;CONFIG_TXTIO_HAVE_COMPONENT;CONFIG_APP_UART_BAUD_RATE;CONFIG_WEATHER_OWM_URL_STRING;CONFIG_WEATHER_TEST_DEBUG;CONFIG_NET_HTTP_SERVER_HAVE_COMPONENT;CONFIG_APP_HTTP_ENABLE;CONFIG_APP_HTTP_USER;CONFIG_APP_HTTP_PASSWORD;CONFIG_NET_HTTP_SERVER_DEBUG;CONFIG_NET_HAVE_COMPONENT;CONFIG_APP_TCPS_ENABLE;CONFIG_APP_TCPS_PORT;CONFIG_APP_TCPS_PORT_IA;CONFIG_APP_TCPS_CONNECTIONS_MAX;CONFIG_NET_HTTP_CLIENT_DEBUG;CONFIG_NET_NTP_CLIENT_DEBUG;CONFIG_NET_TCP_CLI_CLIENT_DEBUG;CONFIG_NET_NETWORK_DEBUG;CONFIG_NET_MQTT_CLIENT_HAVE_COMPONENT;CONFIG_APP_MQTT_CLIENT_ID;CONFIG_APP_MQTT_ROOT_TOPIC;CONFIG_APP_MQTT_URL;CONFIG_APP_MQTT_ENABLE;CONFIG_APP_MQTT_USER;CONFIG_APP_MQTT_PASSWORD;CONFIG_NET_MQTT_CLIENT_DEBUG;CONFIG_UOUT_HAVE_COMPONENT;CONFIG_UOUT_PUBLISH_LOG_AS_TXT;CONFIG_UOUT_PUBLISH_IP_ADDR_AS_TXT;CONFIG_REQUIRE_ROOT_OBJECT;CONFIG_UOUT_DEBUG;CONFIG_APP_USE_FER_TRANSMITTER;CONFIG_APP_USE_FER_RECEIVER;CONFIG_TRONFERNO_TRX_DEBUG;CONFIG_APP_USE_CUAS;CONFIG_APP_USE_SEP;CONFIG_APP_USE_TIMER_DATA_FS;CONFIG_APP_USE_TIMER_DATA_KVS;CONFIG_APP_USE_PAIRINGS;CONFIG_APP_USE_REPEATER;CONFIG_APP_USE_RTC_AUTO_UPD;CONFIG_FERNOTRON_APP_DEBUG;CONFIG_TRONFERNO_PUBLISH_PCT_AS_TXT;CONFIG_TRONFERNO_PUBLISH_TRX_AS_TXT;CONFIG_FERNOTRON_UOUT_DEBUG;CONFIG_UTILS_MISC_MUTEX_DEBUG;CONFIG_APP_USE_ESP_GET_TIME)