/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "app_mqtt/mqtt.h"
#include "net_mqtt/mqtt_imp.h"

#include "mqtt_private.h"
#include "net_mqtt/mqtt.hh"
#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.hh"
#include "uout/status_json.hh"
#include "app_uout/status_output.h"
#include <app_uout/callbacks.h>

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

#define TOPIC_CLI_OUT_END "cli_out"
#define TOPIC_CMD_OUT_END "cmd_out"
#define TOPIC_PCT_OUT_END "pct_out"
#define TOPIC_GPO_OUT_END "gpo_out"

static void io_mqtt_publish_topic_end(const char *topic_end, const char *json) {
  char topic[64];
  snprintf(topic, sizeof topic, "%s%s", TOPIC_ROOT, topic_end);

  Net_Mqtt::publish(topic, json);
}

static void io_mqtt_publish_topic_end_get_json(const TargetDesc &td, const char *topic_end) {
  char *json = td.sj().get_json();
  if (!json && !*json)
    return;
  io_mqtt_publish_topic_end(topic_end, json);
}

void io_mqtt_publish_gmp(const so_arg_gmp_t *gmp) {
  char topic[64], data[16];

  snprintf(topic, 64, "%s%u%u/pct_out", TOPIC_ROOT, gmp->g, gmp->m);
  snprintf(data, 16, "%u", gmp->p);

  Net_Mqtt::publish(topic, data);
}

void io_mqttApp_publishPinChange(int gpio_num, bool level) {
  char topic[64];
  const char *data = level ? "H" : "L";
  snprintf(topic, 64, "%sgpi/%d/level", TOPIC_ROOT, gpio_num);
  Net_Mqtt::publish(topic, data);
}


static void io_mqttApp_uoutPublish_cb(const uoCb_msgT msg) {
  if (auto pch = uoCb_pchFromMsg(msg))
    io_mqttApp_publishPinChange(pch->gpio_num, pch->level);
  if (auto gmp = uoCb_gmpFromMsg(msg))
    io_mqtt_publish_gmp(gmp);
  if (auto json = uoCb_jsonFromMsg(msg)) {
    if (msg.flags.evt.timer_change)
      Net_Mqtt::publish("tfmcu/timer_out", json);
  }
}

static class AppNetMqtt final : public Net_Mqtt {
public:
virtual void connected () override {
  char topic[64];
  snprintf(topic, sizeof topic, "%scli", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/pct", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/cmd", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%sstatus", TOPIC_ROOT);
  Net_Mqtt::publish(topic, "connected"); // for autocreate (ok???)

  snprintf(topic, sizeof topic, "%s%s+%s", TOPIC_ROOT, TOPIC_GPO_MID, TOPIC_GPO_END);
  subscribe(topic, 0);

  uo_flagsT flags {};
  flags.tgt.mqtt = true;
  flags.evt.pin_change = true;
  flags.evt.pct_change = true;
  flags.evt.timer_change = true;
  flags.fmt.raw = true;
  flags.fmt.json = true;
  uoCb_subscribe(io_mqttApp_uoutPublish_cb, flags);
}

virtual void disconnected () override {
  uoCb_unsubscribe(io_mqttApp_uoutPublish_cb);
}

virtual void received(const char *topic, int topic_len, const char *data, int data_len) override {
  TargetDesc td { SO_TGT_MQTT };
  if (!topic_startsWith(topic, topic_len, TOPIC_ROOT)) {
    return; // all topics start with this
  }

  { LockGuard lock(cli_mutex); 
    char line[40 + data_len];
    if (topic_endsWith(topic, topic_len, TOPIC_GPO_END)) {
      const char *addr = strstr(topic, TOPIC_GPO_MID);
      if (!addr)
        return;
      addr += strlen(TOPIC_GPO_MID);
      const char *addr_end = strchr(addr, '/');
      if (!addr_end)
        return;
      int addr_len = addr_end - addr;

      sprintf(line, "mcu gpio%.*s=%.*s", addr_len, addr, data_len, data);

      cli_process_cmdline(line, td);

    } else if (topic_endsWith(topic, topic_len, TOPIC_PCT_END)) {
      const char *addr = topic + strlen(TOPIC_ROOT);
      int addr_len = topic_len - (strlen(TOPIC_ROOT) + (sizeof TOPIC_PCT_END - 1));

      if (addr_len == 2) {
        sprintf(line, "send g=%c m=%c p=%.*s", addr[0], addr[1], data_len, data);
      } else if (addr_len == 6) {
        sprintf(line, "send a=%.*s p=%.*s", 6, addr, data_len, data);
      } else if (addr_len == 8) {
        sprintf(line, "send a=%.*s g=%c m=%c p=%.*s", 6, addr, addr[6], addr[7], data_len, data);
      } else {
       return;
        // wrong topic format in wildcard
      }
      cli_process_cmdline(line, td);

    } else if (topic_endsWith(topic, topic_len, TOPIC_CMD_END)) {
      const char *addr = topic + strlen(TOPIC_ROOT);
      int addr_len = topic_len - (strlen(TOPIC_ROOT) + (sizeof TOPIC_CMD_END - 1));

      if (addr_len == 2) {
        sprintf(line, "send g=%c m=%c c=%.*s", addr[0], addr[1], data_len, data);
      } else if (addr_len == 6) {
        sprintf(line, "send a=%.*s c=%.*s", 6, addr, data_len, data);
      } else if (addr_len == 8) {
        sprintf(line, "send a=%.*s g=%c m=%c c=%.*s", 6, addr, addr[6], addr[7], data_len, data);
      } else {
        return;
        // wrong topic format in wildcard
      }
      cli_process_cmdline(line, td);

    } else if (topic_endsWith(topic, topic_len, TOPIC_CLI_END)) {
      if (data_len > TAG_CLI_LEN && strncmp(data, TAG_CLI, TAG_CLI_LEN) == 0) {
        data += TAG_CLI_LEN;
        data_len -= TAG_CLI_LEN;
      }
      memcpy(line, data, data_len);
      line[data_len] = '\0';
      cli_process_cmdline(line, td);
      io_mqtt_publish_topic_end_get_json(td, TOPIC_CLI_OUT_END);

    }
  }
}



} MyMqtt;


void io_mqttApp_setup(const char *topic_root) {
  if (topic_root && *topic_root && (!io_mqtt_topic_root || 0 != strcmp(io_mqtt_topic_root, topic_root))) {
    char *tr = (char*)malloc(strlen(topic_root)+1);
    if (tr) {
      STRCPY (tr, topic_root);
      free(io_mqtt_topic_root);
      io_mqtt_topic_root = tr;
    }
  }

  Net_Mqtt::setup(&MyMqtt);
}


