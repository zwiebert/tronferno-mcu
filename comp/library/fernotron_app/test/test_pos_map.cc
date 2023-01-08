/*
 * \brief fernotron_app/test/test_pos_map.cpp
 *
 */

#include "../pos/pos_map.hh"

#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

#include <utils_time/ut_sleep.h>
#define sleep(s) ut_sleep(s)
#endif


void tst_posMap_1() {
  Fer_Pos_Map pm;
  uint8_t g = 0, m = 0;

  g = 1;
  for(uint8_t m=1; m <= 7; ++m ) {
    pm.setPct(g, m, 10 * g + m);
  }
  fer_usedMemberMask.setGroup(1, 0xFE);
 // pm.updateAvgPcts(true);
  TEST_ASSERT_EQUAL((11+12+13+14+15+16+17)/7, pm.getPct(g, 0));

  int count = 0;
  auto all = pm.getValidPcts();
  for(auto ia = all.begin(); ia; ++ia) {
    g = ia.getG();
    m = ia.getM();

    ++count;
  }

  TEST_ASSERT_EQUAL(8, count);

  count = 0;
  Fer_GmSet each_m;
  pm.fer_statPos_forEachPct([&count,&each_m](const Fer_GmSet &same_pct, uint8_t pct) {
    ++count;
    each_m |= same_pct;
  });
  TEST_ASSERT_EQUAL(7, count);

  count = 0;
  for(auto ia = each_m.begin(); ia; ++ia) {
    g = ia.getG();
    m = ia.getM();

    ++count;
  }
  TEST_ASSERT_EQUAL(8, count);
}



TEST_CASE("pos_map_1", "[fernotron/pos]") {
  tst_posMap_1();
}
