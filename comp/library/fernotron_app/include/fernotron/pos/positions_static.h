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



extern void (*fer_pos_POSITIONS_SAVE_cb)(bool has_unsaved);

typedef uint8_t fer_shutterGroupPositionsT[8];

// keep track of static positions
int fer_statPos_setPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct);
int fer_statPos_setPcts(Fer_GmBitMask *mm, uint8_t pct);
int fer_statPos_getPct(uint32_t a, uint8_t g, uint8_t m);

int fer_statPos_printAllPcts(const struct TargetDesc &td);

bool fer_statPos_pctsByGroup_store(uint8_t g, fer_shutterGroupPositionsT positions);
bool fer_statPos_pctsByGroup_load(uint8_t g, const fer_shutterGroupPositionsT positions);
void fer_statPos_loopAutoSave();

void fer_pos_loop(void);
void fer_pos_init(void);









