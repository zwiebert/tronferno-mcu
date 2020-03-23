/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#ifdef USE_MQTT

#include "mqtt.h"
#include "mqtt_imp.h"

#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.h"
#include "userio/status_json.h"
#include "userio_app/status_output.h"

void io_mqtt_enable(bool enable) {
  if (enable) {
    io_mqtt_create_and_start();
    io_mqttApp_enable(enable);
  } else {
    io_mqttApp_enable(enable);
    io_mqtt_stop_and_destroy();
  }
}

// implementation interface
void io_mqtt_disconnected () {
  io_mqttApp_disconnected();
}

void io_mqtt_subscribed(const char *topic, int topic_len) {
  io_mqttApp_subscribed(topic, topic_len);
}

void io_mqtt_unsubscribed(const char *topic, int topic_len) {
  io_mqttApp_unsubscribed(topic, topic_len);
}

void io_mqtt_published(int msg_id) {
  io_mqttApp_published(msg_id);
}

bool topic_startsWith(const char *topic, int topic_len, const char *s) {
  size_t s_len = strlen(s);
  return topic_len >= s_len && (0 == strncmp(topic, s, s_len));
}

bool topic_endsWith(const char *topic, int topic_len, const char *s) {
  size_t s_len = strlen(s);
  return topic_len >= s_len && (0 == strncmp(topic + topic_len - s_len, s, s_len));
}


#endif // USE_MQTT

