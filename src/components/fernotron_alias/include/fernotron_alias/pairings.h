#ifndef pairings_h_
#define pairings_h_

#include "fernotron/fsb.h"
#include "fernotron/types.h"

#include <stdlib.h>
#include "stdbool.h"

enum pair_cmds { PC_none, PC_pair, PC_unpair, PC_read };

bool pair_controller(uint32_t controller, uint8_t g, uint8_t m, bool unpair);



bool pair_setControllerPairings(uint32_t controller, gm_bitmask_t *gm);
bool pair_getControllerPairings(uint32_t controller, gm_bitmask_t *gm);
bool pair_rmController(uint32_t controller);

bool pair_so_output_all_pairings(void);

// start listening on RF for controller to pair/unpair
bool pair_auto_set(uint8_t g, uint8_t m, uint8_t c, uint16_t id, unsigned timeout_secs);
// check for result (called from main loop)
void pair_auto_set_check_timeout(void);
bool pair_auto_set_check(const fsbT *fsb);
#endif
