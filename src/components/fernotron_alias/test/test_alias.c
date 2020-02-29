/*
 * test_alias.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "unity.h"
#include "fernotron_alias/pairings.h"
#include "txtio/inout.h"

#define A 0x101234
gm_bitmask_t gm, gme;
const gm_bitmask_t gmz;
bool succ;
#define SIZE_GM sizeof (gm_bitmask_t)
#define CLR_GM(gm) memset(gm, 0, sizeof (gm_bitmask_t))

static void g(unsigned a, uint8_t g, uint8_t m, bool unpair) {
  succ = pair_controller(a, g, m, unpair);
  TEST_ASSERT_TRUE(succ);
  succ = pair_getControllerPairings(a, &gm);
  TEST_ASSERT_TRUE(succ);

  gm_PutBit(gme, g, m, !unpair);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(gme, gm, SIZE_GM);
}

static void f() {
  succ = pair_rmController(A);
  succ = pair_getControllerPairings(A, &gm);
  TEST_ASSERT_FALSE(succ);

  CLR_GM(gme);

  g(A, 1, 2, false);
  g(A, 1, 3, false);
  g(A, 1, 3, true);
  g(A, 4, 5, false);
  g(A, 4, 6, false);
  g(A, 4, 5, true);
  g(A, 4, 6, true);

  succ = pair_rmController(A);
  TEST_ASSERT_TRUE(succ);
}



TEST_CASE("pair multiple members", "[fernotron_alias]")
{
     f();
}
