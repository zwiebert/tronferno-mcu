#include "app_config/proj_app_cfg.h"
#include "fernotron/sep/set_endpos.h"

#include <fernotron_trx/raw/fer_msg_plain.h>
#include <fernotron_trx/raw/fer_msg_attachment.h>
#include <fernotron_trx/raw/fer_msg_tx.h>
#include "fernotron_trx/raw/fer_radio_trx.h"
#include "fernotron_trx/raw/fer_rawmsg_buffer.h"
#include "fernotron_trx/raw/fer_fsb.h"

#include "gpio/pin.h"

#include "app_misc/rtc.h"
#include "txtio/inout.h"
#include "app_uout/status_output.h"
#include <fernotron_trx/fer_trx_c_api.h>

//////////////////////////////////////////////////////////////////
// DANGER ZONE
//
// if not 0, real set position up/down commands are used. They move beyond the current end position and can damage the shutter
// German: wenn nicht 0, werden hoch/runter kommandos gesendet, welche die gespeicherten Endpositionen ignorieren und den Rolladen schädigen können
//
// if 0, only normal up/down commands are used for testing.
// German: wenn 0, werden ungefährliche hoch/runter kommandos gesendet (zum Testen)
#ifndef ENABLE_SET_ENDPOS
#define ENABLE_SET_ENDPOS 1
#endif
//
// DANGER ZONE
//////////////////////////////////////////////////////////////////
#if ENABLE_SET_ENDPOS
#define SEP_DOWN fer_if_cmd_EndPosDOWN // harmful down command moving beyond current end position
#define SEP_UP fer_if_cmd_EndPosUP     // harmful up command moving beyond current end position
#else
#define SEP_DOWN fer_if_cmd_DOWN   // normal down command
#define SEP_UP fer_if_cmd_UP       // normal up command
#endif
//////////////////////////////////////////////////////////////////

static const struct TargetDesc *my_td;

void (*fer_sep_enable_disable_cb)(bool enable);

static inline void fer_sep_ENABLE_cb() {
  if (fer_sep_enable_disable_cb)
    fer_sep_enable_disable_cb(true);
}
static inline void fer_sep_DISABLE_cb() {
  if (fer_sep_enable_disable_cb)
    fer_sep_enable_disable_cb(false);
}

static fer_sbT fer_sep_fsb;
static bool fer_sep_buttons_enabled;
static bool up_pressed, down_pressed;
static fer_if_cmd fer_sep_cmd;

static time_t end_time;

#define TIMEOUT_SECS 30  // disable set position mode after being idle for N seconds
#define TIMEOUT_SET() (end_time = run_time_s() + TIMEOUT_SECS)
#define IS_TIMEOUT_REACHED() (end_time < run_time_s() || (end_time > (run_time_s() + TIMEOUT_SECS + 1)))

#define D(x)

#ifdef SEP_TWO_BUTTONS
#define BUTT_DOWN (mcu_get_buttonDownPin())
#define BUTT_UP (mcu_get_buttonUpPin())
#else
#define BUTT_DOWN (mcu_get_buttonPin() && SEP_DOWN == fer_sep_cmd)
#define BUTT_UP (mcu_get_buttonPin() && SEP_UP == fer_sep_cmd)
#endif

#define IS_BUTTON_PRESSED()  (mcu_get_buttonPin())

bool  fer_sep_is_enabled(void) {
  return fer_sep_buttons_enabled;
}

static bool 
fer_sep_send_stop(void) {
  fer_sbT * const fsb = &fer_sep_fsb;
  FER_SB_PUT_CMD(fsb, fer_if_cmd_STOP);
  fer_update_tglNibble(fsb);
  while (fer_send_msg(fsb, MSG_TYPE_PLAIN, 2)) {
    fer_update_tglNibble(fsb);
  }
  return true;
}

static bool 
fer_sep_send_down(void) {
  fer_sbT * const fsb = &fer_sep_fsb;
  FER_SB_PUT_CMD(fsb, SEP_DOWN);
  fer_update_tglNibble(fsb);
  return fer_send_msg(fsb, MSG_TYPE_PLAIN, 0);
}

static bool 
fer_sep_send_up(void) {
  fer_sbT * const fsb = &fer_sep_fsb;
  FER_SB_PUT_CMD(fsb, SEP_UP);
  fer_update_tglNibble(fsb);
  return fer_send_msg(fsb, MSG_TYPE_PLAIN, 0);
}

void 
fer_sep_disable(void) {
  if (fer_sep_buttons_enabled) {
    fer_sep_DISABLE_cb();
    fer_sep_send_stop();  // don't remove this line
    soMsg_sep_disable(*my_td);
    up_pressed = down_pressed = false;
    fer_sep_buttons_enabled = false;
  }
}

bool
fer_sep_enable(const struct TargetDesc &td, u32 a, u8 g, u8 m, fer_if_cmd cmd) {
  fer_sbT *fsb = fer_get_fsb(a,g,m,(fer_cmd)cmd);
//fer_sep_enable(const struct TargetDesc &td, fer_sbT *fsb) {
  my_td = &td;
  if (fer_sep_buttons_enabled) { // already activated
    fer_sep_disable();
    return false;
  } else if (IS_BUTTON_PRESSED()) {
    soMsg_sep_button_pressed_error(*my_td);
    return false;
  } else {

    // check for possible broadcast
    if (FER_SB_ADDR_IS_CENTRAL(fsb)
        && !((fer_memb_M1 <= FER_SB_GET_MEMB(fsb) && FER_SB_GET_MEMB(fsb) <= fer_memb_M7) && (fer_grp_G1 <= FER_SB_GET_GRP(fsb) && FER_SB_GET_GRP(fsb) <= fer_grp_G7))) {
      return false; // no broadcast allowed
    }

    // set our endpos-up/down command according to normal up/down command in fsb
    if (FER_SB_GET_CMD(fsb) == fer_if_cmd_UP) {
      fer_sep_cmd = SEP_UP;
    } else if (FER_SB_GET_CMD(fsb) == fer_if_cmd_DOWN) {
      fer_sep_cmd = SEP_DOWN;
    } else {
      return false;
    }
    soMsg_sep_enable(*my_td);
    fer_sep_fsb = *fsb;
    fer_sep_buttons_enabled = true;
    TIMEOUT_SET();
    fer_sep_ENABLE_cb();
    return true;
  }
  return false;
}

void
fer_sep_loop(void) {
  if (fer_sep_buttons_enabled) {
    const bool up_pin = BUTT_UP;
    const bool down_pin = BUTT_DOWN;

    if (up_pin && down_pin) {  // emergency stop
      fer_sep_disable();
      return;
    }

    if (!up_pressed && !down_pressed) {
      if ((up_pressed = up_pin)) {
        fer_sep_send_up();
        TIMEOUT_SET();
      } else if ((down_pressed = down_pin)) {
        fer_sep_send_down();
        TIMEOUT_SET();
      } else if (IS_TIMEOUT_REACHED()) {
        fer_sep_disable();
        return;
      }
    } else {
      if (up_pressed && !(up_pressed = up_pin)) {
        fer_sep_send_stop();
      }
      if (down_pressed && !(down_pressed = down_pin)) {
        fer_sep_send_stop();
      }
    }
  }
  return;
}
