/*
 * test_astro.c
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <fernotron_trx/raw/fer_msg_plain.h>
#include "fernotron_trx/astro.h"
#include "utils_misc/sun.h"
#include <string.h>
#include "../fer_app_cfg.h"
#include "debug/dbg.h"
#include "fernotron_trx/raw/fer_rawmsg_build.h"
#include "utils_misc/bcd.h"
#include "app_settings/config.h"
#include <fernotron_trx/fer_trx_api.hh>


int astroTableIndex_from_tm_OLD(const struct tm *tm);

#if 0
static void
test_astro()  // FIXME: this does not really tests anything
{
  struct tm tm = {};
  for (int m = 0; m < 12; ++m) {
    for (int d = 1; d <= 30; ++d) {
      if (m == 1 && d == 29)
        break;
      tm.tm_mon = m;
      tm.tm_mday = d;

      int i1 = astroTableIndex_from_tm_OLD(&tm);
      int i2 = astroTableIndex_from_tm(&tm);
      if (i1 != i2) {
        TEST_ASSERT_EQUAL(i1, i2);
      }
    }
  }
}
#endif

static int  test_get_index(int mon, int mday) {
  struct tm tm = {.tm_mday = mday,  .tm_mon = mon - 1};
  return astroTableIndex_from_tm(&tm);

}

static void test_indexes() {
  TEST_ASSERT_EQUAL(26, test_get_index(4, 5));
  TEST_ASSERT_EQUAL(26, test_get_index(4, 6));
  TEST_ASSERT_EQUAL(29, test_get_index(4, 17));
}

TEST_CASE("astro", "[app]")
{
  struct cfg_astro  cfg = { acAverage, 13.38, 52.52, +1.0 };
  fer_astro_init_and_reinit(&cfg);
  //test_astro();
  test_indexes();
}

