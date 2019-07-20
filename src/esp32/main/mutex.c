/*
 * mutex.c
 *
 *  Created on: 13.07.2019
 *      Author: bertw
 */

#include "user_config.h"
#include "mutex.h"
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t cli_mutex;

void mutex_setup() {

  /* Create a mutex type semaphore. */
  cli_mutex = xSemaphoreCreateMutex();

  assert(cli_mutex);

}

bool mutex_cliTake() {
  if (xSemaphoreTake(cli_mutex, portMAX_DELAY)) {
    return true;
  } else {
   //XXX restart MCU or something
    return false;
  }
}
void mutex_cliGive() {
  xSemaphoreGive(cli_mutex);
}