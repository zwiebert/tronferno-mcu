/*
 * debug.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

#include "debug/debug.h"

bool test_modules(void)
{
  return ((!TEST_MODULE_ASTRO || testModule_astro())
  && (!TEST_MODULE_RTC || testModule_rtc())
  && (!TEST_MODULE_FER_PRG || testModule_fer_prg())
   // && (!TEST_MODULE_CONFIG || testModule_config())
   && (!TEST_MODULE_CLI || testModule_cli())

  );
}
