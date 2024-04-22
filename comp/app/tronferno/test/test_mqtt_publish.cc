#include <unity.h>

#include "net_mqtt_client/mqtt.hh"
#include "app_mqtt/mqtt.h"
#include "../mqtt_private.h"
#include "net_mqtt_client/host/host_mqtt.h"
#include <string.h>


extern char *Topic, *Data;

void tst_publish_pinChange() {
  MyMqtt.publishPinChange(21, 1);
  TEST_ASSERT_EQUAL_STRING("tfmcu/gpi/21/level", Topic);
  TEST_ASSERT_EQUAL_STRING("1", Data);
}

void tst_publish_Pct() {
  MyMqtt.publish_gmp({4,2,56});
  TEST_ASSERT_EQUAL_STRING("tfmcu/42/ipct_out", Topic);
  TEST_ASSERT_EQUAL_STRING("44", Data); // 100-56
}


int main() {
  UNITY_BEGIN();

  RUN_TEST(tst_publish_Pct);
  RUN_TEST(tst_publish_pinChange);

  return UNITY_END();
}

void setUp() {
  cfg_mqtt c;
  io_mqttApp_setup(&c);
}
