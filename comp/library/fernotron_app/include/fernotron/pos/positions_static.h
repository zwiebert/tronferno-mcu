/**
 * \file  fernotron/pos/positions_static.h
 * \brief Store and retrieve shutter positions (persistently)
 * \author: bertw
 */

#pragma once

#include "fernotron/types.h"
#include "app_config/proj_app_cfg.h"
#include <stdbool.h>


/**
 * \brief              Will be called if a position  has changed or has been saved.
 * \param has_unsaved  If true, a changed position needs saving; If false, all positions were saved successfully. XXX
 */
extern void (*fer_pos_POSITIONS_SAVE_cb)(bool has_unsaved);

struct fer_shutterGroupPositionsT {
   operator uint8_t*() { return grpPos_; }
  uint8_t grpPos_[8];
};

// keep track of static positions

/**
 * \brief        Set static position of given shutter or group
 * \param a,g,m  Shutter, group or alias
 * \param pct    Position
 * \return        -1 for error; 0 for success.
 */
int fer_statPos_setPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct);

/**
 * \brief        Set static position for a given set of shutters or group
 * \param mm     Set of shutters/groups
 * \param pct    Position
 * \return        -1 for error; 0 for success.
 */
int fer_statPos_setPcts(Fer_GmSet *mm, uint8_t pct);

/**
 * \brief        Get static position of given shutter or group
 * \param a,g,m  Shutter, group or alias
 * \return        -1 for error or Position in percent.
 */
int fer_statPos_getPct(uint32_t a, uint8_t g, uint8_t m);

/**
 * \brief        Output all tracked static positions to user
 * \param td     Output descriptor
 * \return        -1 for error; 0 for success.
 */
int fer_statPos_printAllPcts(const struct TargetDesc &td);

void fer_statPos_loopAutoSave();

void fer_pos_loop(void);
void fer_pos_init(void);









