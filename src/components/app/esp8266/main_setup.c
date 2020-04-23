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
#include "net/tcp_cli_server.h"
#include "config/config.h"

extern void intTimer_setup(void);
extern void setup_serial(u32 baudrate);
extern void setup_notImplemented(void);
extern void setup_pin(void);
void task_setup(void);

i32 boot_counter;

// hardware specific main setup
void
user_init() {
  // load config and setup serial interface

  setup_serial(115200);
  config_setup_txtio();
  io_puts("\r\n\r\n");
  io_puts("\r\n\r\n");
  io_puts("test io_puts...\n");
  io_printf("test io_printf...\n");

  stor_setup();
  kvs_setup();
  config_setup_global();

  // print debug info
  print_reset_info();

  setup_notImplemented();
  setup_pin();
  config_setup_wifiStation();
  config_setup_cliTcpServer();
  setup_udp();
  main_setup();
  task_setup();
  intTimer_setup();

  kvshT h;
  if ((h = kvs_open("misc", kvs_READ_WRITE))) {
    boot_counter = kvs_get_i32(h, "boot_ct", 0, 0) + 1;
    kvs_set_i32(h, "boot_ct", boot_counter);
    kvs_close(h);
  }
}
