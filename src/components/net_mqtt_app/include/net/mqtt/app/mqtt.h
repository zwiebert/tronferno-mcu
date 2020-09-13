#pragma once

#include "app_config/proj_app_cfg.h"
#include "net/mqtt/mqtt.h"

void io_mqtt_enable(bool enable);
void io_mqttApp_setup(const char *topic_root);



