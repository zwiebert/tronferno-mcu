/*
 * unity_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */


void loop_eventBits_setup();

int unit_testing;

void mcu_init(void);




#ifdef UNIT_TESTING
extern "C++" void unity_suiteSetUp();
void unity_suiteSetUp() {
  unit_testing = 1;
  abort();
  mcu_init();
}

class UnitySuiteSetup {
public:
  UnitySuiteSetup(): x(4) {
  abort();
    unit_testing = 1;
    mcu_init();
    register_settings();
  }
  int x;
};

//volatile UnitySuiteSetup uss;

#endif
