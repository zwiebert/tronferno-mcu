#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include "../mqtt_private.h"
#include "app_mqtt/mqtt.h"
#include "../app_cli/include/app_cli/cli_app.hh"

#include <string.h>

static char *Line;
extern char *Topic, *Data;

static void my_process_cmdline(char *line, const UoutWriter &td) {
  free(Line);
  Line = strdup(line);
}

void tst_receive_cmd_stop() {
  char topic[] = "tfmcu/42/cmd";
  char data[] = "stop";
  MyMqtt.received_cmdl(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"g\":4,\"m\":2,\"c\":\"stop\"}}", Line);
}

void tst_receive_cmd_stop_a() {
  char topic[] = "tfmcu/101234/cmd";
  char data[] = "stop";
  MyMqtt.received_cmdl(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"a\":\"101234\",\"c\":\"stop\"}}", Line);
}
void tst_receive_cmd_stop_agm() {
  char topic[] = "tfmcu/80abcd42/cmd";
  char data[] = "stop";
  MyMqtt.received_cmdl(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"a\":\"80abcd\",\"g\":4,\"m\":2,\"c\":\"stop\"}}", Line);
}

void tst_receive_pct_56() {
  char topic[] = "tfmcu/42/pct";
  char data[] = "56";
  MyMqtt.received_cmdl(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"g\":4,\"m\":2,\"p\":56}}", Line);
}
void tst_receive_pct_56_a() {
  char topic[] = "tfmcu/101234/pct";
  char data[] = "56";
  MyMqtt.received_cmdl(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"a\":\"101234\",\"p\":56}}", Line);
}
void tst_receive_pct_56_agm() {
  char topic[] = "tfmcu/80abcd42/pct";
  char data[] = "56";
  MyMqtt.received_cmdl(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"cmd\":{\"a\":\"80abcd\",\"g\":4,\"m\":2,\"p\":56}}", Line);
}

void tst_receive_GPO_21() {
  char topic[] = "tfmcu/gpo/21/level";
  char data[] = "1";
  MyMqtt.received_cmdl(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("{\"mcu\":{\"gpio21\":1}}", Line);
}

void tst_receive_cli_xxx() {
  char topic[] = "tfmcu/cli";
  char data[] = "config all=?";
  MyMqtt.received_cmdl(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("config all=?", Line);
}

void tst_receive_cli_version_txt() {
  Topic = Data = NULL;
  char topic[] = "tfmcu/cli";
  char data[] = "{\"mcu\":{\"version\"=\"?\"}}";
  MyMqtt.received(topic, sizeof topic - 1, data, sizeof data - 1);
  TEST_ASSERT_EQUAL_STRING("tfmcu/cli_out", Topic);
  const char *data_head = "{\"from\":\"tfmcu\",\"mcu\":{\"chip\":\"host\",\"build-time\":\"Jan-11-2023T17:04:32\"}}";
  TEST_ASSERT_EQUAL_INT(strlen(data_head), strlen(Data));
  TEST_ASSERT_EQUAL_MEMORY(data_head, Data, 50);
}

void tst_receive_cli_version_json() {
  Topic = Data = NULL;
  char topic[] = "tfmcu/cli";
  char data[] = "mcu version=?";
  MyMqtt.received(topic, sizeof topic - 1, data, sizeof data - 1);
  TEST_ASSERT_EQUAL_STRING("tfmcu/cli_out", Topic);
  const char *data_head = "{\"from\":\"tfmcu\",\"mcu\":{\"chip\":\"host\",\"build-time\":\"Jan-11-2023T17:04:32\"}}";
  TEST_ASSERT_EQUAL_INT(strlen(data_head), strlen(Data));
  TEST_ASSERT_EQUAL_MEMORY(data_head, Data, 50);
}
TEST_CASE("mqtt_receive", "[app_mqtt]")
{
  tst_receive_cli_version_json();
  tst_receive_cli_version_txt();
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
  cfg_mqtt c;
  io_mqttApp_setup(&c);
  cliApp_setup();
}
