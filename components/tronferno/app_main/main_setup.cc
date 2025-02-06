#include "app_misc/rtc.h"
#include "fernotron/fer_main.h"
#include "app_uout/so_types.h"
#include "app_uout/so_msg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include <net_http_server/comp_glue.hh>

void publish_ipAddress(); // find header for it
void ntpApp_setup(void);

void main_setup() {
  rtc_setup();

  config_setup_astro();
  UoutWriterConsole td { SO_TGT_CLI };
  soMsg_fw_start_msg_print(td);

  config_setup_cc1101();
#ifdef CONFIG_APP_USE_REPEATER
  config_setup_repeater();
#endif
  fer_main_setup({ config_read_item(CB_CUID, (uint32_t)CONFIG_APP_FER_CENTRAL_UNIT_ID), config_read_item(CB_USED_MEMBERS, (uint32_t)CONFIG_APP_FER_GM_USE)});
}

void main_setup_ip_dependent() {

  publish_ipAddress();

  static int once;
  if (!once && !once++) {
#ifdef CONFIG_APP_USE_NTP
    ntpApp_setup();
#endif
#ifdef CONFIG_APP_USE_MQTT
    config_setup_mqttAppClient();
#endif
#ifdef CONFIG_APP_USE_TCPS_TASK
    config_ext_setup_cliTcpServer();
#endif
#ifdef CONFIG_APP_USE_HTTP
    config_setup_httpServer();
#endif

  }
}
