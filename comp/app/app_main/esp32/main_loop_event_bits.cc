#include "app_config/proj_app_cfg.h"

#include "main.h"
#include <cli/cli.h>
#include <freertos/event_groups.h>
#include <freertos/projdefs.h>
#include <freertos/timers.h>
#include <cstdio>
#include <app_settings/config.h>
#include "utils_misc/int_macros.h"
#include "utils_misc/int_types.h"
#include "utils_time/run_time.h"
#include "utils_time/ut_constants.hh"

#include "net/ipnet.h"

#include "../app_private.h"



EventGroupHandle_t loop_event_group;
#define EVT_BITS  ((1 << lf_Len) - 1)


 void lf_setBits(const EventBits_t uxBitsToSet) {
  xEventGroupSetBits(loop_event_group, uxBitsToSet);
}
void lf_setBits_ISR(const EventBits_t uxBitsToSet, bool yield);

void IRAM_ATTR lf_setBits_ISR(const EventBits_t uxBitsToSet, bool yield) {
   BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;

   xResult = xEventGroupSetBitsFromISR(
       loop_event_group,  // The event group being updated.
       uxBitsToSet, // The bits being set.
       &xHigherPriorityTaskWoken );

   // Was the message posted successfully?
   if (yield && xResult == pdPASS) {
     portYIELD_FROM_ISR();
   }
 }

typedef void (*lfa_funT)(void);

void loop_eventBits_setup() {
  loop_event_group = xEventGroupCreate();
}
u32 loop_eventBits_wait() {
  EventBits_t bits = xEventGroupWaitBits(loop_event_group, EVT_BITS, pdTRUE, pdFALSE, portMAX_DELAY);
  return bits;
}

void loop_eventBits_check() {
  u32 lf = loop_eventBits_wait();
  if (!lf)
    return;
  loop_fun_table_calls(lf);
}
