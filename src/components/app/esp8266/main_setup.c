#include "app_config/proj_app_cfg.h"
#include "storage/storage.h"
#include "key_value_store/kvs_wrapper.h"
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
#include "net/wifistation.h"
#include "net/ntp.h"
#include "config/config.h"

extern void intTimer_setup(void);
extern void setup_serial(u32 baudrate);
extern void setup_notImplemented(void);
extern void setup_pin(void);
void setup_dataFlash(void);
void setup_dataFlash2(void);
void setup_spiffs(void);
void task_setup(void);


// hardware specific main setup
void
user_init() {
  // load config and setup serial interface
  setup_serial(115200);
  txtio_setup(cfg_getTxtio());
  io_puts("\r\n\r\n");
  io_puts("\r\n\r\n");
  io_puts("test io_puts...\n");
  io_printf("test io_printf...\n");

  stor_setup();
  kvs_setup();

  setup_dataFlash();
  read_config(~0);


  // print debug info
  print_reset_info();

  setup_notImplemented();
  setup_pin();
  wifistation_setup(cfg_getWlan());
  tcps_startServer();
  setup_udp();
  main_setup();
  task_setup();
  intTimer_setup();
  setup_dataFlash2();
}
