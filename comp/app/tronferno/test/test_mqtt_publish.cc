#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include "net_mqtt/mqtt.hh"
#include "app_mqtt/mqtt.h"
#include "../mqtt_private.h"
#include "net_mqtt/host/host_mqtt.h"
#include <string.h>


extern char *Topic, *Data;

void tst_publish_pinChange() {
  MyMqtt.publishPinChange(21, 1);
  TEST_ASSERT_EQUAL_STRING("tfmcu/gpi/21/level", Topic);
  TEST_ASSERT_EQUAL_STRING("1", Data);
}

void tst_publish_Pct() {
  MyMqtt.publish_gmp({4,2,56});
  TEST_ASSERT_EQUAL_STRING("tfmcu/42/pct_out", Topic);
  TEST_ASSERT_EQUAL_STRING("56", Data);
}


TEST_CASE("mqtt_publish", "[app_mqtt]")
{
  tst_publish_Pct();
  tst_publish_pinChange();
}


void setUp() {
  io_mqttApp_setup("tfmcu");
}
