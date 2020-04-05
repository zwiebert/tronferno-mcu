/*
 * positions_static.h
 *
 *  Created on: 05.04.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_POS_INCLUDE_FERNOTRON_POS_POSITIONS_STATIC_H_
#define COMPONENTS_FERNOTRON_POS_INCLUDE_FERNOTRON_POS_POSITIONS_STATIC_H_

#include "fernotron/types.h"
#include "fernotron/fer_msg_plain.h"
#include "app_config/callbacks.h"


typedef uint8_t shutterGroupPositionsT[8];

// keep track of static positions
int statPos_setPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct);
int statPos_setPcts(gm_bitmask_t *mm, uint8_t pct);
int statPos_getPct(uint32_t a, uint8_t g, uint8_t m);

int statPos_printAllPcts(void);

int statPos_pctsByGroup_store(uint8_t g, shutterGroupPositionsT positions);
int statPos_pctsByGroup_load(uint8_t g, const shutterGroupPositionsT positions);
void statPos_loopAutoSave();

void ferPos_loop(void);
void ferPos_init(void);


// load/store





#ifndef fpos_POSITIONS_UNSAVED_cb
#define  fpos_POSITIONS_UNSAVED_cb()
#endif
#ifndef fpos_POSTIONS_SAVED_cb
#define  fpos_POSTIONS_SAVED_cb()
#endif

#endif /* COMPONENTS_FERNOTRON_POS_INCLUDE_FERNOTRON_POS_POSITIONS_STATIC_H_ */
