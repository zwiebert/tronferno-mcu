/*
 * test_types.c
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */


#include "unity.h"

#include <string.h>
#include "../fer_app_cfg.h"
#include "debug/debug.h"
#include "fernotron/types.h"

gm_bitmask_t Gma, Gmb, Gmc = {1,2,4,8,16,32,64,128}, *gma=&Gma, *gmb=&Gmb, *gmc=&Gmc;

static void test_gm() {
  int i, state;
  u8 g, m;

  gm_Clear(gma);
  TEST_ASSERT_TRUE(gm_isAllClear(gma));

  gm_SetBit(gma, 4, 2);
  TEST_ASSERT_FALSE(gm_isAllClear(gma));
  TEST_ASSERT_TRUE(gm_GetBit(gma, 4, 2));
  TEST_ASSERT_FALSE(gm_GetBit(gma, 4, 3));
  TEST_ASSERT_EQUAL((1 << 2), gm_GetByte(gma, 4));

  gm_Clear(gma);
  TEST_ASSERT_TRUE(gm_isAllClear(gma));

  gm_PutBit(gma, 5, 3, true);
  TEST_ASSERT_FALSE(gm_isAllClear(gma));
  TEST_ASSERT_EQUAL((1 << 3), gm_GetByte(gma, 5));

  gm_PutBit(gma, 5, 3, false);
  TEST_ASSERT_TRUE(gm_isAllClear(gma));
  gm_PutBit(gma, 5, 4, true);
  TEST_ASSERT_FALSE(gm_isAllClear(gma));
  gm_ClrBit(gma, 5, 4);
  TEST_ASSERT_TRUE(gm_isAllClear(gma));

  for (i = 0; i < 8; ++i) {
    TEST_ASSERT_EQUAL((1 << i), gm_GetByte(gmc,i));
  }

  for ((g = 0), (m = ~0), (state = 0); gm_getNext(gmc, &g, &m); ++state) {
    TEST_ASSERT_EQUAL(state, g);
    TEST_ASSERT_EQUAL(state, m);
  }
  TEST_ASSERT_EQUAL(8, state);

  gm_Clear(gma);
  for ((g = 0), (m = ~0), (state = 0); gm_getNext(gma, &g, &m); ++state) {
    TEST_ASSERT_FALSE(true);
  }

}


TEST_CASE("gm", "[fernotron]")
{
  test_gm();
}
