#include <unity.h>

#include <app_misc/opt_map.hh>
#include <app_uout/so_types.h>

static void testOptMap1() {

  // check mapping string to enumeration
  TEST_ASSERT_EQUAL(otok::k_g, optMap_findToken("g"));
  TEST_ASSERT_EQUAL(otok::k_SEP, optMap_findToken("SEP"));
  TEST_ASSERT_EQUAL(otok::NONE, optMap_findToken("ABCDEFG"));
  TEST_ASSERT_EQUAL(otok::NONE, optMap_findToken(""));

}


int main() {
  UNITY_BEGIN();

  RUN_TEST(testOptMap1);

  return UNITY_END();
}
