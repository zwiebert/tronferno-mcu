#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include "net_mqtt/mqtt.hh"
#include "../mqtt_private.h"
#include "net_mqtt/host/host_mqtt.h"
#include <string.h>

char *Topic;
char *Data;


static void hostTest_mqtt_publish(const char *topic, const char *data) {
  free(Topic), Topic = strdup(topic);
  free(Data), Data = strdup(data);
}


void tst_publish_pinChange() {
  io_mqttApp_publishPinChange(21, 1);
  TEST_ASSERT_EQUAL_STRING("tfmcu/gpi/21/level", Topic);
  TEST_ASSERT_EQUAL_STRING("1", Data);
}

void tst_publish_Pct() {
  io_mqtt_publish_gmp({4,2,56});
  TEST_ASSERT_EQUAL_STRING("tfmcu/42/pct_out", Topic);
  TEST_ASSERT_EQUAL_STRING("56", Data);
}


TEST_CASE("mqtt_publish", "[app_mqtt]")
{
  tst_publish_Pct();
  tst_publish_pinChange();
}


void setUp() {
  io_mqtt_publish_cb = hostTest_mqtt_publish;
  io_mqtt_topic_root = strdup("tfmcu/");
}
