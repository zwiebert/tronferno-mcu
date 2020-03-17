#include "sun.h"

#include <stdlib.h>
#include <math.h>

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
void 
sun_calculateDuskDawn(double *sunrise, double *sunset, double timezone, double day_of_year, double longitude_deg, double latitude_deg, double horizon) {

  double latitude = M_PI * latitude_deg / 180;

  double declination_of_sun = 0.4095 * sin(0.016906 * (day_of_year - 80.086));
  double time_diff = 12 * acos((sin(horizon) - sin(latitude) * sin(declination_of_sun)) / (cos(latitude) * cos(declination_of_sun))) / M_PI;
  double equation_of_time = -0.171 * sin(0.0337 * day_of_year + 0.465) - 0.1299 * sin(0.01787 * day_of_year - 0.168);

  if (sunrise) {
    double sunrise_moz = (12 - time_diff - equation_of_time);
    double sunrise_gmt = sunrise_moz - longitude_deg / 15;
    *sunrise = sunrise_gmt + timezone;
    if (*sunrise > 24.0) *sunrise -= 24.0;
    else if (*sunrise < 0.0) *sunrise += 24.0;
  }

  if (sunset) {
    double sunset_moz = (12 + time_diff - equation_of_time);
    double sunset_gmt = sunset_moz - longitude_deg / 15;
    *sunset = sunset_gmt + timezone;
    if (*sunset > 24.0) *sunset -= 24.0;
    else if (*sunset < 0.0) *sunset += 24.0;
  }
#ifdef postcond
  postcond (!sunrise || (0.0 <=  *sunrise && *sunrise <= 24.0));
  postcond (!sunset || (0.0 <=  *sunset && *sunset <= 24.0));
#endif
}
