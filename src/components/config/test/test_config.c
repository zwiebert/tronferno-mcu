#include "unity.h"

#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "misc/int_types.h"
#include "misc/itoa.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
     strncpy(buf, val, random);
     buf[random] = '\0';
     strcpy(C.wifi.SSID, buf);
     save_config_item(item);
     strcpy (C.wifi.SSID, "------");
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
     strncpy(buf, val, random);
     buf[random] = '\0';
     strcpy(C.wifi.password, buf);
     save_config_item(item);
     strcpy (C.wifi.password, "------");
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

