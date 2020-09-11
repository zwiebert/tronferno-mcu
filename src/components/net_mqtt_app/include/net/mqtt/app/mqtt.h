/*
 * mqtt.h
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include "uout_app/status_output.h"
#include "net/mqtt/mqtt.h"

void io_mqtt_enable(bool enable);

#ifdef USE_MQTT
void io_mqtt_publish_gmp(const so_arg_gmp_t *gmp);
#else
#define io_mqtt_publish_gmp(gmp)
#endif


void io_mqttApp_setup(const char *topic_root);

void io_mqttApp_publishPinChange(int gpio_num, bool level);

