#include "sun.h"

#include <stdlib.h>
#include <math.h>
#include "main/common.h"





/*
 * calculate sunrise, sunset, twilight
 *
 * sunrise - to return value of sunrise or dawn
 * sunset - to return value of sunset or dusk
 * timezone - time offset to UTC
 * day_of_year - 1..366
 * longitude - geographical longitude (in decimal degrees)
 * latitude - geographical latitude (in decimal degrees)
 * horizon - horizon (in rad) to calculate twilight
 */
void ICACHE_FLASH_ATTR
calc_sunrise_sunset(float *sunrise, float *sunset, float timezone, float day_of_year, float longitude_deg, float latitude_deg, float horizon) {

  float latitude = M_PI * latitude_deg / 180;

  float declination_of_sun = 0.4095 * sin(0.016906 * (day_of_year - 80.086));
  float time_diff = 12 * acos((sin(horizon) - sin(latitude) * sin(declination_of_sun)) / (cos(latitude) * cos(declination_of_sun))) / M_PI;
  float equation_of_time = -0.171 * sin(0.0337 * day_of_year + 0.465) - 0.1299 * sin(0.01787 * day_of_year - 0.168);

  if (sunrise) {
    float sunrise_moz = (12 - time_diff - equation_of_time);
    float sunrise_gmt = sunrise_moz - longitude_deg / 15;
    *sunrise = sunrise_gmt + timezone;
  }

  if (sunset) {
    float sunset_moz = (12 + time_diff - equation_of_time);
    float sunset_gmt = sunset_moz - longitude_deg / 15;
    *sunset = sunset_gmt + timezone;
  }
}
