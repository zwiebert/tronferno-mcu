#ifndef gpoutput_h_
#define gpoutput_h_

#include <stdint.h>

enum gpo_cmd { gpoc_DOWN, gpoc_UP, gpoc_SWITCH};

void gpo_configure(u8 gpo_nmb, u8 g, u8 m, u8 gpo_cmd);

void gpo_action(u8 g, u8 m, u8 cmd);


#endif
