/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "user_config.h"
#ifdef USE_MQTT

#include "./mqtt.h"
#include "./mqtt_imp.h"

#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "userio/status_json.h"
#include "userio/status_output.h"

#define TOPIC_ROOT "tfmcu/"
#define TOPIC_CLI TOPIC_ROOT "cli"
#define TOPIC_STATUS TOPIC_ROOT "status"
#define TOPIC_PCT_END "/pct"
#define TOPIC_PCT_MID "+"   // wildcard can be a, gm, agm (like: 23, 102030, 8090a023)
#define TOPIC_PCT TOPIC_ROOT TOPIC_PCT_MID TOPIC_PCT_END
#define TOPIC_CMD_END "/cmd"
#define TOPIC_CMD_MID "+"
#define TOPIC_CMD TOPIC_ROOT TOPIC_CMD_MID TOPIC_CMD_END

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

void io_mqtt_enable(bool enable) {
  if (enable) {
    io_mqtt_create_and_start();
    s_json_config_out = io_mqtt_publish_config;
  } else {
    s_json_config_out = 0;
    io_mqtt_stop_and_destroy();
  }
}

// implementation interface
void io_mqtt_connected () {
  io_mqtt_subscribe(TOPIC_CLI, 0);
  io_mqtt_subscribe(TOPIC_PCT, 0);
  io_mqtt_subscribe(TOPIC_CMD, 0);
  io_mqtt_publish(TOPIC_STATUS, "connected"); // for autocreate (ok???)
}

// implementation interface
void io_mqtt_disconnected () {
}

void io_mqtt_subscribed(const char *topic, int topic_len) {

}

void io_mqtt_unsubscribed(const char *topic, int topic_len) {

}

void io_mqtt_published(int msg_id) {

}

bool topic_startsWith(const char *topic, int topic_len, const char *s) {
  size_t s_len = strlen(s);
  return topic_len >= s_len && (0 == strncmp(topic, s, s_len));
}

bool topic_endsWith(const char *topic, int topic_len, const char *s) {
  size_t s_len = strlen(s);
  return topic_len >= s_len && (0 == strncmp(topic + topic_len - s_len, s, s_len));
}

void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len) {

  if (!topic_startsWith(topic, topic_len, TOPIC_ROOT)) {
    return; // all topics start with this
  }

  so_tgt_set(SO_TGT_MQTT);

  if (topic_endsWith(topic, topic_len, TOPIC_PCT_END)) {
    char *line = set_commandline("x", 1);
    const char *addr = topic + (sizeof TOPIC_ROOT - 1);
    int addr_len = topic_len - ((sizeof TOPIC_ROOT - 1) + (sizeof TOPIC_PCT_END -1));

    if (addr_len == 2) {
      sprintf(line, "send g=%c m=%c p=%.*s", addr[0], addr[1], data_len, data);
    } else if (addr_len == 6) {
      sprintf(line, "send a=%.*s p=%.*s", 6, addr, data_len, data);
    } else if (addr_len == 8) {
      sprintf(line, "send a=%.*s g=%c m=%c p=%.*s", 6, addr, addr[6], addr[7], data_len, data);
    } else {
      return;  // wrong topic format in wildcard
    }
    cli_process_cmdline(line);

  } else if (topic_endsWith(topic, topic_len, TOPIC_CMD_END)) {
    char *line = set_commandline("x", 1);
    const char *addr = topic + (sizeof TOPIC_ROOT - 1);
    int addr_len = topic_len - ((sizeof TOPIC_ROOT - 1) + (sizeof TOPIC_CMD_END -1));

    if (addr_len == 2) {
      sprintf(line, "send g=%c m=%c c=%.*s", addr[0], addr[1], data_len, data);
    } else if (addr_len == 6) {
      sprintf(line, "send a=%.*s c=%.*s", 6, addr, data_len, data);
    } else if (addr_len == 8) {
      sprintf(line, "send a=%.*s g=%c m=%c c=%.*s", 6, addr, addr[6], addr[7], data_len, data);
    } else {
      return;  // wrong topic format in wildcard
    }
    cli_process_cmdline(line);

  } else if (strlen(TOPIC_CLI) == topic_len && 0 == strncmp(topic, TOPIC_CLI, topic_len)) {
    if (strncmp(data, TAG_CLI, TAG_CLI_LEN) == 0) {
      char *line;
      if ((line = set_commandline(data + TAG_CLI_LEN, data_len - TAG_CLI_LEN))) {
        cli_process_cmdline(line);
      }
    } else if ((0 == strncmp(data, TAG_SEND, TAG_SEND_LEN)) || (0 == strncmp(data, TAG_CONFIG, TAG_CONFIG_LEN))
        || (0 == strncmp(data, TAG_TIMER, TAG_TIMER_LEN))) {
      char *line;
      if ((line = set_commandline(data, data_len))) {
        cli_process_cmdline(line);
      }
    }
  }

  so_tgt_default();
}

#endif // USE_MQTT

