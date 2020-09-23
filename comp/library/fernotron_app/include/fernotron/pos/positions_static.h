/*
 * positions_static.h
 *
 *  Created on: 05.04.2020
 *      Author: bertw
 */

#pragma once

#include "fernotron/types.h"
#include "fernotron/fer_msg_plain.h"
#include "app/config/proj_app_cfg.h"
#include <stdbool.h>



extern void (*fpos_POSITIONS_SAVE_cb)(bool has_unsaved);

typedef uint8_t shutterGroupPositionsT[8];

// keep track of static positions
int statPos_setPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct);
int statPos_setPcts(GmBitMask *mm, uint8_t pct);
int statPos_getPct(uint32_t a, uint8_t g, uint8_t m);

int statPos_printAllPcts(const struct TargetDesc &td);

bool statPos_pctsByGroup_store(uint8_t g, shutterGroupPositionsT positions);
bool statPos_pctsByGroup_load(uint8_t g, const shutterGroupPositionsT positions);
void statPos_loopAutoSave();

void ferPos_loop(void);
void ferPos_init(void);









