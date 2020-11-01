#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include <config_kvs/config.h>

#include "utils_misc/int_types.h"
#include "utils_misc/itoa.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#if 0
static const char *make_msg(char *buf, int count)  {
    sprintf(buf, "loop_count: %d", count);
    return buf;
}

#define val "012345678901234567890"

static bool save_restore_item(enum configItem item, int count) {

  switch(item) {
  case CB_VERBOSE:
  {
     i8 random = abs(rand()) & 0x0f;
     C.app_verboseOutput = random;
     save_config_item(item);
     C.app_verboseOutput = 0;
     read_config_item(item);
     char buf[80];
     TEST_ASSERT_EQUAL_MESSAGE(random, C.app_verboseOutput, make_msg(buf, count));
   }
  break;

  case CB_WIFI_SSID:
  {
     int random = abs(rand()) % sizeof(val);
     char buf[80];
     STRLCPY(buf, val, random);
     buf[random] = '\0';
     STRCPY(C.wifi.SSID, buf);
     save_config_item(item);
     STRCPY (C.wifi.SSID, "------");
     read_config_item(item);
     char msg_buf[80];
     make_msg(msg_buf, count);
     TEST_ASSERT_EQUAL_STRING_MESSAGE(buf, C.wifi.SSID, msg_buf);
   }
  break;
  case CB_WIFI_PASSWD:
  {
     int random = abs(rand()) % sizeof(val);
     char buf[80];
     STRLCPY(buf, val, random);
     buf[random] = '\0';
     STRCPY(C.wifi.password, buf);
     save_config_item(item);
     STRCPY (C.wifi.password, "------");
     read_config_item(item);
     char msg_buf[80];
     make_msg(msg_buf, count);
     TEST_ASSERT_EQUAL_STRING_MESSAGE(buf, C.wifi.password, msg_buf);
   }
  break;
  default:
    return false;

  }
  return true;
}


#define LOOP_COUNT 500000
static void test_config_save_restore() {
  TEST_ASSERT_EQUAL(sizeof C.app_verboseOutput, sizeof (int) );



  for (int i=0; i < LOOP_COUNT; ++i) {
    while(true) {
    u8 random = abs(rand()) % CB_size;
    if (save_restore_item(random, i))
      break;
    }
  }

}



#ifdef TEST_HOST

config C;

TEST_CASE("test config save_restore", "[config]")
{
  test_config_save_restore();
}
#endif
#endif


constexpr AppSettings appSettings;

void test_config_keys2()
{
  TEST_ASSERT_EQUAL_STRING("C_TZ", app_settings.get_kvsKey(CB_TZ));
  TEST_ASSERT_EQUAL_STRING("C_VERBOSE", app_settings.get_kvsKey(CB_VERBOSE));
  TEST_ASSERT_EQUAL(CBT_u32, app_settings.get_kvsType(CB_CUID));

  u32 cu = config_read_item(CB_CUID, 0x80abcd);

}


TEST_CASE("test config keys", "[config]")
{
  test_config_keys2();

}
