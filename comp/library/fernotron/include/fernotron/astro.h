/*
 * astro.h
 *
 * Created: 24.08.2017 22:03:12
 *  Author: bertw
 */ 


#pragma once

#include "fernotron/fer_msg_attachment.h"
#include "time.h"

enum astroCorrection {
  acAverage, acMinimum, acMaximum
};

struct cfg_astro {
  enum astroCorrection astroCorrection;
  float geo_longitude, geo_latitude;
  float geo_timezone;
};


// calculate and fill in astro data
void astro_write_data(astro_byte_data d, int mint_offset);

// calculate astro time like the motor does
uint16_t astro_calc_minutes(const struct tm *tm);

void astro_init_and_reinit(const struct cfg_astro *cfg_astro);

