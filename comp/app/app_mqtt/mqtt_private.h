#pragma once

#include "app/config/proj_app_cfg.h"
#include "app/uout/status_output.h"
#include "net/mqtt/mqtt.h"

void io_mqtt_publish_gmp(const so_arg_gmp_t *gmp);
void io_mqttApp_publishPinChange(int gpio_num, bool level);

