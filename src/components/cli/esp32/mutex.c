/*
 * mutex.c
 *
 *  Created on: 13.07.2019
 *      Author: bertw
 */

#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t cli_mutex;

void mutex_setup() {

  /* Create a mutex type semaphore. */
  cli_mutex = xSemaphoreCreateRecursiveMutex();

  assert(cli_mutex);

}

bool mutex_cliTake() {
  if (xSemaphoreTakeRecursive(cli_mutex, portMAX_DELAY)) {
    return true;
  } else {
   //XXX restart MCU or something
    abort();
    return false;
  }
}
void mutex_cliGive() {
  xSemaphoreGiveRecursive(cli_mutex);
}
