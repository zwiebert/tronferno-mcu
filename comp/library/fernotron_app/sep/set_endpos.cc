#include "app_config/proj_app_cfg.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron_trx/fer_trx_api.hh"
#include "app_uout/status_output.h"
#include <fernotron_trx/fer_trx_c_api.h>

#include "gpio/pin.h"

#include <utils_time/run_time.h>
#include "txtio/inout.h"

//////////////////////////////////////////////////////////////////
// DANGER ZONE
//
// if not 0, real set position up/down commands are used. They move beyond the current end position and can damage the shutter
// German: wenn nicht 0, werden hoch/runter kommandos gesendet, welche die gespeicherten Endpositionen ignorieren und den Rolladen schädigen können
//
// if 0, only normal up/down commands are used for testing.
// German: wenn 0, werden ungefährliche hoch/runter kommandos gesendet (zum Testen)
#define ENABLE_SET_ENDPOS 0

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

static u32 sep_auth_key;

void (*fer_sep_enable_disable_cb)(bool enable);

static inline void fer_sep_ENABLE_cb() {
  if (fer_sep_enable_disable_cb)
    fer_sep_enable_disable_cb(true);
}
static inline void fer_sep_DISABLE_cb() {
  if (fer_sep_enable_disable_cb)
    fer_sep_enable_disable_cb(false);
}

class Time_Out {
public:
  bool isTimeoutReached() {
    if (!end_time)
      return false;

    if (end_time < run_time_s()) {
      end_time = 0;
      return true;
    }
    return false;
  }
  Time_Out() {
  }
  Time_Out(int secs) :
      end_time(run_time_s() + secs) {
  }
private:
  time_t end_time = 0;
};

static struct {
  u32 a;
  u8 g, m;

  Fer_MsgCmd make_cmd(fer_if_cmd cmd, u8 repeats = 0) const {
    return Fer_MsgCmd { .a = a, .g = g, .m = m, .cmd = cmd, .repeats = repeats };
  }

} ourAgm;

static Time_Out sep_mode_time_out, sep_button_time_out;

static bool fer_sep_buttons_enabled;
static bool up_pressed, down_pressed;
static fer_if_cmd fer_sep_cmd;

static time_t end_time, sep_button_end_time;
static i16 sep_mode_timeout_secs;
static i16 sep_button_timeout_secs;

#define D(x)

#ifdef SEP_TWO_BUTTONS
#define BUTT_DOWN (mcu_get_buttonDownPin())
#define BUTT_UP (mcu_get_buttonUpPin())
#else
#define BUTT_DOWN (mcu_get_buttonPin() && SEP_DOWN == fer_sep_cmd)
#define BUTT_UP (mcu_get_buttonPin() && SEP_UP == fer_sep_cmd)
#endif

#define IS_BUTTON_PRESSED()  (mcu_get_buttonPin())

bool fer_sep_is_auth_key_valid(u32 auth_key) {
#if 1
  if (!sep_auth_key)
    return false;
#endif
  if (sep_auth_key != auth_key)
    return false;

  return true;
}

bool fer_sep_is_enabled(void) {
  return fer_sep_buttons_enabled;
}

static bool fer_sep_send_stop(void) {
  return Fer_Trx_API::send_cmd(ourAgm.make_cmd(fer_if_cmd_STOP, 2));
}

static bool fer_sep_send_down(void) {
  return Fer_Trx_API::send_cmd(ourAgm.make_cmd(SEP_DOWN));
}

static bool fer_sep_send_test(void) {
  return Fer_Trx_API::send_cmd(ourAgm.make_cmd(fer_if_cmd_Program));
}

static bool fer_sep_send_up(void) {
  return Fer_Trx_API::send_cmd(ourAgm.make_cmd(SEP_UP));
}

void fer_sep_disable(void) {
  if (fer_sep_buttons_enabled) {
    fer_sep_DISABLE_cb();
    fer_sep_send_stop();  // don't remove this line
    up_pressed = down_pressed = false;
    fer_sep_buttons_enabled = false;
  }
}

bool fer_sep_enable(const struct TargetDesc &td, u32 auth_key, const u32 a, const u8 g, const u8 m, int mode_timeout) {
  if (!fer_sep_is_auth_key_valid(auth_key))
    return false;
  if (fer_sep_is_enabled()) {
    return false;
  }

  // check for possible broadcast  XXX: should we also forbid any non motor address (FER_ADDR_TYPE_Receiver)?
  if (FER_U32_TEST_TYPE(a, FER_ADDR_TYPE_CentralUnit) && !((1 <= m && m <= 7) && (1 <= g && g <= 7))) {
    return false; // no broadcast allowed
  }

  ourAgm.a = a;
  ourAgm.g = g;
  ourAgm.m = m;

  fer_sep_buttons_enabled = true;
  sep_mode_timeout_secs = mode_timeout;
  sep_mode_time_out = Time_Out(mode_timeout);
  fer_sep_ENABLE_cb();
  return true;
}

void fer_sep_loop(void) {
  if (sep_button_time_out.isTimeoutReached()) {
    fer_sep_send_stop();
  }

  if (sep_mode_time_out.isTimeoutReached()) {
    fer_sep_send_stop();
    fer_sep_disable();
  }

  return;
}

bool fer_sep_move_up(u32 auth_key, int button_timeout) {
  if (!fer_sep_is_auth_key_valid(auth_key))
    return false;
  if (!fer_sep_is_enabled())
    return false;

  sep_button_time_out = Time_Out(button_timeout);
  sep_mode_time_out = Time_Out(sep_mode_timeout_secs);
  if (down_pressed) {
    fer_sep_send_stop();
    down_pressed = false;
  }
  if (!up_pressed) {
    up_pressed = true;
    fer_sep_send_up();
  }
  return true;
}
bool fer_sep_move_down(u32 auth_key, int button_timeout) {
  if (!fer_sep_is_auth_key_valid(auth_key))
    return false;
  if (!fer_sep_is_enabled())
    return false;

  sep_mode_time_out = Time_Out(sep_mode_timeout_secs);
  sep_button_time_out = Time_Out(button_timeout);
  if (up_pressed) {
    fer_sep_send_stop();
    up_pressed = false;
  }
  if (!down_pressed) {
    down_pressed = true;
    fer_sep_send_down();
  }
  return true;
}
bool fer_sep_move_continue(u32 auth_key, int button_timeout) {
  if (!fer_sep_is_auth_key_valid(auth_key))
    return false;
  if (!fer_sep_is_enabled())
    return false;
  if (!(up_pressed || down_pressed)) {
    return false;
  }
  sep_button_time_out = Time_Out(button_timeout);
  sep_mode_time_out = Time_Out(sep_mode_timeout_secs);
  return true;
}
bool fer_sep_move_stop(u32 auth_key) {
#if 0
  if (!fer_sep_is_auth_key_valid(auth_key))
    return false;
#endif
  fer_sep_send_stop();
  up_pressed = down_pressed = false;
  sep_button_time_out = Time_Out();
  sep_mode_time_out = Time_Out(sep_mode_timeout_secs);

  return false;
}


bool fer_sep_move_test() {
  fer_sep_send_test();
  return true;
}


bool fer_sep_authenticate(const struct TargetDesc &td, u32 auth_key, int sep_mode_timeout_secs) {
  sep_auth_key = auth_key;

  return true;
}
