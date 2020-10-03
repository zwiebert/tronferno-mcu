#pragma once

/**
 * \file     app/opt_map.hh
 * \brief    mapping options strings to enum otok
 */

#include <stdint.h>
#include <app/config/options.hh>

/**
 * \brief get enum for KEY or otok::NONE
 */
otok optMap_findToken(const char *key);

