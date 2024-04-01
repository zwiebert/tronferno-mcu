#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif


#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include <config_kvs/config.h>

#include "utils_misc/int_types.h"
#include "utils_misc/itoa.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//#define DEFINE_COMP_SETTINGS_OBJECT
#include <config_kvs/register_settings.hh>


void test_config_keys2() {
  TEST_ASSERT_EQUAL_STRING("C_TZ", comp_sett.get_kvsKey(CB_TZ));
  TEST_ASSERT_EQUAL_STRING("C_VERBOSE", comp_sett.get_kvsKey(CB_VERBOSE));
  TEST_ASSERT_EQUAL(CBT_u32, comp_sett.get_kvsType(CB_CUID));

  uint32_t cu = config_read_item(CB_CUID, 0x80abcd);

}

TEST_CASE("test config keys", "[config]")
{
  test_config_keys2();
}
