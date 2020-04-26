/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#ifdef USE_MQTT

#include "mqtt.h"
#include "net/mqtt/mqtt_imp.h"

#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.h"
#include "userio/status_json.h"
#include "userio_app/status_output.h"
#include "config/config.h"

static char *io_mqtt_topic_root;

#define TOPIC_ROOT io_mqtt_topic_root
#define TOPIC_PCT_END "/pct"
#define TOPIC_CMD_END "/cmd"
#define TOPIC_CLI_END "/cli"
#define TOPIC_GPI_MID "gpi/"
#define TOPIC_GPI_END "/level"
#define TOPIC_GPO_MID "gpo/"
#define TOPIC_GPO_END "/level"

#define TAG_CLI "cli "
#define TAG_CLI_LEN (sizeof(TAG_CLI) - 1)
#define TAG_SEND "send "
#define TAG_SEND_LEN (sizeof(TAG_SEND) - 1)
#define TAG_CONFIG "config "
#define TAG_CONFIG_LEN (sizeof(TAG_CONFIG) - 1)
#define TAG_TIMER "timer "
#define TAG_TIMER_LEN (sizeof(TAG_TIMER) - 1)

void io_mqtt_publish_config(const char *s)  {
  if (so_tgt_test(SO_TGT_MQTT)) //FIXME: implement this better
    io_mqtt_publish("tfmcu/config_out", s);
}

void io_mqtt_publish_gmp(const so_arg_gmp_t *gmp) {
  char topic[64], data[16];

  snprintf(topic, 64, "%s%u%u/pct_out", TOPIC_ROOT, gmp->g, gmp->m);
  snprintf(data, 16, "%u", gmp->p);

  io_mqtt_publish(topic, data);
}

void io_mqttApp_publishPinChange(int gpio_num, bool level) {
  char topic[64];
  const char *data = level ? "H" : "L";
  snprintf(topic, 64, "%sgpi/%d/level", TOPIC_ROOT, gpio_num);
  io_mqtt_publish(topic, data);
}

void io_mqtt_connected () {
  char topic[64];
  snprintf(topic, sizeof topic, "%scli", TOPIC_ROOT);
  io_mqtt_subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/pct", TOPIC_ROOT);
  io_mqtt_subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/cmd", TOPIC_ROOT);
  io_mqtt_subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%sstatus", TOPIC_ROOT);
  io_mqtt_publish(topic, "connected"); // for autocreate (ok???)

  snprintf(topic, sizeof topic, "%s%s+%s", TOPIC_ROOT, TOPIC_GPO_MID, TOPIC_GPO_END);
  io_mqtt_subscribe(topic, 0);
}


void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len) {

  if (!topic_startsWith(topic, topic_len, TOPIC_ROOT)) {
    return; // all topics start with this
  }

  if (mutex_cliTake()) {
    if (topic_endsWith(topic, topic_len, TOPIC_GPO_END)) {
      char *line = set_commandline("x", 1);
      const char *addr = strstr(topic, TOPIC_GPO_MID);
      if (!addr)
        goto RETURN;
      addr += strlen(TOPIC_GPO_MID);
      const char *addr_end = strchr(addr, '/');
      if (!addr_end)
        goto RETURN;
      int addr_len = addr_end - addr;

      sprintf(line, "mcu gpio%.*s=%.*s", addr_len, addr, data_len, data);

      cli_process_cmdline(line, SO_TGT_MQTT);

    } else if (topic_endsWith(topic, topic_len, TOPIC_PCT_END)) {
      char *line = set_commandline("x", 1);
      const char *addr = topic + strlen(TOPIC_ROOT);
      int addr_len = topic_len - (strlen(TOPIC_ROOT) + (sizeof TOPIC_PCT_END - 1));

      if (addr_len == 2) {
        sprintf(line, "send g=%c m=%c p=%.*s", addr[0], addr[1], data_len, data);
      } else if (addr_len == 6) {
        sprintf(line, "send a=%.*s p=%.*s", 6, addr, data_len, data);
      } else if (addr_len == 8) {
        sprintf(line, "send a=%.*s g=%c m=%c p=%.*s", 6, addr, addr[6], addr[7], data_len, data);
      } else {
        goto RETURN;
        // wrong topic format in wildcard
      }
      cli_process_cmdline(line, SO_TGT_MQTT);

    } else if (topic_endsWith(topic, topic_len, TOPIC_CMD_END)) {
      char *line = set_commandline("x", 1);
      const char *addr = topic + strlen(TOPIC_ROOT);
      int addr_len = topic_len - (strlen(TOPIC_ROOT) + (sizeof TOPIC_CMD_END - 1));

      if (addr_len == 2) {
        sprintf(line, "send g=%c m=%c c=%.*s", addr[0], addr[1], data_len, data);
      } else if (addr_len == 6) {
        sprintf(line, "send a=%.*s c=%.*s", 6, addr, data_len, data);
      } else if (addr_len == 8) {
        sprintf(line, "send a=%.*s g=%c m=%c c=%.*s", 6, addr, addr[6], addr[7], data_len, data);
      } else {
        goto RETURN;
        // wrong topic format in wildcard
      }
      cli_process_cmdline(line, SO_TGT_MQTT);

    } else if (topic_endsWith(topic, topic_len, TOPIC_CLI_END)) {
      if (strncmp(data, TAG_CLI, TAG_CLI_LEN) == 0) {
        char *line;
        if ((line = set_commandline(data + TAG_CLI_LEN, data_len - TAG_CLI_LEN))) {
          cli_process_cmdline(line, SO_TGT_MQTT);
        }
      } else if ((0 == strncmp(data, TAG_SEND, TAG_SEND_LEN)) || (0 == strncmp(data, TAG_CONFIG, TAG_CONFIG_LEN))
          || (0 == strncmp(data, TAG_TIMER, TAG_TIMER_LEN))) {
        char *line;
        if ((line = set_commandline(data, data_len))) {
          cli_process_cmdline(line, SO_TGT_MQTT);
        }
      }
    }

    RETURN:
    mutex_cliGive();
  }
}


void io_mqttApp_enable(bool enable) {
  if (enable) {
    sj_callback_onClose_ifNotEmpty = io_mqtt_publish_config;
  } else {
    sj_callback_onClose_ifNotEmpty = 0;
  }
}

void io_mqttApp_disconnected() {
}
void io_mqttApp_subscribed(const char *topic, int topic_len) {
}
void io_mqttApp_unsubscribed(const char *topic, int topic_len) {
}
void io_mqttApp_published(int msg_id) {
}

void io_mqttApp_setup(struct cfg_mqtt *cfg_mqtt, const char *topic_root) {
  if (topic_root && *topic_root && (!io_mqtt_topic_root || 0 != strcmp(io_mqtt_topic_root, topic_root))) {
    char *tr = malloc(strlen(topic_root)+1);
    if (tr) {
      strcpy (tr, topic_root);
      free(io_mqtt_topic_root);
      io_mqtt_topic_root = tr;
    }
  }
  io_mqtt_setup(cfg_mqtt);
  pin_notify_input_change_cb =  (cfg_mqtt && cfg_mqtt->enable) ? io_mqttApp_publishPinChange : 0;
}

#endif // USE_MQTT

