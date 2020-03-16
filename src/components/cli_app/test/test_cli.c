/*
 * test_cli.c
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#include "unity.h"


#include "app/proj_app_cfg.h"
#include <string.h>
#include "config/config.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "cli_app.h"
#include "userio/status_json.h"
#include "fernotron/fsb.h"



static void test_commandline() {
  char cl[] = "config verbose=? cu=?;";  //"timer g=2 m=2 weekly=08222000++++10552134+";
  int n = parse_commandline(cl);
  TEST_ASSERT_EQUAL(3, n);

  if (n > 0)
    if (sj_open_root_object("tfmcu")) {
      process_parm(par, n);
      sj_close_root_object();
    }

}




TEST_CASE("parse commandline", "[cli_app]")
{
     test_commandline();
}