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
  TEST_ASSERT_EQUAL_STRING("cmd g=4 m=2 c=stop", Line);
}

void tst_receive_pct_56() {
  char topic[] = "tfmcu/42/pct";
  char data[] = "56";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("cmd g=4 m=2 p=56", Line);
}

void tst_receive_GPO_21() {
  char topic[] = "tfmcu/gpo/21/level";
  char data[] = "1";
  io_mqttApp_received(topic, sizeof topic - 1, data, sizeof data - 1, my_process_cmdline);
  TEST_ASSERT_EQUAL_STRING("mcu gpio21=1", Line);
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
  tst_receive_GPO_21();
  tst_receive_pct_56();
  tst_receive_cmd_stop();
}


void setUp() {
  io_mqtt_topic_root = strdup("tfmcu/");
}
