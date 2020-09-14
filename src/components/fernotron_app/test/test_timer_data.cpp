/*
 * test_timer_data.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "unity.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "fernotron/auto/fau_tminutes.h"
#include "fernotron/astro.h"
#include "time.h"
#include "debug/dbg.h"

static timer_data_t td, tde = td_initializer;

static int save_and_restore_gm(uint8_t g, uint8_t m) {
  if (!save_timer_data(&tde, g, m))
    return -1;

  if (!read_timer_data(&td, &g, &m, false))
    return -2;

  return 0;
}

static void test_save_and_restore() {

  TEST_ASSERT_EQUAL(0, save_and_restore_gm(1,2));
  TEST_ASSERT_EQUAL_MEMORY(&tde, &td, sizeof tde);

  TEST_ASSERT_EQUAL(0, save_and_restore_gm(7,4));
  TEST_ASSERT_EQUAL_MEMORY(&tde, &td, sizeof tde);

  TEST_ASSERT_EQUAL(0, save_and_restore_gm(6,0));
  TEST_ASSERT_EQUAL_MEMORY(&tde, &td, sizeof tde);

  u8 g = 6, m = 3;
  TEST_ASSERT_TRUE(read_timer_data(&td, &g, &m, true));
  TEST_ASSERT_EQUAL(6,g);
  TEST_ASSERT_EQUAL(0,m);
  TEST_ASSERT_EQUAL_MEMORY(&tde, &td, sizeof tde);

}

TEST_CASE("save and restore timer data", "[fernotron/auto]") {
  test_save_and_restore();
}

#ifdef TEST_HOST
#include "app/settings/config.h"
config C;
#endif

