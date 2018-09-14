/*
 * user_main.c
 *
 *  Created on: 11.09.2017
 *      Author: bertw
 */

#include "esp_missing_includes.h"

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"

#include "../user_config.h"

#include "main/common.h"
#include "main/inout.h"
#include "main/config.h"
#ifdef DEBUG
#include <../../esp-gdbstub/gdbstub.h>
#endif

#define printf ets_uart_printf

void loop(void);
void tcps_loop(void);

extern void setup_timer(void);
extern void setup_serial(uint32_t baudrate);
extern void setup_notImplemented(void);
extern void setup_pin(void);
void main_setup(void);
void setup_wifistation(void);
void setup_dataFlash(void);
void setup_dataFlash2(void);
void setup_ntp(void);
void setup_spiffs(void);

// main loop task
#define user_procTaskPrio     2
#define user_procTaskQueueLen 1
static os_event_t user_procTaskQueue[user_procTaskQueueLen];

// io task
#define io_procTaskPrio     0
#define io_procTaskQueueLen 1
static os_event_t io_procTaskQueue[io_procTaskQueueLen];

static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events) {
  tcps_loop();
  udp_loop();
  loop();
  ets_delay_us(1000);
  system_os_post(user_procTaskPrio, 0, 0);
}

static void ICACHE_FLASH_ATTR
io_procTask(os_event_t *events) {
  tcps_loop();
  ets_delay_us(1000);
  system_os_post(io_procTaskPrio, 0, 0);
}

void ICACHE_FLASH_ATTR
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
      printf("(hint: pass epc1 to $ xtensa-lx106-elf-addr2line -e build/app.out)\n\n\n");

    }
  }
}


#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 2)
#define SYSTEM_PARTITION_OTA_SIZE             0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR             0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR            0xfb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR            0xfc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR        0xfd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 3)
#define SYSTEM_PARTITION_OTA_SIZE             0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR             0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR            0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR            0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR        0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 4)
#define SYSTEM_PARTITION_OTA_SIZE             0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR             0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR            0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR            0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR        0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 5)
#define SYSTEM_PARTITION_OTA_SIZE             0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR             0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR            0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR            0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR        0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0xfc000
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_OTA_SIZE             0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR             0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR            0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR            0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR        0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0xfc000
#else
#error "The flash map is not supported"
#endif

#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM                SYSTEM_PARTITION_CUSTOMER_BEGIN

uint32 priv_param_start_sec;
#ifdef SPI_FLASH_SIZE_MAP
static const partition_item_t at_partition_table[] = {
    { SYSTEM_PARTITION_BOOTLOADER,            0x0,                        0x1000},
    { SYSTEM_PARTITION_OTA_1,               0x1000,                       SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_OTA_2,               SYSTEM_PARTITION_OTA_2_ADDR,            SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_RF_CAL,              SYSTEM_PARTITION_RF_CAL_ADDR,             0x1000},
    { SYSTEM_PARTITION_PHY_DATA,            SYSTEM_PARTITION_PHY_DATA_ADDR,           0x1000},
    { SYSTEM_PARTITION_SYSTEM_PARAMETER,        SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR,       0x3000},
    { SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM,             SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR,          0x1000},
};


void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
    os_printf("system_partition_table_regist fail\r\n");
    while(1);
  }
}
#endif


// hardware specific main setup
void ICACHE_FLASH_ATTR
user_init() {

  // load config and setup serial interface
  setup_dataFlash();
#if ENABLE_SPIFFS
  setup_spiffs();
#endif
  read_config(~0);
  setup_serial(C.mcu_serialBaud);
  io_puts("\r\n\r\n");

  // print debug info
  print_reset_info();

  setup_notImplemented();
#ifdef USE_NTP
  setup_ntp();
#endif
  setup_pin();
  setup_wifistation();
  setup_tcp_server();
  setup_udp();

  main_setup();

  system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
  system_os_post(user_procTaskPrio, 0, 0);
#if 0
  system_os_task(io_procTask, io_procTaskPrio, io_procTaskQueue, io_procTaskQueueLen);
  system_os_post(io_procTaskPrio, 0, 0 );
#endif

  setup_timer();

  setup_dataFlash2();


#ifdef DEBUG
  gdbstub_init();
#endif
  // system_set_os_print(1);
  //system_print_meminfo();

}

void ICACHE_FLASH_ATTR mcu_restart(void) {
  printf("mcu_restart()\n");
  os_delay_us(10000);
  system_restart();
  for (;;) {
  };
}
