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





