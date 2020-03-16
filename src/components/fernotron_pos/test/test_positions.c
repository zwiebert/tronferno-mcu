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

  ferPos_mSetPct(0, 2, 3, 42);
  pct = ferPos_mGetPct(0, 2, 3);
  TEST_ASSERT_EQUAL(42,pct);
}



TEST_CASE("set/get pct", "[fernotron_pos]") {
 test_set_get_pct();
}
