/*
 * test_timer_state.c
 *
 *  Created on: 10.03.2020
 *      Author: bertw
 */

#include "unity.h"
#include "fernotron_auto/timer_data.h"
#include "fernotron_auto/timer_state.h"
#include "fernotron/astro.h"
#include "time.h"
#include "debug/debug.h"

bool succ;


static void setup_timer_data() {
  timer_data_t t1 = {.astro = 20000, .bf = 0, .daily = "04562345", .weekly = "" };
  save_timer_data(&t1, 1, 1);
  timer_data_t t2 = {.astro = 20000, .bf = 0, .daily = "", .weekly = "06542109+++0822-07082211+" };
  save_timer_data(&t2, 1, 2);

}

static int t2m(int hour, int minute) {
  return hour * 60 + minute;
}

static void test_timer_minutes() {
  setup_timer_data();

  struct tm tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 19,
     .tm_wday = 1,
     .tm_mday = 2,
     .tm_mon = 3,
  };

  minutes_t now = t2m(tm.tm_hour, tm.tm_min);

  timer_minutes_t timi;
  uint8_t g=1, m=1;

  succ = get_timer_minutes_tm(&timi, &g, &m, false, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(t2m(4, 56), timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(23, 45), timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(23, 45), timi_get_earliest(&timi, now));


   ///////////////////////////////////////////////////////////////////////////
  g=1;
  m=2;

  tm.tm_wday = 1;

  succ = get_timer_minutes_tm(&timi, &g, &m, false, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 9), timi_get_earliest(&timi, now));

  tm.tm_wday = 2;

  succ = get_timer_minutes_tm(&timi, &g, &m, false, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 9), timi_get_earliest(&timi, now));

  tm.tm_wday = 3;

  succ = get_timer_minutes_tm(&timi, &g, &m, false, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 9), timi_get_earliest(&timi, now));

  tm.tm_wday = 4;

  succ = get_timer_minutes_tm(&timi, &g, &m, false, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 9), timi_get_earliest(&timi, now));

  tm.tm_wday = 5;

  succ = get_timer_minutes_tm(&timi, &g, &m, false, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(8,22), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi_get_earliest(&timi, now));


  tm.tm_wday = 6;

  succ = get_timer_minutes_tm(&timi, &g, &m, false, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(7,8), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(22,11), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(22, 11), timi_get_earliest(&timi, now));


  tm.tm_wday = 0;

  succ = get_timer_minutes_tm(&timi, &g, &m, false, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(7,8), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(22,11), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(22, 11), timi_get_earliest(&timi, now));


}


TEST_CASE("calculate timer minutes", "[fernotron_auto]") {
  astro_init_and_reinit();
  test_timer_minutes();
}
