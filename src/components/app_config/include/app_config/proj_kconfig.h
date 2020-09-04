#pragma once
#include <sdkconfig.h>


#ifdef CONFIG_TF_USE_HTTP
#define USE_HTTP
#else
#define NO_HTTP
#endif

#ifdef CONFIG_TF_USE_LAN
#define USE_LAN
#else
#define NO_LAN
#endif

#ifdef CONFIG_TF_USE_WLAN
#define USE_WLAN
#else
#define NO_WLAN
#endif

#ifdef CONFIG_TF_USE_WLAN_AP
#define USE_WLAN_AP
#else
#define NO_WLAN_AP
#endif

#ifdef CONFIG_TF_USE_NTP
#define USE_NTP
#else
#define NO_NTP
#endif


#ifdef CONFIG_TF_USE_JSON
#define USE_JSON
#else
#define NO_JSON
#endif


#ifdef CONFIG_TF_USE_MQTT
#define USE_MQTT
#else
#define NO_MQTT
#endif

#ifdef CONFIG_TF_USE_POSIX_TIME
#define USE_POSIX_TIME
#else
#define NO_POSIX_TIME
#endif

#ifdef CONFIG_TF_USE_SERIAL
#define USE_SERIAL
#else
#define NO_SERIAL
#endif

#ifdef CONFIG_TF_USE_WDG
#define USE_WDG
#else
#define NO_WDG
#endif

#ifdef CONFIG_TF_USE_FS
#define USE_FS
#else
#define NO_FS
#endif

#ifdef CONFIG_TF_USE_HTTP_CLIENT
#define USE_HTTP_CLIENT
#else
#define NO_HTTP_CLIENT
#endif

#ifdef CONFIG_TF_USE_OTA
#define USE_OTA
#else
#define NO_OTA
#endif

#ifdef CONFIG_TF_USE_STM32OTA
#define USE_STM32OTA
#else
#define NO_STM32OTA
#endif

#ifdef CONFIG_TF_USE_TCPS_TASK
#define USE_TCPS_TASK
#else
#define NO_TCPS_TASK
#endif

#ifdef CONFIG_TF_USE_TCPS
#define USE_TCPS
#else
#define NO_TCPS
#endif

#ifdef CONFIG_TF_USE_EG
#define USE_EG
#else
#define NO_EG
#endif

#ifdef CONFIG_TF_USE_LPH
#define USE_LPH
#else
#define NO_LPH
#endif

#ifdef CONFIG_TF_USE_WS
#define USE_WS
#else
#define NO_WS
#endif

#ifdef CONFIG_TF_USE_CLI_MUTEX
#define USE_CLI_MUTEX
#else
#define NO_CLI_MUTEX
#endif

#ifdef CONFIG_TF_USE_NETWORK
#define USE_NETWORK
#else
#define NO_NETWORK
#endif

#ifdef CONFIG_TF_USE_STM32COM
#define USE_STM32COM
#else
#define NO_STM32COM
#endif

#ifdef CONFIG_TF_USE_AP_FALLBACK
#define USE_AP_FALLBACK
#else
#define NO_AP_FALLBACK
#endif


#ifdef __cplusplus

#ifdef CONFIG_TF_USE_HTTP
constexpr bool use_HTTP = true;
#else
constexpr bool use_HTTP = false;
#endif

#ifdef CONFIG_TF_USE_LAN
constexpr bool use_LAN = true;
#else
constexpr bool use_LAN = false;
#endif

#ifdef CONFIG_TF_USE_WLAN
constexpr bool use_WLAN = true;
#else
constexpr bool use_WLAN = false;
#endif

#ifdef CONFIG_TF_USE_WLAN_AP
constexpr bool use_WLAN_AP = true;
#else
constexpr bool use_WLAN_AP = false;
#endif

#ifdef CONFIG_TF_USE_NTP
constexpr bool use_NTP = true;
#else
constexpr bool use_NTP = false;
#endif


#ifdef CONFIG_TF_USE_JSON
constexpr bool use_JSON = true;
#else
constexpr bool use_JSON = false;
#endif

#ifdef CONFIG_TF_USE_MQTT
constexpr bool use_MQTT = true;
#else
constexpr bool use_MQTT = false;
#endif

#ifdef CONFIG_TF_USE_POSIX_TIME
constexpr bool use_POSIX_TIME = true;
#else
constexpr bool use_POSIX_TIME = false;
#endif

#ifdef CONFIG_TF_USE_SERIAL
constexpr bool use_SERIAL = true;
#else
constexpr bool use_SERIAL = false;
#endif

#ifdef CONFIG_TF_USE_WDG
constexpr bool use_WDG = true;
#else
constexpr bool use_WDG = false;
#endif

#ifdef CONFIG_TF_USE_FS
constexpr bool use_FS = true;
#else
constexpr bool use_FS = false;
#endif

#ifdef CONFIG_TF_USE_HTTP_CLIENT
constexpr bool use_HTTP_CLIENT = true;
#else
constexpr bool use_HTTP_CLIENT = false;
#endif

#ifdef CONFIG_TF_USE_OTA
constexpr bool use_OTA = true;
#else
constexpr bool use_OTA = false;
#endif

#ifdef CONFIG_TF_USE_STM32OTA
constexpr bool use_STM32OTA = true;
#else
constexpr bool use_STM32OTA = false;
#endif

#ifdef CONFIG_TF_USE_TCPS_TASK
constexpr bool use_TCPS_TASK = true;
#else
constexpr bool use_TCPS_TASK = false;
#endif

#ifdef CONFIG_TF_USE_TCPS
constexpr bool use_TCPS = true;
#else
constexpr bool use_TCPS = false;
#endif

#ifdef CONFIG_TF_USE_EG
constexpr bool use_EG = true;
#else
constexpr bool use_EG = false;
#endif

#ifdef CONFIG_TF_USE_LPH
constexpr bool use_LPH = true;
#else
constexpr bool use_LPH = false;
#endif

#ifdef CONFIG_TF_USE_WS
constexpr bool use_WS = true;
#else
constexpr bool use_WS = false;
#endif

#ifdef CONFIG_TF_USE_CLI_MUTEX
constexpr bool use_CLI_MUTEX = true;
#else
constexpr bool use_CLI_MUTEX = false;
#endif

#ifdef CONFIG_TF_USE_NETWORK
constexpr bool use_NETWORK = true;
#else
constexpr bool use_NETWORK = false;
#endif


#ifdef CONFIG_TF_USE_AP_FALLBACK
constexpr bool use_AP_FALLBACK = true;
#else
constexpr bool use_AP_FALLBACK = false;
#endif

#endif // __cplusplus
