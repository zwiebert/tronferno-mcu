/**
 * \file     app_mqtt/mqtt.h
 * \brief    component which connects mqtt client to CLI and uout/callbacks
 */

#pragma once


#include "net_mqtt/mqtt.hh"

void io_mqttApp_setup(struct cfg_mqtt *cp);

void io_mqttApp_test1();

bool io_mqttApp_HassConfig(const class Fer_GmSet &gmSet, bool remove = false, const char *hass_root_topic = "homeassistant");
