/*
 * test_astro.c
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#include "unity.h"

#include <fernotron/fer_msg_basic.h>
#include "astro.h"
#include "misc/sun.h"
#include <string.h>
#include "../fer_app_cfg.h"
#include "debug/debug.h"

#include "../fer.h"
#include "misc/bcd.h"

static u8 data[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT];

void
test_astro()  // FIXME: this does not really tests anything
{
  double rise, set;
  u16 doy = 172;
  calc_sunrise_sunset(&rise, &set, C.geo_timezone, doy, C.geo_longitude, C.geo_latitude, CIVIL_TWILIGHT_RAD);

  astro_write_data(data, 0);
  ets_printf("test calc_minutes\n");
  int ct=0;
  struct tm tm;
  for(tm.tm_mon=0; tm.tm_mon < 12; ++tm.tm_mon)
    for (tm.tm_mday=1; tm.tm_mday <= 30; ++tm.tm_mday) {
      u16 min = astro_calc_minutes(&tm);
      ets_printf("%02d:%02d, %s", min / 60, min %60, (ct++ % 16? "": "\n"));
    }
  ets_printf("\n------------------------\n");
  ct = 0; int ct2 = 0;
  for(tm.tm_mon=0; tm.tm_mon < 12; ++tm.tm_mon)
    for (tm.tm_mday=1; tm.tm_mday <= 30; ++tm.tm_mday) {
      u16 min = astro_calc_minutes(&tm);
      if (!(ct2++ % 4))
        ets_printf("%02d:%02d, %s", min / 60, min %60, (++ct % 4? "": "\n"));
    }

  TEST_ASSERT_NOT_EQUAL(rise, set); // XXX
}


TEST_CASE("astro", "[app]")
{
     astro_init_and_reinit();
     test_astro();
}

