#pragma once

#include "time.h"

enum astroCorrection {
  acAverage, acMinimum, acMaximum
};

struct cfg_astro {
  enum astroCorrection astroCorrection;
  float geo_longitude, geo_latitude;
  float geo_timezone;
};

// calculate astro time like the motor does
uint16_t fer_astro_calc_minutes(const struct tm *tm);
void fer_astro_init_and_reinit(const struct cfg_astro *cfg_astro);
enum astroCorrection config_read_astro_correction();
