/*
 * test_rtc.c
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#include "unity.h"
#include "stdbool.h"

#include "app/rtc.h"


void test_rtc()
{
  char s[20]; bool result;
  result = rtc_set_by_string("2017-09-07T13:22:33");
  TEST_ASSERT_TRUE(result);
  result = rtc_get_by_string(s);
  TEST_ASSERT_TRUE(result);
}


TEST_CASE("test rtc", "[app]")
{
     test_rtc();
}
