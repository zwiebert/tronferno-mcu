#ifndef sun_h_
#define sun_h_

/*
 * horizon values for twilight (in radian)
 */
#define CIVIL_TWILIGHT_RAD         -0.10471975511966
#define NAUTICAL_TWILIGHT_RAD      -0.20943951023932
#define ASTRONOMICAL_TWILIGHT_RAD  -0.31415926535898

#define float double


void calc_sunrise_sunset(float *sunrise, float *sunset, float timezone, float day_of_year, float longitude_deg, float latitude_deg, float horizon);


#endif
