#pragma once

class UoutWriter;

void soCfg_all(class UoutWriter &td, bool backup = false);
void soCfg_all_part(class UoutWriter &td, int part_num, int part_size);
void soCfg_all_net(class UoutWriter &td, bool backup = false);
void soCfg_all_gpio(class UoutWriter &td, bool backup = false);
void soCfg_all_fer(class UoutWriter &td, bool backup = false);
void soCfg_all_time(class UoutWriter &td, bool backup = false);

void soCfg_BAUD(UoutWriter &td);
void soCfg_RTC(UoutWriter &td);
void soCfg_CU(UoutWriter &td);
void soCfg_NETWORK(UoutWriter &td);
void soCfg_TZ(UoutWriter &td);
void soCfg_LONGITUDE(UoutWriter &td);
void soCfg_LATITUDE(UoutWriter &td);
void soCfg_TIMEZONE(UoutWriter &td);
void soCfg_DST(UoutWriter &td);
void soCfg_GM_USED(UoutWriter &td);
void soCfg_GPIO_RFOUT(UoutWriter &td);
void soCfg_GPIO_RFIN(UoutWriter &td);
void soCfg_GPIO_SETBUTTON(UoutWriter &td);
void soCfg_RF_TRX(class UoutWriter &td);
void soCfg_RF_REPEATER(class UoutWriter &td);
void soCfg_RTC_AUTO_UPD(class UoutWriter &td);
void soCfg_GPIO_RFSCK(UoutWriter &td);
void soCfg_GPIO_RFMOSI(UoutWriter &td);
void soCfg_GPIO_RFMISO(UoutWriter &td);
void soCfg_GPIO_RFSS(UoutWriter &td);
void soCfg_CC1101_CONFIG(class UoutWriter &td);

void soCfg_GPIO_PIN(UoutWriter &td, int gpio_number);
void soCfg_GPIO_MODES(UoutWriter &td);
void soCfg_GPIO_MODES_AS_STRING(UoutWriter &td);
void soCfg_ASTRO_CORRECTION(UoutWriter &td);
void soCfg_begin(UoutWriter &td);
void soCfg_end(UoutWriter &td);




#ifdef CONFIG_APP_USE_LAN
void soCfg_LAN_PHY(class UoutWriter &td);
void soCfg_LAN_PWR_GPIO(class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_WLAN
void soCfg_WLAN_SSID(class UoutWriter &td);
void soCfg_WLAN_PASSWORD(class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_NTP
void soCfg_NTP_SERVER(class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_MQTT
void soCfg_MQTT(class UoutWriter &td, bool backup = false);
void soCfg_MQTT_ENABLE(class UoutWriter &td);
void soCfg_MQTT_URL(class UoutWriter &td);
void soCfg_MQTT_USER(class UoutWriter &td);
void soCfg_MQTT_PASSWORD(class UoutWriter &td);
void soCfg_MQTT_CLIENT_ID(class UoutWriter &td);
void soCfg_MQTT_ROOT_TOPIC(class UoutWriter &td);
#endif

#ifdef CONFIG_APP_USE_HTTP
void soCfg_HTTP(class UoutWriter &td);
void soCfg_HTTP_ENABLE(class UoutWriter &td);
void soCfg_HTTP_USER(class UoutWriter &td);
void soCfg_HTTP_PASSWORD(class UoutWriter &td);
#endif
void soCfg_VERBOSE(class UoutWriter &td);
