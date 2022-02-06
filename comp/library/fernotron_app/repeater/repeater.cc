#include <fernotron/repeater/repeater.h>
#include "app_config/proj_app_cfg.h"

#include <fernotron_trx/fer_trx_api.hh>
#include <fernotron_trx/raw/fer_msg_plain.h>
#include <fernotron_trx/raw/fer_msg_tx.h>
#include <fernotron_trx/raw/fer_fsb.h>
#include "../fer_msg_tx_queue.h"

#include <utils_time/run_time.h>

const unsigned REP_DELAY_TS = 10;

static u32 *known_ids;
static int known_ids_count;

static bool ferRep_isKnownId(u32 id) {

  for (int i = 0; i < known_ids_count; ++i) {
    if (known_ids[i] == id)
      return true;
  }
  return false;
}

static bool ferRep_isKnownSender(const struct Fer_Cmd *cmd) {

  if (!(FER_RB_ADDR_IS_PLAIN(cmd) || FER_RB_ADDR_IS_SUNSENS(cmd)))
    return false;

  return ferRep_isKnownId(FER_RB_GET_DEVID(cmd));

}

bool ferRep_repeatCommand(struct Fer_Cmd cmd) {
  if (!ferRep_isKnownSender(&cmd))
    return false;

  struct sf
  msg { .fsb = { .sd = cmd }, .when_to_transmit_ts = (u32) get_now_time_ts() + REP_DELAY_TS, .mt = MSG_TYPE_PLAIN, .repeats = 1, .sent_ct = 1 };

  fer_tx_pushMsg (&msg);
  fer_tx_loop();

  return true;
}

bool ferRep_setup(const char *senders) {
  known_ids_count = strlen(senders) / 6;
  if (!known_ids_count) {
    free(known_ids);
    known_ids = 0;
    return true;
  }
  void *storage = realloc(known_ids, known_ids_count * sizeof(*known_ids));
  if (!storage)
    return false;
  known_ids = static_cast<u32*>(storage);

  for (int i = 0; i < known_ids_count; ++i) {
    char id[7];
    memcpy(id, senders + i * 6, 6);
    id[6] = '\0';
    known_ids[i] = strtol(id, 0, 16);
  }
  return true;
}
