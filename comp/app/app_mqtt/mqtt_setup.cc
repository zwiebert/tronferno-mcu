/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "app_mqtt/mqtt.h"
#include "net_mqtt/mqtt.hh"
#include "net_mqtt/mqtt_imp.h"
#include "mqtt_private.h"

void io_mqttApp_setup(const char *topic_root) {
  if (topic_root && *topic_root && (!io_mqtt_topic_root || 0 != strcmp(io_mqtt_topic_root, topic_root))) {
    free(io_mqtt_topic_root);
    if ((io_mqtt_topic_root = static_cast<char *>(malloc(strlen(topic_root) + 2)))) {
      strcat(strcpy(io_mqtt_topic_root, topic_root), "/");
    }
  }

  Net_Mqtt::setup(&MyMqtt);
}


