#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <app_misc/opt_map.hh>
#include <app_uout/so_types.h>

static void testOptMap1() {

  // check mapping string to enumeration
  TEST_ASSERT_EQUAL(otok::k_g, optMap_findToken("g"));
  TEST_ASSERT_EQUAL(otok::k_SEP, optMap_findToken("SEP"));
  TEST_ASSERT_EQUAL(otok::NONE, optMap_findToken("ABCDEFG"));
  TEST_ASSERT_EQUAL(otok::NONE, optMap_findToken(""));

  // check if settings related enumerations are in right order and positions
  TEST_ASSERT_EQUAL(otok::k_cu, SO_CFG_CU - SO_CFG_begin - 1);
  TEST_ASSERT_EQUAL(otok::k_gpio, SO_CFG_GPIO_MODES - SO_CFG_begin - 1);

}

TEST_CASE("","") {
  testOptMap1();
}

