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

extern void intTimer_setup(void);
extern void setup_serial(u32 baudrate);
extern void setup_notImplemented(void);
extern void setup_pin(void);
void wifistation_setup(void);
void setup_dataFlash(void);
void setup_dataFlash2(void);
void ntp_setup(void);
void setup_spiffs(void);
void task_setup(void);


// hardware specific main setup
void
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
  ntp_setup();
#endif
  setup_pin();
  wifistation_setup();
  tcps_startServer();
  setup_udp();
  main_setup();
  task_setup();
  intTimer_setup();
  setup_dataFlash2();
}
