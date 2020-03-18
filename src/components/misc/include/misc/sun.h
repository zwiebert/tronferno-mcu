#ifndef sun_h_
#define sun_h_

/*
 * horizon values for twilight (in radian)
 */
#define CIVIL_TWILIGHT_RAD         -0.10471975511966
#define NAUTICAL_TWILIGHT_RAD      -0.20943951023932
#define ASTRONOMICAL_TWILIGHT_RAD  -0.31415926535898

#define LONGITUDE_DEG_TO_HOUR(longitude)    ((longitude)*0.06666133376)

void sun_calculateDuskDawn(double *sunrise, double *sunset, double timezone, double day_of_year, double longitude_deg, double latitude_deg, double horizon);


#endif
