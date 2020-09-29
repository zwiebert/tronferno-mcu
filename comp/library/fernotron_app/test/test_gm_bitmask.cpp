/*
 * test_types.c
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */


#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <string.h>
#include "misc/int_types.h"
#include "fernotron/gm_bitmask.hh"


#pragma GCC push_options
#pragma GCC optimize ("O0")

static void test_gm_bitmask_iterator() {

  {
    Fer_GmBitMask gmm;
    gmm.setBit(0, 0);
    gmm.setBit(1, 4);
    gmm.setBit(2, 6);
    gmm.setBit(7, 7);

    auto it = gmm.begin();

    TEST_ASSERT_TRUE(it);
    TEST_ASSERT_EQUAL(0, it.getG());
    TEST_ASSERT_EQUAL(0, it.getM());


    TEST_ASSERT_TRUE(++it);
    TEST_ASSERT_EQUAL(1, it.getG());
    TEST_ASSERT_EQUAL(4, it.getM());

    TEST_ASSERT_TRUE(++it);
    TEST_ASSERT_EQUAL(2, it.getG());
    TEST_ASSERT_EQUAL(6, it.getM());

    TEST_ASSERT_TRUE(++it);
    TEST_ASSERT_EQUAL(7, it.getG());
    TEST_ASSERT_EQUAL(7, it.getM());


    TEST_ASSERT_FALSE(++it);
  }

  {
    Fer_GmBitMask gmm;
    gmm.setBit(1, 4);
    gmm.setBit(2, 6);

    auto it = gmm.begin();

    TEST_ASSERT_TRUE(it);
    TEST_ASSERT_EQUAL(1, it.getG());
    TEST_ASSERT_EQUAL(4, it.getM());

    TEST_ASSERT_TRUE(++it);
    TEST_ASSERT_EQUAL(2, it.getG());
    TEST_ASSERT_EQUAL(6, it.getM());

    TEST_ASSERT_FALSE(++it);
  }

  {
    Fer_GmBitMask gmm;
    for (auto it = gmm.begin(); it; ++it) {
      printf("g=%d, m=%d\n", it.getG(), it.getM());
      TEST_ASSERT_LESS_THAN(8, it.getG());
      TEST_ASSERT_LESS_THAN(8, it.getM());
    }
  }

}


TEST_CASE("gm_bitmask", "[fernotron]")
{
  test_gm_bitmask_iterator();
}
#pragma GCC pop_options
