/*
 * unity_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "key_value_store/kvs_wrapper.h"
#include "storage/storage.h"
#include "txtio/inout.h"
#include "config/config.h"
#include "fernotron_pos/shutter_pct.h"
#include "fernotron/astro.h"
#include "cli/mutex.h"

 void loop_eventBits_setup();

int unit_testing;

void mcu_init(void);

void unity_suiteSetUp() {

  unit_testing = 1;

#if 0
 mcu_init();
#else
  //config_setup();
#ifdef USE_EG
  loop_eventBits_setup();
#endif
  mutex_setup();
  kvs_setup();
  txtio_setup(cfg_getTxtio());
  stor_setup();
  ferPos_init();
  config_setup_astro();
#endif

}
