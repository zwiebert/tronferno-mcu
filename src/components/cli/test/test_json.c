/*
 * test_json.c
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */


/*
 * test_cli.c
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#include "unity.h"
#include "app_config/proj_app_cfg.h"
#include <string.h>
#include "userio/status_json.h"
#include "cli/cli_json.h"
#include "cli/cli.h"

//char json[] = "{ \"to\": \"tfmcu\", \"config\": { \"cu\": \"801234\",\"baud\": 115200,\"longitude\": 13.5,\"latitude\": 52.6,\"timezone\": 1.0,\"tz\": \"\",\"verbose\": 5,\"mqtt-enable\": 1,\"mqtt-url\": \"mqtt://192.168.1.42:7777\",\"mqtt-user\": \"mqusr\",\"mqtt-password\": \"mqpw\" } }";

#define NAME      "timer"
#define CMD_OBJ   "\"g\":1,\"m\":0,\"f\":\"imDwArs\",\"daily\":\"0700\",\"astro\":15"
#define NAME2     "config"
#define CMD_OBJ2  "\"cu\": \"801234\",\"baud\": 115200,\"longitude\": 13.5,\"latitude\": 52.6,\"timezone\": 1.0,\"tz\": \"\""

static char json[] = "{\"to\":\"tfmcu\",\"" NAME "\":{" CMD_OBJ "},\"" NAME2 "\":{" CMD_OBJ2 "}" "}";

static void test_json() {
  char *name, *cmd_obj, *next_name;
  int i;


  cmd_obj = json_get_command_object(json, &name, &next_name);
  TEST_ASSERT_EQUAL_STRING(NAME, name);
  TEST_ASSERT_EQUAL_STRING(CMD_OBJ, cmd_obj);
  TEST_ASSERT_NOT_NULL(next_name);

  //printf("name: %s, obj: %s\n", name, cmd_obj);

  if (cmd_obj) {
    int len = parse_json(name, cmd_obj);
    i=0;
    TEST_ASSERT_EQUAL_STRING(NAME, cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("g", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("1", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("m", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("0", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("f", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("imDwArs", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("daily", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("0700", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("astro", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("15", cli_par[i].val);
    ++i;



    cmd_obj = json_get_command_object(next_name, &name, &next_name);
    TEST_ASSERT_EQUAL_STRING(NAME2, name);
    TEST_ASSERT_EQUAL_STRING(CMD_OBJ2, cmd_obj);

    cmd_obj = json_get_command_object(next_name, &name, &next_name);
    TEST_ASSERT_NULL(cmd_obj);


    //for (int i = 0; i < len; ++i)  printf("key: %s, val: %s\n", cli_par[i].key, cli_par[i].val);

  }
}


TEST_CASE("json", "[cli]")
{
     test_json();
}
