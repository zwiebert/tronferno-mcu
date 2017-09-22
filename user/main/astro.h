/*
 * astro.h
 *
 * Created: 24.08.2017 22:03:12
 *  Author: bertw
 */ 


#ifndef ASTRO_H_
#define ASTRO_H_

#include "common.h"
#include "fer_code.h"
#include "fer.h"

void write_astro(uint8_t d[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset);


#endif /* ASTRO_H_ */