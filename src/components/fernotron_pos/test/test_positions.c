/*
 * test_positions.c
 *
 *  Created on: 16.03.2020
 *      Author: bertw
 */

#include "app/proj_app_cfg.h"
#include "fernotron_pos/shutter_pct.h"
#include "move.h"

#include "unity.h"



void   test_set_get_pct() {
  int pct;

  ferPos_setPct(0, 2, 3, 42);
  pct = ferPos_getPct(0, 2, 3);
  TEST_ASSERT_EQUAL(42,pct);
  pct = ferPos_getPct_afterDuration(2, 3, false, 0);
  TEST_ASSERT_EQUAL(42,pct);
  pct = ferPos_getPct_afterDuration(2, 3, false, 1000);
  TEST_ASSERT_EQUAL(0,pct);
  pct = ferPos_getPct_afterDuration(2, 3, true, 1000);
  TEST_ASSERT_EQUAL(100,pct);
}



TEST_CASE("set/get pct", "[fernotron_pos]") {
 test_set_get_pct();
}
