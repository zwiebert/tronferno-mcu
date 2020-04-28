#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "config/config_defaults.h"
#include "app/fernotron.h"
#include "misc/int_types.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

config C;

#ifdef POSIX_TIME
double tz2offset(const char *tz) {
  double offset = 0.0;

  for (const char *p=tz; *p; ++p) {
    if (isalpha(*p))
      continue;
    offset = strtod(p, 0) * -1;
    return offset;
  }
  return 0;
}

#endif
#ifdef POSIX_TIME
const char *config_read_tz(char *d, unsigned d_size) {
 return config_read_item_s(CB_TZ, d, d_size, MY_GEO_TZ);
}
#endif
#ifdef MDR_TIME
float config_read_timezone() {
  return config_read_item_f(CB_TIZO, MY_GEO_TIMEZONE);
}

enum dst config_read_dst() {
  return config_read_item_i8(CB_DST, MY_GEO_DST);
}
#endif
#ifdef USE_NETWORK
enum nwConnection  config_read_network_connection() {
  return config_read_item_i8(CB_NETWORK_CONNECTION, MY_NETWORK_CONNECTION);
}
#endif
float config_read_longitude() {
  return config_read_item_f(CB_LONGITUDE, MY_GEO_LONGITUDE);
}
float config_read_latitude() {
  return config_read_item_f(CB_LATITUDE, MY_GEO_LATITUDE);
}

uint32_t config_read_used_members() {
  return  config_read_item_u32(CB_USED_MEMBERS, MY_FER_GM_USE);
}
int8_t config_read_rfout_gpio() {
  return config_read_item_i8(CB_RFOUT_GPIO, MY_RFOUT_GPIO);
}
int8_t config_read_rfin_gpio() {
  return config_read_item_i8(CB_RFIN_GPIO, MY_RFIN_GPIO);
}
int8_t config_read_setbutton_gpio() {
  return config_read_item_i8(CB_SETBUTTON_GPIO, MY_SETBUTTON_GPIO);
}
enum astroCorrection config_read_astro_correction() {
  return config_read_item_i8(CB_ASTRO_CORRECTION, acAverage);
}
#ifndef MCU_ESP32
uint32_t config_read_baud() {
  return config_read_item_u32(CB_BAUD, MY_MCU_UART_BAUD_RATE);
}
#endif

