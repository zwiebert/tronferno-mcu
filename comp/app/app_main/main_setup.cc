#include "app_misc/rtc.h"
#include "fernotron/fer_main.h"
#include "app_uout/so_types.h"
#include "app_uout/so_msg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "app_settings/config_defaults.h"

void main_setup() {
  rtc_setup();

  config_setup_astro();
  TargetDescCon td { SO_TGT_CLI };
  soMsg_fw_start_msg_print(td);

  config_setup_cc1101();
  fer_main_setup({ config_read_item(CB_CUID, MY_FER_CENTRAL_UNIT_ID), config_read_item(CB_USED_MEMBERS, MY_FER_GM_USE)});
}
