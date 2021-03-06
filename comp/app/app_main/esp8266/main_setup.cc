#include "app_config/proj_app_cfg.h"
#include "storage/storage.h"
#include "key_value_store/kvs_wrapper.h"
#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "gpio/pin.h"

#include "txtio/inout.h"
#include "driver/uart.h"
#include "fernotron_trx/callbacks.h"
#include "utils_misc/int_types.h"
#include "app_settings/config.h"
#include "net/wifi_station_setup.h"
#include "net/ntp_client_setup.h"
#include "net/tcp_cli_server.h"
#include "app_settings/config.h"

#include "main.h"

extern void app_timerISR_setup(void);
extern void setup_serial(u32 baudrate);
extern void setup_notImplemented(void);
void task_setup(void);

i32 boot_counter;

// hardware specific main setup
void user_init() {
  // load config and setup serial interface

  setup_serial(115200);
  config_setup_txtio();
  io_puts("\r\n\r\n");
  io_puts("\r\n\r\n");
  io_puts("test io_puts...\n");
  io_printf("test io_printf...\n");

  stor_setup();
  kvs_setup();
  config_setup_txtio();
  config_setup_global();

  // print debug info
  print_reset_info();

  setup_notImplemented();
  config_setup_gpio();
  config_setup_wifiStation();
  config_setup_cliTcpServer();
 // setup_udp();
  main_setup();
  task_setup();
  app_timerISR_setup();

  kvshT h;
  if ((h = kvs_open("misc", kvs_READ_WRITE))) {
    boot_counter = kvs_get_i32(h, "boot_ct", 0, 0) + 1;
    kvs_set_i32(h, "boot_ct", boot_counter);
    kvs_close(h);
  }
}
