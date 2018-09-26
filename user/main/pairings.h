#ifndef pairings_h_
#define pairings_h_
#include "fer_code.h"
#include "shutter_state.h"

bool pair_controller(uint32_t controller, uint8_t g, uint8_t m, bool unpair);
bool read_pairings(gm_bitmask_t *gm, uint32_t controller);


#endif
