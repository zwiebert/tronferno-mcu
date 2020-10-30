#pragma once

#include "app_config/proj_app_cfg.h"
#include "app_uout/status_output.h"
#include "net_mqtt/mqtt.hh"

extern char *io_mqtt_topic_root;

class AppNetMqtt final : public Net_Mqtt {
public:
  void connected() override;
  void disconnected() override;
  void received(const char *topic, int topic_len, const char *data, int data_len) override;
};

extern AppNetMqtt MyMqtt;

void io_mqtt_publish_gmp(const so_arg_gmp_t *gmp);
void io_mqttApp_publishPinChange(int gpio_num, bool level);

typedef void (*proc_cmdline_funT)(char *line, const TargetDesc &td);
void io_mqttApp_received(const char *topic, int topic_len, const char *data, int data_len, proc_cmdline_funT proc_cmdline_fun);
