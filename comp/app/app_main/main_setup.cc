#include "app/rtc.h"
#include "fernotron/fer_main.h"
#include "app/uout/so_types.h"
#include "app/uout/so_msg.h"
#include "app/settings/config.h"

void main_setup() {
  rtc_setup();

  config_setup_astro();
  TargetDescCon td { SO_TGT_CLI };
  soMsg_fw_start_msg_print(td);

  fer_setup(fer_configT {C.fer_centralUnitID});
}
