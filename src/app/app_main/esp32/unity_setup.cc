/*
 * unity_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */
#if 0
#include "key_value_store/kvs_wrapper.h"
#include "storage/storage.h"
#include "txtio/inout.h"
#include "app/settings/config.h"
#include "fernotron/pos/shutter_pct.h"
#include "fernotron/astro.h"
#include "cli/mutex.hh"
#endif

 void loop_eventBits_setup();

int unit_testing;

void mcu_init(void);




#ifdef UNIT_TESTING
extern "C" void unity_suiteSetUp();
void unity_suiteSetUp() {
  unit_testing = 1;
  mcu_init();
}

class UnitySuiteSetup {
public:
  UnitySuiteSetup(): x(4) {
    unit_testing = 1;
    mcu_init();
  }
  int x;
};

//volatile UnitySuiteSetup uss;

#endif
