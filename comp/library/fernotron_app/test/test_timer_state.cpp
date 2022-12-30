/*
 * test_timer_state.c
 *
 *  Created on: 10.03.2020
 *      Author: bertw
 */
#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif
#include "fernotron/auto/fau_tdata_store.h"
#include "fernotron/auto/fau_tevent.h"
#include "fernotron_trx/astro.h"
#include <time.h>
#include <fernotron/fer_pct.h>
#include "debug/dbg.h"
#include "fernotron/fer_main.h"
#include <utils_misc/int_types.h>
#include <fernotron_trx/fer_trx_api.hh>

bool succ;

static int t2m(int hour, int minute) {
  return hour * 60 + minute;
}

static void test_timer_event5() {
  fer_stor_timerData_erase(0, 0);
  Fer_TimerData t1;
  t1.putDaily("07001801");
  fer_stor_timerData_save(&t1, 1, 1);
  t1.putDaily("07001802");
  fer_stor_timerData_save(&t1, 1, 2);
  t1.putDaily("07001803");
  fer_stor_timerData_save(&t1, 1, 3);

  Fer_TimerEvent tevt;

  {
    struct tm now_tm = { .tm_sec = 0, .tm_min = 0, .tm_hour = 6, .tm_mday = 22, .tm_mon = 0, // January
        .tm_year = 120, .tm_wday = 3, };
    time_t now_time = mktime(&now_tm);
    tevt.fer_am_updateTimerEvent(now_time);

    TEST_ASSERT_FALSE(tevt.fer_am_loop(now_time));
  }

  {
    struct tm now_tm = { .tm_sec = 0, .tm_min = 0, .tm_hour = 7, .tm_mday = 22, .tm_mon = 0, // January
        .tm_year = 120, .tm_wday = 3, };
    time_t now_time = mktime(&now_tm);

    TEST_ASSERT_TRUE(tevt.fer_am_loop(now_time));
    tevt.fer_am_updateTimerEvent(now_time);
  }

  {
    struct tm now_tm = { .tm_sec = 0, .tm_min = 1, .tm_hour = 18, .tm_mday = 22, .tm_mon = 0, // January
        .tm_year = 120, .tm_wday = 3, };
    time_t now_time = mktime(&now_tm);

    TEST_ASSERT_TRUE(tevt.fer_am_loop(now_time));
    tevt.fer_am_updateTimerEvent(now_time);

  }

  {
    struct tm now_tm = { .tm_sec = 0, .tm_min = 2, .tm_hour = 18, .tm_mday = 22, .tm_mon = 0, // January
        .tm_year = 120, .tm_wday = 3, };
    time_t now_time = mktime(&now_tm);

    TEST_ASSERT_TRUE(tevt.fer_am_loop(now_time));
    tevt.fer_am_updateTimerEvent(now_time);

  }
  {
    struct tm now_tm = { .tm_sec = 0, .tm_min = 3, .tm_hour = 18, .tm_mday = 22, .tm_mon = 0, // January
        .tm_year = 120, .tm_wday = 3, };
    time_t now_time = mktime(&now_tm);

    TEST_ASSERT_TRUE(tevt.fer_am_loop(now_time));
    tevt.fer_am_updateTimerEvent(now_time);

  }
  {
    struct tm now_tm = { .tm_sec = 0, .tm_min = 4, .tm_hour = 18, .tm_mday = 22, .tm_mon = 0, // January
        .tm_year = 120, .tm_wday = 3, };
    time_t now_time = mktime(&now_tm);

    TEST_ASSERT_FALSE(tevt.fer_am_loop(now_time));
    tevt.fer_am_updateTimerEvent(now_time);

  }
}

static void test_timer_event4() {
  fer_stor_timerData_erase(0,0);
  Fer_TimerData t1;
  t1.putAstro(15); t1.putDaily("03452123");
  fer_stor_timerData_save(&t1, 1, 0);
  Fer_TimerData t2;
  t2.putAstro(0); t2.putWeekly("06542109+++0822-07082211+");
  fer_stor_timerData_save(&t2, 1, 1);

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
  Fer_TimerEvent tevt;
  tevt.fer_am_get_next_timer_event(&now_time);

  TEST_ASSERT_EQUAL(t2m(20,54), tevt.te_getEventMinute()); // astro time
  Fer_GmSet test1;
  test1[1] = fer_usedMemberMask[1]&0x02;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(test1, *tevt.te_getMaskDown(), 8);
}


static void test_timer_event3() {
  fer_stor_timerData_erase(0,0);
  Fer_TimerData t1;
  t1.putDaily("03452123");
  fer_stor_timerData_save(&t1, 1, 0);
  Fer_TimerData t2;
  t2.putWeekly("06542109+++0822-07082211+");
  fer_stor_timerData_save(&t2, 1, 1);

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
  Fer_TimerEvent tevt;
  tevt.fer_am_get_next_timer_event(&now_time);

  TEST_ASSERT_EQUAL(t2m(6,54), tevt.te_getEventMinute());
  Fer_GmSet test1;
  test1[1] = fer_usedMemberMask[1]&0x02;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(test1, *tevt.te_getMaskUp(), 8);

}


static void test_timer_event2() {
  fer_stor_timerData_erase(0,0);
  Fer_TimerData t1;
  t1.putDaily("03452123");
  fer_stor_timerData_save(&t1, 1, 0);
  Fer_TimerData t2;
  t2.putDaily("04562345");
  fer_stor_timerData_save(&t2, 1, 1);

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
  Fer_TimerEvent tevt;
  tevt.fer_am_get_next_timer_event(&now_time);

  TEST_ASSERT_EQUAL(t2m(21,23), tevt.te_getEventMinute());
  Fer_GmSet test1;
  test1[1] = fer_usedMemberMask[1]&0xfc;
  TEST_ASSERT_EQUAL_HEX8_ARRAY(test1, *tevt.te_getMaskDown(), 8);

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

  tevt.fer_am_get_next_timer_event(&now_time);
  TEST_ASSERT_EQUAL(t2m(23,45), tevt.te_getEventMinute());

}

struct tm test_tm, *test_tmp;


static void test_timer_event() {
  fer_stor_timerData_erase(0,0);

  Fer_TimerData t1;
  t1.putDaily("04562345");
  fer_stor_timerData_save(&t1, 1, 0);
  Fer_TimerData t2;
  t2.putDaily("03452123");
  fer_stor_timerData_save(&t2, 1, 1);

  struct tm now_tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 19,
     .tm_mday = 2,
     .tm_mon = 3,
     .tm_year = 120,
     .tm_wday = 1,
  };

  fer_au_minutesT now_minutes = t2m(now_tm.tm_hour, now_tm.tm_min);

  Fer_TimerMinutes timi;
  Fer_TimerData tida;
  uint8_t g=1, m=2;
  succ = fer_stor_timerData_load(&tida, &g, &m, true) && fer_au_get_timer_minutes_from_timer_data_tm(&timi, &tida, &now_tm);
  TEST_ASSERT_TRUE(succ);
  TEST_ASSERT_EQUAL(1, g);
  TEST_ASSERT_EQUAL(0, m);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(t2m(4, 56), timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(t2m(23, 45), timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(23, 45), fer_au_get_earliest_from_timer_minutes(&timi, now_minutes));


   ///////////////////////////////////////////////////////////////////////////
  g=1;
  m=1;

  succ = fer_stor_timerData_load(&tida, &g, &m, true) && fer_au_get_timer_minutes_from_timer_data_tm(&timi, &tida, &now_tm);
  TEST_ASSERT_TRUE(succ);
  TEST_ASSERT_EQUAL(1, g);
  TEST_ASSERT_EQUAL(1, m);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(t2m(3, 45), timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(t2m(21, 23), timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(21, 23), fer_au_get_earliest_from_timer_minutes(&timi, now_minutes));


  time_t now_time = mktime(&now_tm);

  static Fer_TimerEvent tevt;
  tevt.fer_am_get_next_timer_event(&now_time);

  TEST_ASSERT_EQUAL(t2m(21,23), tevt.te_getEventMinute());

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

  tevt.fer_am_get_next_timer_event(&now_time);;
  TEST_ASSERT_EQUAL(t2m(23,45), tevt.te_getEventMinute());
}


static void test_timer_minutes() {
  Fer_TimerData t1;
  t1.putDaily("04562345");
  Fer_TimerData t2;
  t2.putWeekly("06542109+++0822-07082211+");

  struct tm tm = {
     .tm_sec = 0,
     .tm_min = 0,
     .tm_hour = 19,
     .tm_mday = 2,
     .tm_mon = 3,
     .tm_year = 120,
     .tm_wday = 1,
  };

  fer_au_minutesT now = t2m(tm.tm_hour, tm.tm_min);

  Fer_TimerMinutes timi;


  succ = fer_au_get_timer_minutes_from_timer_data_tm(&timi, &t1, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(t2m(4, 56), timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(t2m(23, 45), timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(23, 45), fer_au_get_earliest_from_timer_minutes(&timi, now));


   ///////////////////////////////////////////////////////////////////////////

  tm.tm_wday = 1;

  succ = fer_au_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(21, 9), fer_au_get_earliest_from_timer_minutes(&timi, now));

  tm.tm_wday = 2;

  succ = fer_au_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(21, 9), fer_au_get_earliest_from_timer_minutes(&timi, now));

  tm.tm_wday = 3;

  succ = fer_au_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(21, 9), fer_au_get_earliest_from_timer_minutes(&timi, now));

  tm.tm_wday = 4;

  succ = fer_au_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(t2m(6,54), timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(t2m(21,9), timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(21, 9), fer_au_get_earliest_from_timer_minutes(&timi, now));

  tm.tm_wday = 5;

  succ = fer_au_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(t2m(8,22), timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, fer_au_get_earliest_from_timer_minutes(&timi, now));


  tm.tm_wday = 6;

  succ = fer_au_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(t2m(7,8), timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(t2m(22,11), timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(22, 11), fer_au_get_earliest_from_timer_minutes(&timi, now));


  tm.tm_wday = 0;

  succ = fer_au_get_timer_minutes_from_timer_data_tm(&timi, &t2, &tm);
  TEST_ASSERT_TRUE(succ);

  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_ASTRO]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_UP]);
  TEST_ASSERT_EQUAL(MINUTES_DISABLED, timi.minutes[FER_MINTS_DAILY_DOWN]);
  TEST_ASSERT_EQUAL(t2m(7,8), timi.minutes[FER_MINTS_WEEKLY_UP]);
  TEST_ASSERT_EQUAL(t2m(22,11), timi.minutes[FER_MINTS_WEEKLY_DOWN]);

  TEST_ASSERT_EQUAL(t2m(22, 11), fer_au_get_earliest_from_timer_minutes(&timi, now));


}


TEST_CASE("calculate timer minutes", "[fernotron/auto]") {

  test_timer_minutes();
}

TEST_CASE("timer next event", "[fernotron/auto]") {
  test_timer_event();
  test_timer_event2();
  test_timer_event3();
  test_timer_event4();
  test_timer_event5();
}

#ifdef TEST_HOST
#include "fernotron/fer_main.h"

static struct cfg_astro cfg_astro =
    { .astroCorrection = acAverage, .geo_longitude = 13.38, .geo_latitude = 52.5, .geo_timezone = 1,  };
void setUp() {
  setenv("TZ", "CET-1CEST-2,M3.5.0,M10.5.0", 1);
  fer_main_setup({0x801234, ~0U});
  Fer_Trx_API::setup_astro(&cfg_astro);
}
#endif

#ifdef TEST_HOST

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

