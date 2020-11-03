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
#include <gpio/pin.h>
#include <fernotron_uout/fer_uo_publish.h>
#include <fernotron/fer_main.h>
#include <fernotron/pos/commands.h>

#include <stdlib.h>
#include <charconv>

char *io_mqtt_topic_root;
AppNetMqtt MyMqtt;

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

#define TOPIC_CLI_OUT_END "cli_out"
#define TOPIC_CMD_OUT_END "cmd_out"
#define TOPIC_PCT_OUT_END "pct_out"
#define TOPIC_GPO_OUT_END "gpo_out"

static void io_mqtt_publish_sub_topic(const char *sub_topic, const char *json) {
  char topic[64];
  snprintf(topic, sizeof topic, "%s%s", TOPIC_ROOT, sub_topic);

  Net_Mqtt::publish(topic, json);
}

static void io_mqtt_publish_sub_topic_get_json(const TargetDesc &td, const char *sub_topic) {
  char *json = td.sj().get_json();
  if (!json || !*json)
    return;
  io_mqtt_publish_sub_topic(sub_topic, json);
}

void io_mqtt_publish_gmp(const so_arg_gmp_t gmp) {
  char topic[64], data[16];

  snprintf(topic, 64, "%s%u%u/pct_out", TOPIC_ROOT, gmp.g, gmp.m);
  snprintf(data, 16, "%u", gmp.p);

  Net_Mqtt::publish(topic, data);
}

void io_mqttApp_publishPinChange(int gpio_num, bool level) {
  char topic[64];
  const char *data = level ? "1" : "0";
  snprintf(topic, 64, "%sgpi/%d/level", TOPIC_ROOT, gpio_num);
  Net_Mqtt::publish(topic, data);
}


static void io_mqttApp_uoutPublish_cb(const uoCb_msgT msg) {
  // No lock required: esp_mqtt_client_publish is thread safe
  if (auto pch = uoCb_pchFromMsg(msg))
    io_mqttApp_publishPinChange(pch->gpio_num, pch->level);
  if (auto gmp = uoCb_gmpFromMsg(msg))
    io_mqtt_publish_gmp(*gmp);
  if (auto json = uoCb_jsonFromMsg(msg)) {
    if (msg.flags.evt.timer_change)
      Net_Mqtt::publish("tfmcu/timer_out", json);
  }
}

void AppNetMqtt::connected ()  {
  char topic[64];

  // subscribe topics on MQTT server
  snprintf(topic, sizeof topic, "%scli", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/pct", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/cmd", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s%s+%s", TOPIC_ROOT, TOPIC_GPO_MID, TOPIC_GPO_END);
  subscribe(topic, 0);

  // publish status change to MQTT server
  snprintf(topic, sizeof topic, "%sstatus", TOPIC_ROOT);
  publish(topic, "connected"); // for autocreate (ok???)

  // subscribe to uout-component
  uo_flagsT flags {};
  flags.tgt.mqtt = true;
  flags.evt.pin_change = true;
  flags.evt.pct_change = true;
  flags.evt.timer_change = true;
  flags.fmt.raw = true;
  flags.fmt.json = true;
  uoCb_subscribe(io_mqttApp_uoutPublish_cb, flags);
}

void AppNetMqtt::disconnected ()  {
  // unsubscribe from uout-component
  uoCb_unsubscribe(io_mqttApp_uoutPublish_cb);
}

typedef void (*proc_cmdline_fun)(char *line, const TargetDesc &td);

void io_mqttApp_process_cmdline(char *line, const TargetDesc &td) {
  cli_process_cmdline(line, td);
}

void AppNetMqtt::received(const char *topic, int topic_len, const char *data, int data_len)  {
  io_mqttApp_received(topic, topic_len, data, data_len, io_mqttApp_process_cmdline);
}


struct c_map {
  const char *fs;
  fer_if_cmd fc;
};

constexpr struct c_map const fc_map[] = { //
    { "down", fer_if_cmd_DOWN }, //
        { "up", fer_if_cmd_UP }, //
        { "stop", fer_if_cmd_STOP }, //
        { "sun-down", fer_if_cmd_SunDOWN }, //
        { "sun-up", fer_if_cmd_SunUP }, //
        { "sun-inst", fer_if_cmd_SunINST }, //
        //{"sun-test", fer_if_cmd_Program},//
        { "set", fer_if_cmd_SET },  //
    };

static fer_if_cmd parm_to_ferCMD(const char *token, size_t token_len) {
  for (int i = 0; i < (sizeof(fc_map) / sizeof(fc_map[0])); ++i) {
    if (strncmp(token, fc_map[i].fs, token_len) == 0) {
      return fc_map[i].fc;
    }
  }
  return fer_if_cmd_None;
}

static so_arg_agm_t topic_to_Agm(const char *addr, unsigned addr_len) {
  so_arg_agm_t r{};
  if (addr_len == 2) {
    r.g = addr[0] - '0';
    r.m = addr[1] - '0';
  } else if (addr_len == 6) {
    std::from_chars(addr, addr+6, r.a, 16);

  } else if (addr_len == 8) {
    std::from_chars(addr, addr+6, r.a, 16);
    r.g = addr[6] - '0';
    r.m = addr[7] - '0';
  } else {
    return r;
  }
  if (!r.a) {
    r.a = fer_config.cu;
  }
    return r;
}

void io_mqttApp_received(const char *topic, int topic_len, const char *data, int data_len, proc_cmdline_funT proc_cmdline_fun) {
  TargetDesc td { SO_TGT_MQTT | SO_TGT_FLAG_JSON };
  if (!topic_startsWith(topic, topic_len, TOPIC_ROOT)) {
    return; // all topics start with this
  }

  {
    LockGuard lock(cli_mutex);
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
#ifdef USE_GPIO_PINS
      int gpio_number = -1;
      std::from_chars(addr, addr + addr_len, gpio_number, 10);
      if (*data == '?') {
        mcu_pin_state result;
        if (mcu_access_pin(gpio_number, &result, PIN_READ)) {
          if (result == PIN_CLEAR || result == PIN_SET)
            io_mqttApp_publishPinChange(gpio_number, result == PIN_SET);
        }
      } else {
        mcu_pin_state ps = *data == '0' ? PIN_CLEAR : PIN_SET;
        mcu_access_pin(gpio_number, NULL, ps);
      }
#endif
    } else if (topic_endsWith(topic, topic_len, TOPIC_PCT_END)) {
      const char *addr = topic + strlen(TOPIC_ROOT);
      int addr_len = topic_len - (strlen(TOPIC_ROOT) + (sizeof TOPIC_PCT_END - 1));

      auto agm = topic_to_Agm(addr, addr_len);
      if (!agm.a)
        return;

      if (strncmp("?", data, data_len) == 0) {
        if (agm.a != fer_config.cu)
          return;
        if (Pct pos = fer_simPos_getPct_whileMoving(agm.g, agm.m)) {
          io_mqtt_publish_gmp({agm.g, agm.m, pos});
        }
      } else {
        int pct = -1;
        if (auto res = std::from_chars(data, data + data_len, pct, 10); res.ec == std::errc()) {
           fer_cmd_moveShutterToPct(agm.a, agm.g, agm.m, pct, 2);
        }
      }

    } else if (topic_endsWith(topic, topic_len, TOPIC_CMD_END)) {
      const char *addr = topic + strlen(TOPIC_ROOT);
      int addr_len = topic_len - (strlen(TOPIC_ROOT) + (sizeof TOPIC_CMD_END - 1));

      if (auto agm = topic_to_Agm(addr, addr_len); agm.a) {
        if (auto cmd = parm_to_ferCMD(data, data_len)) {
          fer_cmd_sendShutterCommand(agm.a, agm.g, agm.m, cmd, 2);
        }
      }

    } else if (topic_endsWith(topic, topic_len, TOPIC_CLI_END)) {
      if (data_len > TAG_CLI_LEN && strncmp(data, TAG_CLI, TAG_CLI_LEN) == 0) {
        data += TAG_CLI_LEN;
        data_len -= TAG_CLI_LEN;
      }
      memcpy(line, data, data_len);
      line[data_len] = '\0';
      proc_cmdline_fun(line, td);
      io_mqtt_publish_sub_topic_get_json(td, TOPIC_CLI_OUT_END);

    }
  }
}






