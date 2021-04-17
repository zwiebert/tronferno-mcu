/**
 * \file   fernotron_trx/astro.h
 * \brief  civil dusk timer calculations
 */
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

/**
 * \brief            Calculate todays time the astro timer event occurs
 *                   Note: the result may be incorrect if the global astro-table differs from the one uploaded to the motor (XXX)
 *                   Note: the result may be off, because sometimes the wrong table entry is picket (FIXME: This is a bug)
 * \param tm         Provide tm_mon, tm_mday, tm_isdst.  The other members of TM are ignored
 * \return minutes   Return the calculated event time in minutes according to TM and the current global astro-table.
 */
uint16_t fer_astro_calc_minutes(const struct tm *tm);

/**
 * \brief            Create the global astro table
 * \param cfg_astro  configuration data
 */
void fer_astro_init_and_reinit(const struct cfg_astro *cfg_astro);


/**
 * \brief Get persistent setting of astro_correction FIXME: This function should be declared in app-settings, where it's defined
 */
enum astroCorrection config_read_astro_correction();
