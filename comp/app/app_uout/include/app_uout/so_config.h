#pragma once

struct TargetDesc;

void soCfg_all(const struct TargetDesc &td);
void soCfg_all_part(const struct TargetDesc &td, int part_num, int part_size);
void soCfg_all_net(const struct TargetDesc &td);
void soCfg_all_gpio(const struct TargetDesc &td);
void soCfg_all_fer(const struct TargetDesc &td);
void soCfg_all_time(const struct TargetDesc &td);

void soCfg_BAUD(const TargetDesc &td);
void soCfg_RTC(const TargetDesc &td);
void soCfg_CU(const TargetDesc &td);
void soCfg_NETWORK(const TargetDesc &td);
void soCfg_TZ(const TargetDesc &td);
void soCfg_LONGITUDE(const TargetDesc &td);
void soCfg_LATITUDE(const TargetDesc &td);
void soCfg_TIMEZONE(const TargetDesc &td);
void soCfg_DST(const TargetDesc &td);
void soCfg_GM_USED(const TargetDesc &td);
void soCfg_GPIO_RFOUT(const TargetDesc &td);
void soCfg_GPIO_RFIN(const TargetDesc &td);
void soCfg_GPIO_SETBUTTON(const TargetDesc &td);
void soCfg_RF_TRX(const struct TargetDesc &td);
void soCfg_RF_REPEATER(const struct TargetDesc &td);
void soCfg_GPIO_RFSCK(const TargetDesc &td);
void soCfg_GPIO_RFMOSI(const TargetDesc &td);
void soCfg_GPIO_RFMISO(const TargetDesc &td);
void soCfg_GPIO_RFSS(const TargetDesc &td);
void soCfg_CC1101_CONFIG(const struct TargetDesc &td);

void soCfg_GPIO_PIN(const TargetDesc &td, int gpio_number);
void soCfg_GPIO_MODES(const TargetDesc &td);
void soCfg_GPIO_MODES_AS_STRING(const TargetDesc &td);
void soCfg_ASTRO_CORRECTION(const TargetDesc &td);
void soCfg_begin(const TargetDesc &td);
void soCfg_end(const TargetDesc &td);




#ifdef USE_LAN
void soCfg_LAN_PHY(const struct TargetDesc &td);
void soCfg_LAN_PWR_GPIO(const struct TargetDesc &td);
#endif
#ifdef USE_WLAN
void soCfg_WLAN_SSID(const struct TargetDesc &td);
void soCfg_WLAN_PASSWORD(const struct TargetDesc &td);
#endif
#ifdef USE_NTP
void soCfg_NTP_SERVER(const struct TargetDesc &td);
#endif
#ifdef USE_MQTT
void soCfg_MQTT_ENABLE(const struct TargetDesc &td);
void soCfg_MQTT_URL(const struct TargetDesc &td);
void soCfg_MQTT_USER(const struct TargetDesc &td);
void soCfg_MQTT_PASSWORD(const struct TargetDesc &td);
void soCfg_MQTT_CLIENT_ID(const struct TargetDesc &td);
void soCfg_MQTT_ROOT_TOPIC(const struct TargetDesc &td);
#endif

#ifdef USE_HTTP
void soCfg_HTTP_ENABLE(const struct TargetDesc &td);
void soCfg_HTTP_USER(const struct TargetDesc &td);
void soCfg_HTTP_PASSWORD(const struct TargetDesc &td);
#endif
void soCfg_VERBOSE(const struct TargetDesc &td);
