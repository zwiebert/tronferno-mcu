#pragma once

void soCfg_BAUD(const struct TargetDesc &td);
void soCfg_RTC(const struct TargetDesc &td);
void soCfg_CU(const struct TargetDesc &td);
void soCfg_NETWORK(const struct TargetDesc &td);
void soCfg_TZ(const struct TargetDesc &td);
void soCfg_LONGITUDE(const struct TargetDesc &td);
void soCfg_LATITUDE(const struct TargetDesc &td);
void soCfg_TIMEZONE(const struct TargetDesc &td);
void soCfg_DST(const struct TargetDesc &td);
void soCfg_GM_USED(const struct TargetDesc &td);
void soCfg_GPIO_RFOUT(const struct TargetDesc &td);
void soCfg_GPIO_RFIN(const struct TargetDesc &td);
void soCfg_GPIO_SETBUTTON(const struct TargetDesc &td);
void soCfg_GPIO_PIN(const struct TargetDesc &td, int gpio_number);
void soCfg_GPIO_MODES(const struct TargetDesc &td);
void soCfg_GPIO_MODES_AS_STRING(const struct TargetDesc &td);
void soCfg_ASTRO_CORRECTION(const struct TargetDesc &td);
void soCfg_begin(const struct TargetDesc &td);
void soCfg_end(const struct TargetDesc &td);
