
#include "app_settings/config.h"

#include "app_settings/app_settings.hh"

#include "utils_misc/int_types.h"
#include <ctype.h>
#include <stdlib.h>

#define CI(cb) static_cast<configItem>(cb)

config C;

#ifdef CONFIG_APP_USE_POSIX_TIME
double tz2offset(const char *tz) {
  double offset = 0.0;

  for (const char *p = tz; *p; ++p) {
    if (isalpha(*p))
      continue;
    offset = strtod(p, 0) * -1;
    return offset;
  }
  return 0;
}

#endif
#ifdef CONFIG_APP_USE_POSIX_TIME
const char* config_read_tz(char *d, unsigned d_size) {
  return config_read_item((CB_TZ), d, d_size, CONFIG_APP_GEO_TZ);
}
#endif
#ifdef CONFIG_APP_USE_MDR_TIME
float config_read_timezone() {
  return config_read_item((CB_TIZO), MY_GEO_TIMEZONE);
}

enum dst config_read_dst() {
  return config_read_item((CB_DST), MY_GEO_DST);
}
#endif
#ifdef CONFIG_APP_USE_NETWORK
enum nwConnection config_read_network_connection() {
  return static_cast<enum nwConnection>(config_read_item((CB_NETWORK_CONNECTION), nwWlanAp));
}
#endif

#ifndef MCU_ESP32
uint32_t config_read_baud() {
  return config_read_item(CB_BAUD, CONFIG_APP_UART_BAUD_RATE);
}
#endif

