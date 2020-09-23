/*
 * test_timer_state.c
 *
 *  Created on: 10.03.2020
 *      Author: bertw
 */
#include "unity.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "fernotron/auto/fau_tevent.h"
#include "fernotron/astro.h"
#include <time.h>
#include <fernotron/types.h>
#include "debug/dbg.h"
#include "app/settings/config.h"
#include <misc/int_types.h>

bool succ;

static int t2m(int hour, int minute) {
  return hour * 60 + minute;
}

static void test_timer_event4() {
  erase_timer_data(0,0);
  Fer_TimerData t1 = {15, 0, "03452123", "" };
  save_timer_data(&t1, 1, 0);
  Fer_TimerData t2 = {0, 0, "", "06542109+++0822-07082211+" };
  save_timer_data(&t2, 1, 1);

  struct tm now_tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 18,
     .tm_mday = 22,
     .tm_mon = 3, // april
     .tm_year = 120,
     .tm_wday = 3,
  };


  time_t now_time = mktime(&now_tm);
  timer_event_t tevt;
  fam_get_next_timer_event(&tevt, &now_time);

  TEST_ASSERT_EQUAL(t2m(20,54), tevt.next_event); // astro time
  GmBitMask test1;
  test1[1] = C.fer_usedMemberMask[1]&0x02;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(test1, *te_getMaskDown(&tevt), 8);

}


static void test_timer_event3() {
  erase_timer_data(0,0);
  Fer_TimerData t1 = {20000, 0, "03452123", "" };
  save_timer_data(&t1, 1, 0);
  Fer_TimerData t2 = {20000, 0, "", "06542109+++0822-07082211+" };
  save_timer_data(&t2, 1, 1);

  struct tm now_tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 5,
     .tm_mday = 2,
     .tm_mon = 3,
     .tm_year = 120,
     .tm_wday = 1,
  };


  time_t now_time = mktime(&now_tm);
  timer_event_t tevt;
  fam_get_next_timer_event(&tevt, &now_time);

  TEST_ASSERT_EQUAL(t2m(6,54), tevt.next_event);
  GmBitMask test1;
  test1[1] = C.fer_usedMemberMask[1]&0x02;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(test1, *te_getMaskUp(&tevt), 8);

}


static void test_timer_event2() {
  erase_timer_data(0,0);
  Fer_TimerData t1 = {20000, 0, "03452123", "" };
  save_timer_data(&t1, 1, 0);
  Fer_TimerData t2 = {20000, 0, "04562345", "" };
  save_timer_data(&t2, 1, 1);

  struct tm now_tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 19,
     .tm_mday = 2,
     .tm_mon = 3,
     .tm_year = 120,
     .tm_wday = 1,
  };


  time_t now_time = mktime(&now_tm);
  timer_event_t tevt;
  fam_get_next_timer_event(&tevt, &now_time);

  TEST_ASSERT_EQUAL(t2m(21,23), tevt.next_event);
  GmBitMask test1;
  test1[1] = C.fer_usedMemberMask[1]&0xfc;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(test1, *te_getMaskDown(&tevt), 8);

  now_tm = tm {
       .tm_sec = 0,
       .tm_min = 0,
       .tm_hour = 22,
       .tm_mday = 2,
       .tm_mon = 3,
       .tm_year = 120,
       .tm_wday = 1,
  };

  now_time = mktime(&now_tm);

  fam_get_next_timer_event(&tevt, &now_time);
  TEST_ASSERT_EQUAL(t2m(23,45), tevt.next_event);

}

struct tm test_tm, *test_tmp;


static void test_timer_event() {
  erase_timer_data(0,0);

  Fer_TimerData t1 = {20000, 0, "04562345", "" };
  save_timer_data(&t1, 1, 0);
  Fer_TimerData t2 = {20000, 0, "03452123", "" };
  save_timer_data(&t2, 1, 1);

  struct tm now_tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 19,
     .tm_mday = 2,
     .tm_mon = 3,
     .tm_year = 120,
     .tm_wday = 1,
  };

  minutes_t now_minutes = t2m(now_tm.tm_hour, now_tm.tm_min);

  timer_minutes_t timi;
  uint8_t g=1, m=2;
  succ = fau_get_timer_minutes_tm(&timi, &g, &m, true, &now_tm);
  TEST_ASSERT_TRUE(succ);
  TEST_ASSERT_EQUAL(1, g);
  TEST_ASSERT_EQUAL(0, m);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(t2m(4, 56), timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(23, 45), timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(23, 45), fau_get_earliest_from_timer_minutes(&timi, now_minutes));


   ///////////////////////////////////////////////////////////////////////////
  g=1;
  m=1;


  succ = fau_get_timer_minutes_tm(&timi, &g, &m, false, &now_tm);
  TEST_ASSERT_TRUE(succ);
  TEST_ASSERT_EQUAL(1, g);
  TEST_ASSERT_EQUAL(1, m);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(t2m(3, 45), timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21, 23), timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 23), fau_get_earliest_from_timer_minutes(&timi, now_minutes));


  time_t now_time = mktime(&now_tm);

  static timer_event_t tevt;
  fam_get_next_timer_event(&tevt, &now_time);

  TEST_ASSERT_EQUAL(t2m(21,23), tevt.next_event);

  now_tm = tm {
       .tm_sec = 0,
       .tm_min = 0,
       .tm_hour = 22,
       .tm_mday = 2,
       .tm_mon = 3,
       .tm_year = 120,
       .tm_wday = 1,
  };

  now_time = mktime(&now_tm);

  fam_get_next_timer_event(&tevt, &now_time);;
  TEST_ASSERT_EQUAL(t2m(23,45), tevt.next_event);
}


static void test_timer_minutes() {
  Fer_TimerData t1 = {20000,  0, "04562345", "" };
  Fer_TimerData t2 = {20000,  0, "", "06542109+++0822-07082211+" };

  struct tm tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 19,
     .tm_mday = 2,
     .tm_mon = 3,
     .tm_year = 120,
     .tm_wday = 1,
  };

  minutes_t now = t2m(tm.tm_hour, tm.tm_min);

  timer_minutes_t timi;


  succ = fau_get_timer_minutes_from_timer_data_tm(&timi, &t1, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(t2m(4, 56), timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(23, 45), timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(23, 45), fau_get_earliest_from_timer_minutes(&timi, now));


   ///////////////////////////////////////////////////////////////////////////

  tm.tm_wday = 1;

  succ = fau_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 9), fau_get_earliest_from_timer_minutes(&timi, now));

  tm.tm_wday = 2;

  succ = fau_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 9), fau_get_earliest_from_timer_minutes(&timi, now));

  tm.tm_wday = 3;

  succ = fau_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 9), fau_get_earliest_from_timer_minutes(&timi, now));

  tm.tm_wday = 4;

  succ = fau_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 9), fau_get_earliest_from_timer_minutes(&timi, now));

  tm.tm_wday = 5;

  succ = fau_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(8,22), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, fau_get_earliest_from_timer_minutes(&timi, now));


  tm.tm_wday = 6;

  succ = fau_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(7,8), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(22,11), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(22, 11), fau_get_earliest_from_timer_minutes(&timi, now));


  tm.tm_wday = 0;

  succ = fau_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(t2m(7,8), timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(22,11), timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(22, 11), fau_get_earliest_from_timer_minutes(&timi, now));


}


TEST_CASE("calculate timer minutes", "[fernotron/auto]") {

  test_timer_minutes();
}

TEST_CASE("timer next event", "[fernotron/auto]") {

  test_timer_event();
  test_timer_event2();
  test_timer_event3();
  test_timer_event4();
}

#ifdef TEST_HOST
#include "app/settings/config.h"
//GmBitMask manual_bits;
//config C;

static struct cfg_astro cfg_astro =
    { .astroCorrection = acAverage, .geo_longitude = 13.38, .geo_latitude = 52.5, .geo_timezone = 1,  };
void setUp() {

  C.fer_usedMembers = ~0U;
  C.fer_usedMemberMask.fromNibbleCounters(C.fer_usedMembers);
  astro_init_and_reinit(&cfg_astro);
}
#endif

#ifdef TEST_HOST
#ifndef cfg_getCuId
uint32_t cfg_getCuId() { return 0x801234; }
#endif
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

