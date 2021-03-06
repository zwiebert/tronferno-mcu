/**
 * \file     app_mqtt/mqtt.h
 * \brief    component which connects mqtt client to CLI and uout/callbacks
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include "net_mqtt/mqtt.h"

/**
 * \brief setup the component. Will not call the setup of the MQTT client (\link io_mqtt_setup \endlink). FIXME:?!
 */
void io_mqttApp_setup(const char *topic_root);



