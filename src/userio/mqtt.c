/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "user_config.h"
#include "./mqtt.h"
#include <string.h>
#include "cli/cli.h"
#include "userio/status_json.h"

#define TOPIC_CLI "tfmcu/cli"
#define TOPIC_STATUS "tfmcu/status"

#define TAG_CLI "cli "
#define TAG_CLI_LEN (sizeof(TAG_CLI) - 1)
#define TAG_SEND "send "
#define TAG_SEND_LEN (sizeof(TAG_SEND) - 1)
#define TAG_CONFIG "config "
#define TAG_CONFIG_LEN (sizeof(TAG_CONFIG) - 1)
#define TAG_TIMER "timer "
#define TAG_TIMER_LEN (sizeof(TAG_TIMER) - 1)

static void io_mqtt_publish_config(const char *s)  {
  io_mqtt_publish("tfmcu/config_out", s);
}

void io_mqtt_enable(bool enable) {
  if (enable) {
    io_mqtt_create_and_start();
    s_json_config_out = io_mqtt_publish_config;
  } else {
    s_json_config_out = 0;
    io_mqtt_stop_and_destroy();
  }
}

// implementation interface
void io_mqtt_connected () {
  io_mqtt_subscribe(TOPIC_CLI, 0);
  io_mqtt_publish(TOPIC_STATUS, "connected"); // for autocreate (ok???)
}

// implementation interface
void io_mqtt_disconnected () {
}

void io_mqtt_subscribed(const char *topic, int topic_len) {

}

void io_mqtt_unsubscribed(const char *topic, int topic_len) {

}

void io_mqtt_published(int msg_id) {

}

void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len) {

  if (strlen(TOPIC_CLI) == topic_len && 0 == strncmp(topic, TOPIC_CLI, topic_len)) {
    if (strncmp(data, TAG_CLI, TAG_CLI_LEN) == 0) {
      char *line;
      if ((line = set_commandline(data + TAG_CLI_LEN, data_len - TAG_CLI_LEN))) {
        process_cmdline(line);
      }
    } else if ((0 == strncmp(data, TAG_SEND, TAG_SEND_LEN)) || (0 == strncmp(data, TAG_CONFIG, TAG_CONFIG_LEN))
        || (0 == strncmp(data, TAG_TIMER, TAG_TIMER_LEN))) {
      char *line;
      if ((line = set_commandline(data, data_len))) {
        process_cmdline(line);
      }
    }
  }
}



