#pragma once


#include "app_uout/status_output.h"
#include "net_mqtt/mqtt.hh"

/**
 * \brief  Root topic allocated with malloc(3).
 *  XXX: Currently always set to "tfmcu/"
 * \note   The root topic should be user-configurable and always end with '/'.
 */
extern char *io_mqtt_topic_root;

/**
 * \brief Methods called by MQTT client component
 */
class AppNetMqtt final : public Net_Mqtt {
public:
  void connected() override;
  void disconnected() override;
  void received(const char *topic, int topic_len, const char *data, int data_len) override;
#ifdef TEST_HOST
  void publish(const char *topic, const char *data, bool retain = false) override {
    extern char *Topic, *Data;
    free(Topic), Topic = strdup(topic);
    free(Data), Data = strdup(data);
    Net_Mqtt::publish(topic, data, retain);
  }
#endif

  /**
   * \brief      Publish shutter percentage change
   * \param gmp  Percentage data
   */
  void publish_gmp(const so_arg_gmp_t gmp);

  /**
   * \brief           Publish level change for Input pin
   * \param gpio_num  GPIO number of input pin
   * \param level     level of pin (0 or 1)
   */
  void publishPinChange(int gpio_num, bool level);

  typedef void (*proc_cmdline_funT)(char *line, const UoutWriter &td);
  /**
   * \brief  Create a CLI command line from incoming topic/data
   */
  void received_cmdl(const char *topic, int topic_len, const char *data, int data_len, proc_cmdline_funT proc_cmdline_fun);
};

/**
 * \brief Will be passed to MQTT client component
 */
extern AppNetMqtt MyMqtt;

