#ifndef pairings_h_
#define pairings_h_
#include "../shutter_positions/shutter_state.h"
#include "fernotron/fer_code.h"

enum pair_cmds { PC_none, PC_pair, PC_unpair, PC_read };

bool pair_controller(u32 controller, u8 g, u8 m, bool unpair);
bool read_pairings(gm_bitmask_t *gm, u32 controller);

// start listening on RF for controller to pair/unpair
bool pair_auto_set(u8 g, u8 m, u8 c, u16 id, unsigned timeout_secs);
// check for result (called from main loop)
bool pair_auto_set_check_timeout(void);
bool pair_auto_set_check(fer_sender_basic *fsb);
#endif
