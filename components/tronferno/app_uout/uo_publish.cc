/**
 * @file uo_publish.cc
 * @author bertw
 * @brief publish/broadcast messages to targets (websocket, terminal, ...)
 * @date 2024-04-30
 * 
 */

#include <uout/uo_callbacks.h>

void uoApp_publish_errorMask(unsigned error_bits) {
  uo_flagsT flags;
  flags.evt.gen_app_error_message = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {

    char sj_buf[256];
    UoutBuilderJson sj = { sj_buf, sizeof sj_buf };

    if (sj.open_root_object("tfmcu")) {
      if(sj.add_object("mcu")) {
        sj.add_key_value_pair_d("error-bits", error_bits);
        sj.close_object();
      }
      sj.close_root_object();

      uoCb_publish(idxs, sj.get_json(), flags);
    }
  }
}
