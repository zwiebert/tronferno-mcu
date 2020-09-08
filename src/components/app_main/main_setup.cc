#include "app/rtc.h"
#include "fernotron/fer_main.h"
#include "userio_app/status_output.h"
#include "config/config.h"

void main_setup() {
  rtc_setup();

  config_setup_astro();
  so_output_message(SO_FW_START_MSG_PRINT, 0);

  fer_setup(fer_configT {C.fer_centralUnitID});
}
