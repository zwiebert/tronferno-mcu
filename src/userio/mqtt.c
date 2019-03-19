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

#define TOPIC_CLI "tfmcu/cli"

// implementation interface
void io_mqtt_connected () {
  io_mqtt_subscribe(TOPIC_CLI, 0);
  io_mqtt_publish("tfmcu/status", "connected"); // for autocreate (ok???)
}

void io_mqtt_subscribed(const char *topic, int topic_len) {

}

void io_mqtt_unsubscribed(const char *topic, int topic_len) {

}

void io_mqtt_published(int msg_id) {

}

void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len) {
  if (strlen(TOPIC_CLI) == topic_len && 0 == strncmp(topic, TOPIC_CLI, topic_len)) {
    if (strncmp(data, "cli ", 4) == 0) {
      char *line;
      if ((line = set_commandline(data + 4, data_len - 4))) {
        process_cmdline(line);
      }
    }
  }
}



