#pragma once

#include <stdint.h>

enum gpi_cmd { gpic_DOWN, gpic_UP, gpic_STOP, gpic_RAIN};

void gpi_configure(uint8_t gpi_nmb, uint8_t g, uint8_t m, uint8_t gpi_cmd);

void gpi_loop(void);



