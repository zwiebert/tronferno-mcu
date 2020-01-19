#ifndef gpinput_h_
#define gpinput_h_

#include <stdint.h>

enum gpi_cmd { gpic_DOWN, gpic_UP, gpic_STOP, gpic_RAIN};

void gpi_configure(u8 gpi_nmb, u8 g, u8 m, u8 gpi_cmd);

void gpi_loop(void);


#endif

