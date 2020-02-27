/*
 * unity_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "key_value_store/kvs_wrapper.h"
#include "storage/storage.h"

void unity_suiteSetUp() {

  kvs_setup();
  storage_setup();

}
