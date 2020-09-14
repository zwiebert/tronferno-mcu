#include "app/rtc.h"
#include "fernotron/fer_main.h"
#include "app/uout/status_output.h"
#include "app/settings/config.h"

void main_setup() {
  rtc_setup();

  config_setup_astro();
  soMsg_fw_start_msg_print();

  fer_setup(fer_configT {C.fer_centralUnitID});
}
