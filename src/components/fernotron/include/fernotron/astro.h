/*
 * astro.h
 *
 * Created: 24.08.2017 22:03:12
 *  Author: bertw
 */ 


#ifndef ASTRO_H_
#define ASTRO_H_

#include "fernotron/fer_msg_attachment.h"
#include "time.h"

enum astroCorrection {
  acAverage, acMinimum, acMaximum
};

extern struct astro_cfg {
  float geo_latitude, geo_longitude;
  uint8_t geo_timezone;
  enum astroCorrection astroCorrection;
} astro_cfg;

// calculate and fill in astro data
void astro_write_data(astro_byte_data d, int mint_offset);

// calculate astro time like the motor does
uint16_t astro_calc_minutes(const struct tm *tm);

void astro_init_and_reinit(void);

#endif /* ASTRO_H_ */
