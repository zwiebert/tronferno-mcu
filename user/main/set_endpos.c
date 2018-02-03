#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "rtc.h"
#include "all.h"
#include "fer.h"
#include "rtc.h"

//////////////////////////////////////////////////////////////////
// DANGER ZONE
//
// if not 0, real set position up/down commands are used. They move beyond the current end position and can damage the shutter
// German: wenn nicht 0, werden hoch/runter kommandos gesendet, welche die gespeicherten Endpositionen ignorieren und den Rolladen schädigen können
//
// if 0, only normal up/down commands are used for testing.
// German: wenn 0, werden ungefährliche hoch/runter kommandos gesendet (zum Testen)
#define ENABLE_SET_ENDPOS 1
//
// DANGER ZONE
//////////////////////////////////////////////////////////////////
#if ENABLE_SET_ENDPOS
#define SEP_DOWN fer_cmd_EndPosDOWN // harmful down command moving beyond current end position
#define SEP_UP fer_cmd_EndPosUP     // harmful up command moving beyond current end position
#else
#define SEP_DOWN fer_cmd_DOWN   // normal down command
#define SEP_UP fer_cmd_UP       // normal up command
#endif
//////////////////////////////////////////////////////////////////

static fer_sender_basic sep_fsb;
static bool sep_buttons_enabled;
static bool up_pressed, down_pressed;
static fer_cmd sep_cmd;

static time_t end_time;

#define TIMEOUT_SECS 30  // disable set position mode after being idle for N seconds
#define TIMEOUT_SET() (end_time = run_time(NULL) + TIMEOUT_SECS)
#define IS_TIMEOUT_REACHED() (end_time < run_time(NULL) || (end_time > (run_time(NULL) + TIMEOUT_SECS + 1)))

/*
 if (init_seconds > 0) {
 end_time = run_time(NULL) + init_seconds;
 last_received_sender.data[0] = 0;
 cuas_msgid = msgid;
 */

#define D(x) x

#if 0
#define BUTT_DOWN (mcu_get_buttonDownPin())
#define BUTT_UP (mcu_get_buttonUpPin())
#else
#define BUTT_DOWN (mcu_get_buttonPin() && SEP_DOWN == sep_cmd)
#define BUTT_UP (mcu_get_buttonPin() && SEP_UP == sep_cmd)
#endif

#define IS_BUTTON_PRESSED()  (mcu_get_buttonPin())

bool ICACHE_FLASH_ATTR sep_is_enabled(void) {
  return sep_buttons_enabled;
}

static bool ICACHE_FLASH_ATTR
sep_send_stop(void) {
  fer_sender_basic * const fsb = &sep_fsb;
  fsb->repeats = 2;
  FSB_PUT_CMD(fsb, fer_cmd_STOP);
  fer_update_tglNibble(fsb);
  while (fer_send_msg(fsb, MSG_TYPE_PLAIN)) {
    fer_update_tglNibble(fsb);
  }
  return true;
}

static bool ICACHE_FLASH_ATTR
sep_send_down(void) {
  fer_sender_basic * const fsb = &sep_fsb;
  fsb->repeats = 0;
  FSB_PUT_CMD(fsb, SEP_DOWN);
  fer_update_tglNibble(fsb);
  return fer_send_msg(fsb, MSG_TYPE_PLAIN);
}

static bool ICACHE_FLASH_ATTR
sep_send_up(void) {
  fer_sender_basic * const fsb = &sep_fsb;
  fsb->repeats = 0;
  FSB_PUT_CMD(fsb, SEP_UP);
  fer_update_tglNibble(fsb);
  return fer_send_msg(fsb, MSG_TYPE_PLAIN);
}

void ICACHE_FLASH_ATTR
sep_disable(void) {
  if (sep_buttons_enabled) {
    sep_send_stop();  // don't remove this line
    D(io_puts("sep disable\n"));
    up_pressed = down_pressed = false;
    sep_buttons_enabled = false;
    recv_lockBuffer(false);
  }
}

bool ICACHE_FLASH_ATTR
sep_enable(fer_sender_basic *fsb) {
  if (sep_buttons_enabled) { // already activated
    sep_disable();
    return false;
  } else if (IS_BUTTON_PRESSED()) {
    io_puts("error: hardware button is pressed\n");
    return false;
  } else {

    // check for possible broadcast
    if (FSB_ADDR_IS_CENTRAL(fsb)
        && !((fer_memb_M1 <= FSB_GET_MEMB(fsb) && FSB_GET_MEMB(fsb) <= fer_memb_M7) && (fer_grp_G1 <= FSB_GET_GRP(fsb) && FSB_GET_GRP(fsb) <= fer_grp_G7))) {
      return false; // no broadcast allowed
    }

    // set our endpos-up/down command according to normal up/down command in fsb
    if (FSB_GET_CMD(fsb) == fer_cmd_UP) {
      sep_cmd = SEP_UP;
    } else if (FSB_GET_CMD(fsb) == fer_cmd_DOWN) {
      sep_cmd = SEP_DOWN;
    } else {
      return false;
    }

    D(io_puts("sep enable\n"));
    sep_fsb = *fsb;
    sep_buttons_enabled = true;
    recv_lockBuffer(true);
    TIMEOUT_SET();
    return true;
  }
  return false;
}

bool ICACHE_FLASH_ATTR
sep_loop(void) {
  if (sep_buttons_enabled && !is_sendMsgPending) {
    const bool up_pin = BUTT_UP;
    const bool down_pin = BUTT_DOWN;

    if (up_pin && down_pin) {  // emergency stop
      sep_disable();
      return false;
    }

    if (!up_pressed && !down_pressed) {
      if ((up_pressed = up_pin)) {
        sep_send_up();
        TIMEOUT_SET();
      } else if ((down_pressed = down_pin)) {
        sep_send_down();
        TIMEOUT_SET();
      } else if (IS_TIMEOUT_REACHED()) {
        sep_disable();
        return false;
      }
    } else {
      if (up_pressed && !(up_pressed = up_pin)) {
        sep_send_stop();
      }
      if (down_pressed && !(down_pressed = down_pin)) {
        sep_send_stop();
      }
    }
  }
}
