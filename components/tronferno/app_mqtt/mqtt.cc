/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app_mqtt/mqtt.h"

#include "mqtt_private.h"
#include "net_mqtt_client/mqtt.hh"
#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.hh"
#include "uout/uout_builder_json.hh"
#include "app_uout/status_output.h"
#include <gpio/pin.h>
#include <fernotron_uout/fer_uo_publish.h>
#include <fernotron/fer_main.h>
#include <fernotron/pos/commands.h>
#include <fernotron/pos/shutter_prefs.h>

#include <stdlib.h>
#include <charconv>

char *io_mqtt_topic_root;
AppNetMqtt MyMqtt;

#define TOPIC_ROOT io_mqtt_topic_root
#define TOPIC_PCT_END "/pct"
#define TOPIC_IPCT_END "/ipct"
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

#ifdef TEST_HOST
char *Topic, *Data;
#endif

void AppNetMqtt::publish_gmp(const so_arg_gmp_t gmp) {
  char topic[64], data[16];

  snprintf(topic, sizeof topic, "%s%u%u/pct_out", TOPIC_ROOT, gmp.g, gmp.m);
  snprintf(data, sizeof data, "%u", gmp.p);
  publish(topic, data, true);

  snprintf(topic, sizeof topic, "%s%u%u/ipct_out", TOPIC_ROOT, gmp.g, gmp.m);
  snprintf(data, sizeof data, "%u", 100 - gmp.p);
  publish(topic, data, true);
}

void AppNetMqtt::publishPinChange(int gpio_num, bool level) {
  char topic[64];
  const char *data = level ? "1" : "0";
  snprintf(topic, sizeof topic, "%sgpi/%d/level", TOPIC_ROOT, gpio_num);
  publish(topic, data, true);
}

void AppNetMqtt::publish_plainCmd(const so_arg_plain_cmd_t *pl_cmd) {
  char topic[64];

  if (pl_cmd->flags.addrType_central) {
    snprintf(topic, sizeof topic, "%s%06lx%u%u/rf_out", TOPIC_ROOT, (long unsigned) pl_cmd->plain_msg->a, pl_cmd->plain_msg->g, pl_cmd->plain_msg->m);
  } else {
    snprintf(topic, sizeof topic, "%s%06lx/rf_out", TOPIC_ROOT, (long unsigned) pl_cmd->plain_msg->a);
  }
  publish(topic, pl_cmd->cmd_string, true);
}

static void io_mqttApp_uoutPublish_cb(const uoCb_msgT msg) {
  // No lock required: esp_mqtt_client_publish is thread safe
  if (auto pch = uoCb_pchFromMsg(msg))
    MyMqtt.publishPinChange(pch->gpio_num, pch->level);
  if (auto gmp = uoCb_gmpFromMsg(msg))
    MyMqtt.publish_gmp(*gmp);
  if (auto plCmd = uoCb_plainCmdFromMsg(msg))
    MyMqtt.publish_plainCmd(plCmd);
  if (auto json = uoCb_jsonFromMsg(msg)) {
    char topic[64];
    if (msg.flags.evt.uo_evt_flag_timerChange) {
      snprintf(topic, sizeof topic, "%stimer_out", TOPIC_ROOT);
      MyMqtt.publish(topic, json, true);
    }
#if 0
    if (msg.flags.evt.uo_evt_flag_rfMsgReceived) {
      snprintf(topic, sizeof topic, "%srfrx_out", TOPIC_ROOT);
      MyMqtt.publish(topic, json, true);
    }
    if (msg.flags.evt.uo_evt_flag_msgSent) {
      snprintf(topic, sizeof topic, "%srftx_out", TOPIC_ROOT);
      MyMqtt.publish(topic, json, true);
    }
#endif
  }
}

void AppNetMqtt::connected() {
  char topic[64];

  // subscribe topics on MQTT server
  snprintf(topic, sizeof topic, "%scli", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/pct", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/ipct", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s+/cmd", TOPIC_ROOT);
  subscribe(topic, 0);
  snprintf(topic, sizeof topic, "%s%s+%s", TOPIC_ROOT, TOPIC_GPO_MID, TOPIC_GPO_END);
  subscribe(topic, 0);

  // publish status change to MQTT server
  snprintf(topic, sizeof topic, "%sstatus", TOPIC_ROOT);
  publish(topic, "connected"); // for autocreate (ok???)

  // subscribe to uout-component
  uo_flagsT flags { };
  flags.tgt.mqtt = true;
  flags.evt.pin_change = true;
  flags.evt.uo_evt_flag_pctChange = true;
  flags.evt.uo_evt_flag_timerChange = true;
  flags.evt.uo_evt_flag_rfMsgReceived = true;
  flags.evt.uo_evt_flag_msgSent = true;
  flags.fmt.raw = true;
  flags.fmt.json = true;
  uoCb_subscribe(io_mqttApp_uoutPublish_cb, flags);

  // publish shutter positions
  bool groups[8] = { };

  for (auto it = fer_usedMemberMask.begin(1); it; ++it) {
    const so_arg_gmp_t gmp = { .g = it.getG(), .m = it.getM(), .p = fer_simPos_getPct_whileMoving(it.getG(), it.getM()) };
    publish_gmp(gmp);
    groups[gmp.g] = true;
  }

  for (uint8_t i = 0; i < 8; ++i)
    if (groups[i]) {
      publish_gmp(so_arg_gmp_t { .g = i, .p = fer_simPos_getPct_whileMoving(i, 0) });
    }
}

void AppNetMqtt::disconnected() {
  // unsubscribe from uout-component
  uoCb_unsubscribe(io_mqttApp_uoutPublish_cb);
}

typedef void (*proc_cmdline_fun)(char *line, UoutWriter &td);

void io_mqttApp_process_cmdline(char *cmdline, UoutWriter &td) {
  if (cmdline[0] == '{') {
    cli_process_json(cmdline, td);
  } else {
    cli_process_cmdline(cmdline, td);
  }
}

void AppNetMqtt::received(const char *topic, int topic_len, const char *data, int data_len) {
  received_cmdl(topic, topic_len, data, data_len, io_mqttApp_process_cmdline);
}

struct c_map {
  const char *fs;
  fer_if_cmd fc;
};

constexpr struct c_map const fc_map[] = { //
  { "down", fer_if_cmd_DOWN}, //
  { "up", fer_if_cmd_UP}, //
  { "stop", fer_if_cmd_STOP}, //
  { "sun-down", fer_if_cmd_SunDOWN}, //
  { "sun-up", fer_if_cmd_SunUP}, //
  { "sun-inst", fer_if_cmd_SunINST}, //
  //{"sun-test", fer_if_cmd_Program},//
  { "set", fer_if_cmd_SET},  //
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
  so_arg_agm_t r { };
  if (addr_len == 2) {
    r.g = addr[0] - '0';
    r.m = addr[1] - '0';
  } else if (addr_len == 6) {
    std::from_chars(addr, addr + 6, r.a, 16);

  } else if (addr_len == 8) {
    std::from_chars(addr, addr + 6, r.a, 16);
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

static void io_mqtt_publish_sub_topic_get_json(UoutWriter &td, const char *sub_topic) {
  char *json = td.sj().get_json();
  if (!json || !*json)
    return;

  char topic[64];
  snprintf(topic, sizeof topic, "%s%s", TOPIC_ROOT, sub_topic);

  MyMqtt.publish(topic, json);
}

void AppNetMqtt::received_cmdl(const char *topic, int topic_len, const char *data, int data_len, proc_cmdline_funT proc_cmdline_fun) {
  UoutWriterBuilder td { SO_TGT_MQTT | SO_TGT_FLAG_JSON };
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
#ifdef CONFIG_APP_USE_GPIO_PINS
      int gpio_number = -1;
      std::from_chars(addr, addr + addr_len, gpio_number, 10);
      if (*data == '?') {
        mcu_pin_state result;
        if (mcu_access_pin(gpio_number, &result, PIN_READ)) {
          if (result == PIN_CLEAR || result == PIN_SET)
            publishPinChange(gpio_number, result == PIN_SET);
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
          publish_gmp( { agm.g, agm.m, pos });
        }
      } else {
        int pct = -1;
        if (auto res = std::from_chars(data, data + data_len, pct, 10); res.ec == std::errc()) {
          fer_cmd_moveShutterToPct(agm.a, agm.g, agm.m, pct, 2);
        }
      }
    } else if (topic_endsWith(topic, topic_len, TOPIC_IPCT_END)) {
      const char *addr = topic + strlen(TOPIC_ROOT);
      int addr_len = topic_len - (strlen(TOPIC_ROOT) + (sizeof TOPIC_IPCT_END - 1));

      auto agm = topic_to_Agm(addr, addr_len);
      if (!agm.a)
        return;

      if (strncmp("?", data, data_len) == 0) {
        if (agm.a != fer_config.cu)
          return;
        if (Pct pos = fer_simPos_getPct_whileMoving(agm.g, agm.m)) {
          publish_gmp( { agm.g, agm.m, pos });
        }
      } else {
        int pct = -1;
        if (auto res = std::from_chars(data, data + data_len, pct, 10); res.ec == std::errc()) {
          pct = 100 - pct;
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

void io_mqttApp_test1() {
  const char *topic = "homeassistant/cover/shutter25/config";
  const char *data = "{"
      "\"uniq_id\":\"tronferno25ad\","
      "\"name\":\"AD 25 Erker\","
      "\"cmd_t\":\"tfmcu23/25/cmd\",\"pos_t\":\"tfmcu23/25/pct_out\",\"set_pos_t\":\"tfmcu23/25/pct\","
      "\"qos\":1,"
      "\"pl_open\":\"up\",\"pl_close\":\"down\",\"pl_stop\":\"stop\""
      "}";
  MyMqtt.publish(topic, data);
}

#define UNIQUE_ID "tronferno%s"

bool io_mqttApp_HassConfig(const class Fer_GmSet &gmSet, bool remove, const char *hass_root_topic) {

  for (auto it = gmSet.begin(1); it; ++it) {
    uint8_t g = it.getG();
    uint8_t m = it.getM();
    const char gm[] = { char('0' + g), char('0' + m), '\0' };

    char name[32] = "";
    fer_shPref_strByM_load(name, sizeof name, "NAME", g, m);

    const char topic_fmt[] = "%s/cover/" UNIQUE_ID "/config";
    const size_t topic_size = (sizeof topic_fmt) + strlen(hass_root_topic) + strlen(gm);
    char topic[topic_size];
    if (topic_size <= snprintf(topic, topic_size, topic_fmt, hass_root_topic, gm))
      return false;

    if (remove) {
      MyMqtt.publish(topic, "", true);
    } else {
      const char data_fmt[] = "{"
          "\"uniq_id\":\"" UNIQUE_ID "\"," // 1
          "\"name\":\"Rollo %s %s\","// 2
          "\"~\": \"%s%s\","// 3
          //////////////////////////////////////////
          "\"cmd_t\":\"~/cmd\","//
          "\"pos_t\":\"~/pct_out\","//
          "\"set_pos_t\":\"~/pct\","//
          "\"qos\":1,"//
          "\"dev_cla\":\"shutter\","//
          "\"pl_open\":\"up\","//
          "\"pl_cls\":\"down\","//
          "\"pl_stop\":\"stop\""//
          "}";
      const size_t data_size = (sizeof data_fmt) + strlen(gm) * 3 + strlen(name) + strlen(TOPIC_ROOT);
      char data[data_size];
      if (data_size <= snprintf(data, data_size, data_fmt //
          , gm // 1
          , gm, name // 2
          , TOPIC_ROOT, gm // 3
          )) {
        return false;
      }
      MyMqtt.publish(topic, data, true);
    }
  }

  return true;
}

