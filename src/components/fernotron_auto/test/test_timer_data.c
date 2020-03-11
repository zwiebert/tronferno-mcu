/*
 * test_timer_data.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "unity.h"
#include "fernotron_auto/timer_data.h"
#include "fernotron_auto/timer_state.h"
#include "fernotron/astro.h"
#include "time.h"
#include "debug/debug.h"

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

static void setup_timer_data() {
  timer_data_t t1 = {.astro = 20000, .bf = 0, .daily = "04562345", .weekly = "" };
  save_timer_data(&t1, 1, 1);
  timer_data_t t2 = {.astro = 20000, .bf = 0, .daily = "", .weekly = "06542109+++0822-07082211+" };
  save_timer_data(&t2, 1, 2);

}

TEST_CASE("save and restore timer data", "[fernotron_auto]") {
  f();
}

