/**
 * \file  fernotron/pos/positions_static.h
 * \brief Store and retrieve shutter positions (persistently)
 * \author: bertw
 */

#pragma once

#include "fernotron/types.h"
#include "app_config/proj_app_cfg.h"
#include <stdbool.h>


struct fer_shutterGroupPositionsT {
   operator uint8_t*() { return grpPos_; }
  uint8_t grpPos_[8];
};

// keep track of static positions

/**
 * \brief        Set static position of given shutter or group
 * \param a,g,m  Shutter, group or alias
 * \param pct    Position
 */
void fer_statPos_setPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct);

/**
 * \brief        Set static position for a given set of shutters or group
 * \param mm     Set of shutters/groups
 * \param pct    Position
 */
void fer_statPos_setPcts(Fer_GmSet *mm, uint8_t pct);

/**
 * \brief        Get static position of given shutter or group
 * \param g,m    Shutter or group
 * \return        -1 for error or Position in percent.
 */
Pct fer_statPos_getPct(uint8_t g, uint8_t m);

/**
 * \brief        Output all tracked static positions to user
 * \param td     Output descriptor
 */
void fer_statPos_printAllPcts(const struct TargetDesc &td);

void fer_pos_loop(void);
void fer_pos_init(void);









