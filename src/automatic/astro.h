/*
 * astro.h
 *
 * Created: 24.08.2017 22:03:12
 *  Author: bertw
 */ 


#ifndef ASTRO_H_
#define ASTRO_H_

#include "main/common.h"
#include "fernotron/fer_code.h"
#include "fernotron/fer.h"




// calculate and fill in astro data
void astro_write_data(astro_byte_data d, int mint_offset);

// calculate astro time like the motor does
u16  astro_calc_minutes(const struct tm *tm);


#endif /* ASTRO_H_ */
