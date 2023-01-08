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
 * \brief            Calculate when the astro timer event occurs today
 *                   Note: the result may be incorrect if the global astro-table differs from the one which was transmitted to the receiver
 *                   Note/FIXME: the result may be off on some days, because we predict wrong which table index the receiver uses for a given date
 * \param tm         Provide tm_mon, tm_mday, tm_isdst.  The other members of TM are ignored
 * \return minutes   Return the calculated event time in minutes according to TM and the current global astro-table.
 */
uint16_t fer_astro_calc_minutes(const struct tm *tm);

/**
 * \brief    Get astro table index from calendar date
 * \param tm Calender date. Only tm_mon and tm_mday are used, the rest of TM will be ignored
 * \return table_index
 */
int astroTableIndex_from_tm(const struct tm *tm);

/**
 * \brief            Create the global astro table
 * \param cfg_astro  configuration data
 */
void fer_astro_init_and_reinit(const struct cfg_astro *cfg_astro);
