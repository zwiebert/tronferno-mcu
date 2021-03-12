#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include "../mqtt_private.h"

#include <string.h>

static char *Line;

static void my_process_cmdline(char *line, const TargetDesc &td) {
  free(Line);
  Line = strdup(line);
}

void tst_receive_cmd_stop() {
  char topic[] = "tfmcu/42/cmd";
  char data[] = "stop";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"g\":4,\"m\":2,\"c\":\"stop\"}}", Line);
}

void tst_receive_cmd_stop_a() {
  char topic[] = "tfmcu/101234/cmd";
  char data[] = "stop";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"a\":\"101234\",\"c\":\"stop\"}}", Line);
}
void tst_receive_cmd_stop_agm() {
  char topic[] = "tfmcu/80abcd42/cmd";
  char data[] = "stop";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"a\":\"80abcd\",\"g\":4,\"m\":2,\"c\":\"stop\"}}", Line);
}

void tst_receive_pct_56() {
  char topic[] = "tfmcu/42/pct";
  char data[] = "56";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"g\":4,\"m\":2,\"p\":56}}", Line);
}
void tst_receive_pct_56_a() {
  char topic[] = "tfmcu/101234/pct";
  char data[] = "56";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"a\":\"101234\",\"p\":56}}", Line);
}
void tst_receive_pct_56_agm() {
  char topic[] = "tfmcu/80abcd42/pct";
  char data[] = "56";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"a\":\"80abcd\",\"g\":4,\"m\":2,\"p\":56}}", Line);
}

void tst_receive_GPO_21() {
  char topic[] = "tfmcu/gpo/21/level";
  char data[] = "1";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"mcu\":{\"gpio21\":1}}", Line);
}

void tst_receive_cli_xxx() {
  char topic[] = "tfmcu/cli";
  char data[] = "config all=?";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("config all=?", Line);
}

TEST_CASE("mqtt_receive", "[app_mqtt]")
{
  tst_receive_cli_xxx();

#if 0 // pct,cmd,gpio topics no longer use CLI
  tst_receive_GPO_21();
  tst_receive_pct_56();
  tst_receive_pct_56_a();
  tst_receive_pct_56_agm();

  tst_receive_cmd_stop();
  tst_receive_cmd_stop_a();
  tst_receive_cmd_stop_agm();
#endif
}


void setUp() {
  io_mqtt_topic_root = strdup("tfmcu/");
}
