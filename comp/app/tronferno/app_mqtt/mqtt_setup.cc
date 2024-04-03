/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */


#include "net_mqtt_client/mqtt.hh"
#include "mqtt_private.h"

void io_mqttApp_setup(struct cfg_mqtt *cp) {
  io_mqtt_setup(cp);

  const char *topic_root = cp->root_topic;
  if (topic_root && *topic_root && (!io_mqtt_topic_root || 0 != strcmp(io_mqtt_topic_root, topic_root))) {
    free(io_mqtt_topic_root);
    if ((io_mqtt_topic_root = static_cast<char *>(malloc(strlen(topic_root) + 2)))) {
      strcat(strcpy(io_mqtt_topic_root, topic_root), "/");
    }
  }

  Net_Mqtt::setup(&MyMqtt);
}


