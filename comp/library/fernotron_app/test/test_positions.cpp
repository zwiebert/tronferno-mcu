/*
 * test_positions.c
 *
 *  Created on: 16.03.2020
 *      Author: bertw
 */

#include "app/config/proj_app_cfg.h"
#include "fernotron/pos/shutter_pct.h"
#include "../pos/move.h"

#include "unity.h"



void   test_set_get_pct() {
  int pct;
  fer_statPos_setPct(0, 2, 3, 42);
  pct = fer_statPos_getPct(0, 2, 3);
  TEST_ASSERT_EQUAL(42,pct);
  pct = fer_simPos_getPct_afterDuration(2, 3, false, 0);
  TEST_ASSERT_EQUAL(42,pct);
  pct = fer_simPos_getPct_afterDuration(2, 3, false, 1000);
  TEST_ASSERT_EQUAL(0,pct);
  pct = fer_simPos_getPct_afterDuration(2, 3, true, 1000);
  TEST_ASSERT_EQUAL(100,pct);
}



TEST_CASE("set/get pct", "[fernotron/pos]") {
 test_set_get_pct();
}

#ifdef TEST_HOST
u32 cfg_getCuId() { return 0x801234; }
#include "time.h"

uint32_t run_time_ts() {
  clock_t c = clock();
  return c / (CLOCKS_PER_SEC/10);
}
uint32_t run_time_s() {
  clock_t c = clock();
  return c / CLOCKS_PER_SEC;
}

#endif
