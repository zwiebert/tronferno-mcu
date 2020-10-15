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
#include "utils_misc/int_types.h"
#include "fernotron/gm_set.hh"


#pragma GCC push_options
#pragma GCC optimize ("O0")

static void test_gm_bitmask_iterator() {

  {
    Fer_GmSet gmm;
    gmm.setMember(0, 0);
    gmm.setMember(1, 4);
    gmm.setMember(2, 6);
    gmm.setMember(7, 7);

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
    Fer_GmSet gmm;
    gmm.setMember(1, 4);
    gmm.setMember(2, 6);

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
    Fer_GmSet gmm;
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
