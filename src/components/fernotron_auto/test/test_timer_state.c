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
#include <fernotron/types.h>
#include "debug/debug.h"

bool succ;


static int t2m(int hour, int minute) {
  return hour * 60 + minute;
}

static void test_timer_event3() {
  erase_timer_data(0,0);
  timer_data_t t1 = {.astro = 20000, .bf = 0, .daily = "03452123", .weekly = "" };
  save_timer_data(&t1, 1, 0);
  timer_data_t t2 = {.astro = 20000, .bf = 0, .daily = "", .weekly = "06542109+++0822-07082211+" };
  save_timer_data(&t2, 1, 1);

  struct tm now_tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 5,
     .tm_wday = 1,
     .tm_mday = 2,
     .tm_mon = 3,
  };


  time_t now_time = timegm(&now_tm);
  timer_event_t teu, ted;
  get_next_timer_event(&teu, &ted, &now_time);

  TEST_ASSERT_EQUAL(t2m(6,54), teu.next_event);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, ted.next_event);
  gm_bitmask_t test1 = {0,0x02,0,0,0,0,0,0};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(test1, teu.matching_members, 8);

}


static void test_timer_event2() {
  erase_timer_data(0,0);
  timer_data_t t1 = {.astro = 20000, .bf = 0, .daily = "03452123", .weekly = "" };
  save_timer_data(&t1, 1, 0);
  timer_data_t t2 = {.astro = 20000, .bf = 0, .daily = "04562345", .weekly = "" };
  save_timer_data(&t2, 1, 1);

  struct tm now_tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 19,
     .tm_wday = 1,
     .tm_mday = 2,
     .tm_mon = 3,
  };


  time_t now_time = timegm(&now_tm);
  timer_event_t teu, ted;
  get_next_timer_event(&teu, &ted, &now_time);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, teu.next_event);
  TEST_ASSERT_EQUAL(t2m(21,23), ted.next_event);
  gm_bitmask_t test1 = {0,0xfc,0,0,0,0,0,0};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(test1, ted.matching_members, 8);

  now_tm = (struct tm){
       .tm_sec = 0,
       .tm_min = 0,
       .tm_hour = 22,
       .tm_wday = 1,
       .tm_mday = 2,
       .tm_mon = 3,
    };

  now_time = timegm(&now_tm);

  get_next_timer_event(&teu, &ted, &now_time);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, teu.next_event);
  TEST_ASSERT_EQUAL(t2m(23,45), ted.next_event);

}



static void test_timer_event() {
  erase_timer_data(0,0);
  timer_data_t t1 = {.astro = 20000, .bf = 0, .daily = "04562345", .weekly = "" };
  save_timer_data(&t1, 1, 0);
  timer_data_t t2 = {.astro = 20000, .bf = 0, .daily = "03452123", .weekly = "" };
  save_timer_data(&t2, 1, 1);

  struct tm now_tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 19,
     .tm_wday = 1,
     .tm_mday = 2,
     .tm_mon = 3,
  };

  minutes_t now_minutes = t2m(now_tm.tm_hour, now_tm.tm_min);

  timer_minutes_t timi;
  uint8_t g=1, m=2;

  succ = get_timer_minutes_tm(&timi, &g, &m, true, &now_tm);
  TEST_ASSERT_TRUE(succ);
  TEST_ASSERT_EQUAL(1, g);
  TEST_ASSERT_EQUAL(0, m);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(t2m(4, 56), timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(23, 45), timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(23, 45), timi_get_earliest(&timi, now_minutes));


   ///////////////////////////////////////////////////////////////////////////
  g=1;
  m=1;


  succ = get_timer_minutes_tm(&timi, &g, &m, false, &now_tm);
  TEST_ASSERT_TRUE(succ);
  TEST_ASSERT_EQUAL(1, g);
  TEST_ASSERT_EQUAL(1, m);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[ASTRO_MINTS]);
  TEST_ASSERT_EQUAL(t2m(3, 45), timi.minutes[DAILY_UP_MINTS]);
  TEST_ASSERT_EQUAL(t2m(21, 23), timi.minutes[DAILY_DOWN_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_UP_MINTS]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[WEEKLY_DOWN_MINTS]);

  TEST_ASSERT_EQUAL(t2m(21, 23), timi_get_earliest(&timi, now_minutes));


  time_t now_time = timegm(&now_tm);

  timer_event_t teu, ted;
  get_next_timer_event(&teu, &ted, &now_time);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, teu.next_event);
  TEST_ASSERT_EQUAL(t2m(21,23), ted.next_event);

  now_tm = (struct tm){
       .tm_sec = 0,
       .tm_min = 0,
       .tm_hour = 22,
       .tm_wday = 1,
       .tm_mday = 2,
       .tm_mon = 3,
    };

  now_time = timegm(&now_tm);

  get_next_timer_event(&teu, &ted, &now_time);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, teu.next_event);
  TEST_ASSERT_EQUAL(t2m(23,45), ted.next_event);




}


static void test_timer_minutes() {
  erase_timer_data(0,0);
  timer_data_t t1 = {.astro = 20000, .bf = 0, .daily = "04562345", .weekly = "" };
  save_timer_data(&t1, 1, 1);
  timer_data_t t2 = {.astro = 20000, .bf = 0, .daily = "", .weekly = "06542109+++0822-07082211+" };
  save_timer_data(&t2, 1, 2);

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

  test_timer_minutes();
}

TEST_CASE("timer next event", "[fernotron_auto]") {

  test_timer_event();
  test_timer_event2();
  test_timer_event3();
}

#ifdef TEST_HOST
#include "config/config.h"
gm_bitmask_t manual_bits;
config C;

static struct cfg_astro cfg_astro =
    { .geo_longitude = 13, .geo_latitude = 52, .geo_timezone = 1, .astroCorrection = acAverage, };
void setUp() {

  C.fer_usedMembers = ~0U;
  gm_fromNibbleCounters(&C.fer_usedMemberMask, C.fer_usedMembers);
  astro_init_and_reinit(&cfg_astro);
}
#endif
