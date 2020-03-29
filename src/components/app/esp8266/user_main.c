/*
 * user_main.c
 *
 *  Created on: 11.09.2017
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "pin.h"

#include "txtio/inout.h"
#include "driver/uart.h"
#include "fernotron/callbacks.h"



#include "app/common.h"
#include "config/config.h"

#define printf ets_uart_printf




// main loop task
#define user_procTaskPrio     2
#define user_procTaskQueueLen 1
static os_event_t user_procTaskQueue[user_procTaskQueueLen];

// io task
#define io_procTaskPrio     0
#define io_procTaskQueueLen 1

void main_setup_ip_dependent() {

}

static void 
user_procTask(os_event_t *events) {
  tcps_loop();
  udp_loop();
  loop();
  ets_delay_us(1000);
  system_os_post(user_procTaskPrio, 0, 0);
}

void task_setup() {
  system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
  system_os_post(user_procTaskPrio, 0, 0);
}

void 
print_reset_info() {
  struct rst_info *rtc_info = system_get_rst_info();
  if (rtc_info) {
    printf("\n                                          \n");
    printf("\r\n\r\n\r\nreset reason: %x\n", rtc_info->reason);
    if (rtc_info->reason == REASON_WDT_RST || rtc_info->reason == REASON_EXCEPTION_RST || rtc_info->reason == REASON_SOFT_WDT_RST) {
      if (rtc_info->reason == REASON_EXCEPTION_RST) {
        printf("Fatal exception (%d):\n", rtc_info->exccause);
      }
      printf("epc1=0x%08x, epc2=0x%08x, epc3=0x%08x, excvaddr=0x%08x,	depc=0x%08x\n", rtc_info->epc1, rtc_info->epc2, rtc_info->epc3, rtc_info->excvaddr,
          rtc_info->depc); //The address of the last crash is printed, which is used to debug garbled	output.
      printf("(hint: pass epc1 to $ xtensa-lx106-elf-addr2line -e esp8266-build/app.out)\n\n\n");

    }
  }
}



void  mcu_restart(void) {
  printf("mcu_restart()\n");
  os_delay_us(10000);
  system_restart();
  for (;;) {
  };
}

