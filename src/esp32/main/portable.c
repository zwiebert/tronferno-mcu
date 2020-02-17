/*
 * portable.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

#include <stdio.h>
#include <string.h>

#include "userio/inout.h"

char *
itoa(int val, char *s, int radix) {
	if (s)
		sprintf(s, (radix == 10 ? "%i" : "%x"), val);
	return s;
}

char *
ltoa(long val, char *s, int radix) {
	if (s)
		sprintf(s, (radix == 10 ? "%li" : "%lx"), val);
	return s;

}
