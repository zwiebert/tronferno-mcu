/*
 * test_positions.c
 *
 *  Created on: 16.03.2020
 *      Author: bertw
 */


#include "fernotron/pos/shutter_pct.h"
#include "../pos/move.hh"
#include "fernotron/fer_main.h"
#include <utils_time/run_time.h>

#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

#include <utils_time/ut_sleep.h>
#define sleep(s) ut_sleep(s)
#endif



void   test_set_get_pct() {
  uint8_t pct;
  fer_statPos_setPct(0, 2, 3, 42);
  pct = fer_statPos_getPct(2, 3);
  TEST_ASSERT_EQUAL(42,pct);
  pct = fer_simPos_getPct_afterDuration(2, 3, false, 0);
  TEST_ASSERT_EQUAL(42,pct);
  pct = fer_simPos_getPct_afterDuration(2, 3, false, 1000);
  TEST_ASSERT_EQUAL(0,pct);
  pct = fer_simPos_getPct_afterDuration(2, 3, true, 1000);
  TEST_ASSERT_EQUAL(100,pct);
}


void tst_dynamicPos() {
  uint8_t pct;

  uint8_t pcts[] { 50, 46, 42, 38, 34, 30, 26, 22, 18, 14, 10 };

  uint32_t a = fer_config.cu;
  fer_statPos_setPct(a, 2, 3, 50);
  fer_simPos_registerMovingShutter(a, 2, 3, fer_if_cmd_DOWN);

  for (int i = 0; i < 15; ++i) {
    sleep(1);

    pct = fer_simPos_getPct_whileMoving(2, 3);
    std::cout << '\r' << i << "pct: " << (int) pct << " run_time_ts: " << run_time_ts() << std::flush;
    fer_pos_loop();

    if (IS_IN_RANGE(1, i, 10)) {
      //TEST_ASSERT_TRUE(Is_Moving);
    }

    if (IS_IN_RANGE(5, i, 9)) {
      TEST_ASSERT_NOT_EQUAL(50, pct);
    }

    if (IS_IN_RANGE(1, i, 9)) {
      TEST_ASSERT_EQUAL(pcts[i + 1], pct);
    }

    if (IS_IN_RANGE(14, i, 14)) {
      //TEST_ASSERT_FALSE(Is_Moving);
    }

  }

  // TEST_ASSERT_TRUE(false);
}


void tst_dynamicPos_stop() {
  uint8_t pct;

  uint8_t pcts[] { 50, 46, 42, 38, 34, 30, 26, 22, 18, 14, 10 };

  uint32_t a = fer_config.cu;
  fer_statPos_setPct(a, 2, 3, 50);
  fer_simPos_registerMovingShutter(a, 2, 3, fer_if_cmd_DOWN);

  for (int i = 0; i < 15; ++i) {
    sleep(1);

    pct = fer_simPos_getPct_whileMoving(2, 3);
    std::cout << '\r' << i << "pct: " << (int) pct << " run_time_ts: " << run_time_ts() << std::flush;

    fer_pos_loop();

        if (i == 7) {
          fer_simPos_registerMovingShutter(a, 2, 3, fer_if_cmd_STOP);
          fer_pos_loop();
        }



    if (IS_IN_RANGE(1, i, 6)) {
      //TEST_ASSERT_TRUE(Is_Moving);
      TEST_ASSERT_EQUAL(pcts[i + 1], pct);
    }

    if (IS_IN_RANGE(5, i, 9)) {
      TEST_ASSERT_NOT_EQUAL(50, pct);
    }

    if (IS_IN_RANGE(9, i, 9)) {
     //TEST_ASSERT_FALSE(Is_Moving);
     TEST_ASSERT_EQUAL(pcts[7+1], pct);
    }
  }

  // TEST_ASSERT_TRUE(false);
}


TEST_CASE("dynamic_positions_stop", "[fernotron/pos]") {
 tst_dynamicPos_stop();
}


TEST_CASE("set/get pct", "[fernotron/pos]") {
 test_set_get_pct();
}


TEST_CASE("dynamic_positions", "[fernotron/pos]") {
 tst_dynamicPos();
}



#ifdef TEST_HOST

void setUp() {
  fer_main_setup({0x801234, ~0U});
}

#endif
