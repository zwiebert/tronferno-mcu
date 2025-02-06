

#include "main.hh"
#include "main_loop/main_queue.hh"
#include "net/ipnet.h"

#include <freertos/FreeRTOS.h>
#include <freertos/projdefs.h>
#include <freertos/timers.h>



void loop(void) {
  mainLoop_processMessages(0, 5000);
}
