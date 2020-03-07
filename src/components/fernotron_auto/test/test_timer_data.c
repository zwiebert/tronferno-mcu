/*
 * test_timer_data.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "unity.h"
#include "fernotron_auto/timer_data.h"

timer_data_t td, tde = td_initializer;
bool succ;

static void g(uint8_t g, uint8_t m) {
  succ = save_timer_data(&tde, g, m);
  TEST_ASSERT_TRUE(succ);
  succ = read_timer_data(&td, &g, &m, false);
  TEST_ASSERT_TRUE(succ);
  TEST_ASSERT_EQUAL_MEMORY(&tde, &td, sizeof tde);
}

static void f() {
  g(1,2);
}

TEST_CASE("save and restore timer data", "[fernotron_auto]") {
  f();
}
