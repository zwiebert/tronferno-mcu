#pragma once

struct TargetDesc;

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
void soCfg_GPIO_PIN(const TargetDesc &td, int gpio_number);
void soCfg_GPIO_MODES(const TargetDesc &td);
void soCfg_GPIO_MODES_AS_STRING(const TargetDesc &td);
void soCfg_ASTRO_CORRECTION(const TargetDesc &td);
void soCfg_begin(const TargetDesc &td);
void soCfg_end(const TargetDesc &td);
