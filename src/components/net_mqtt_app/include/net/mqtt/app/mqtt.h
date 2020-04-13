/*
 * mqtt.h
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#ifndef SRC_net_mqtt_app_H_
#define SRC_net_mqtt_app_H_

#include "app_config/proj_app_cfg.h"
#include "userio_app/status_output.h"
#include "net/mqtt/mqtt.h"

void io_mqtt_enable(bool enable);

void io_mqtt_publish_config(const char *json);

#ifdef USE_MQTT
void io_mqtt_publish_gmp(const so_arg_gmp_t *gmp);
#else
#define io_mqtt_publish_gmp(gmp)
#endif


// interface implemented in ./mqtt.c called by MQTT implementation (mcu specific)
void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len);


void io_mqttApp_setup(struct cfg_mqtt *cfg_mqtt);

#endif /* SRC_net_mqtt_app_H_ */
