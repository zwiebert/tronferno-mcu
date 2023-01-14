#pragma once

class UoutWriter;

void soCfg_all(const class UoutWriter &td);
void soCfg_all_part(const class UoutWriter &td, int part_num, int part_size);
void soCfg_all_net(const class UoutWriter &td);
void soCfg_all_gpio(const class UoutWriter &td);
void soCfg_all_fer(const class UoutWriter &td);
void soCfg_all_time(const class UoutWriter &td);

void soCfg_BAUD(const UoutWriter &td);
void soCfg_RTC(const UoutWriter &td);
void soCfg_CU(const UoutWriter &td);
void soCfg_NETWORK(const UoutWriter &td);
void soCfg_TZ(const UoutWriter &td);
void soCfg_LONGITUDE(const UoutWriter &td);
void soCfg_LATITUDE(const UoutWriter &td);
void soCfg_TIMEZONE(const UoutWriter &td);
void soCfg_DST(const UoutWriter &td);
void soCfg_GM_USED(const UoutWriter &td);
void soCfg_GPIO_RFOUT(const UoutWriter &td);
void soCfg_GPIO_RFIN(const UoutWriter &td);
void soCfg_GPIO_SETBUTTON(const UoutWriter &td);
void soCfg_RF_TRX(const class UoutWriter &td);
void soCfg_RF_REPEATER(const class UoutWriter &td);
void soCfg_GPIO_RFSCK(const UoutWriter &td);
void soCfg_GPIO_RFMOSI(const UoutWriter &td);
void soCfg_GPIO_RFMISO(const UoutWriter &td);
void soCfg_GPIO_RFSS(const UoutWriter &td);
void soCfg_CC1101_CONFIG(const class UoutWriter &td);

void soCfg_GPIO_PIN(const UoutWriter &td, int gpio_number);
void soCfg_GPIO_MODES(const UoutWriter &td);
void soCfg_GPIO_MODES_AS_STRING(const UoutWriter &td);
void soCfg_ASTRO_CORRECTION(const UoutWriter &td);
void soCfg_begin(const UoutWriter &td);
void soCfg_end(const UoutWriter &td);




#ifdef CONFIG_APP_USE_LAN
void soCfg_LAN_PHY(const class UoutWriter &td);
void soCfg_LAN_PWR_GPIO(const class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_WLAN
void soCfg_WLAN_SSID(const class UoutWriter &td);
void soCfg_WLAN_PASSWORD(const class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_NTP
void soCfg_NTP_SERVER(const class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_MQTT
void soCfg_MQTT(const class UoutWriter &td);
void soCfg_MQTT_ENABLE(const class UoutWriter &td);
void soCfg_MQTT_URL(const class UoutWriter &td);
void soCfg_MQTT_USER(const class UoutWriter &td);
void soCfg_MQTT_PASSWORD(const class UoutWriter &td);
void soCfg_MQTT_CLIENT_ID(const class UoutWriter &td);
void soCfg_MQTT_ROOT_TOPIC(const class UoutWriter &td);
#endif

#ifdef CONFIG_APP_USE_HTTP
void soCfg_HTTP(const class UoutWriter &td);
void soCfg_HTTP_ENABLE(const class UoutWriter &td);
void soCfg_HTTP_USER(const class UoutWriter &td);
void soCfg_HTTP_PASSWORD(const class UoutWriter &td);
#endif
void soCfg_VERBOSE(const class UoutWriter &td);
