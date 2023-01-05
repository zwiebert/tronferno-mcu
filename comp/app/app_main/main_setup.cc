#include "app_misc/rtc.h"
#include "fernotron/fer_main.h"
#include "app_uout/so_types.h"
#include "app_uout/so_msg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"


void main_setup() {
  rtc_setup();

  config_setup_astro();
  TargetDescCon td { SO_TGT_CLI };
  soMsg_fw_start_msg_print(td);

  config_setup_cc1101();
  config_setup_repeater();
  fer_main_setup({ config_read_item(CB_CUID, (uint32_t)CONFIG_APP_FER_CENTRAL_UNIT_ID), config_read_item(CB_USED_MEMBERS, (uint32_t)CONFIG_APP_FER_GM_USE)});
}
