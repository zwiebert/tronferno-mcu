#ifndef pairings_h_
#define pairings_h_
#include "fernotron/fer_code.h"
#include "positions/shutter_state.h"

enum pair_cmds { PC_none, PC_pair, PC_unpair, PC_read };

bool pair_controller(uint32_t controller, uint8_t g, uint8_t m, bool unpair);
bool read_pairings(gm_bitmask_t *gm, uint32_t controller);

// start listening on RF for controller to pair/unpair
bool pair_auto_set(uint8_t g, uint8_t m, uint8_t c, uint16_t id, unsigned timeout_secs);
// check for result (called from main loop)
bool pair_auto_set_check_timeout(void);
bool pair_auto_set_check(fer_sender_basic *fsb);
#endif
