/*
 * sep.hh
 *
 *  Created on: Jun 10, 2022
 *      Author: bertw
 */

#pragma once

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


#include "sep_auth.hh"

class Sep {
  struct Agm {
    u32 a;
    u8 g, m;

    Fer_MsgCmd make_cmd(fer_if_cmd cmd, u8 repeats = 0) const {
      return Fer_MsgCmd { .a = a, .g = g, .m = m, .cmd = cmd, .repeats = repeats };
    }
  };
public:
  bool isEnabled() const {
    return m_buttons_enabled;
  }

public:
  bool send_stop(void) {
    return Fer_Trx_API::send_cmd(m_agm.make_cmd(fer_if_cmd_STOP, 2));
  }

  bool send_down(void) {
    return Fer_Trx_API::send_cmd(m_agm.make_cmd(SEP_DOWN));
  }

  bool send_test(void) {
    return Fer_Trx_API::send_cmd(m_agm.make_cmd(fer_if_cmd_Program));
  }

  bool send_up(void) {
    return Fer_Trx_API::send_cmd(m_agm.make_cmd(SEP_UP));
  }

public:

  bool move_up(u32 auth_key, int button_timeout) {
    if (!m_sep_auth.isAuthenticated(auth_key))
      return false;
    if (!fer_sep_is_enabled())
      return false;

    m_button_time_out.set(button_timeout);
    m_sep_auth.restart_timeout();
    if (m_down_pressed) {
      send_stop();
      m_down_pressed = false;
    }
    if (!m_up_pressed) {
      m_up_pressed = true;
      send_up();
    }
    return true;
  }
  bool move_down(u32 auth_key, int button_timeout) {
    if (!m_sep_auth.isAuthenticated(auth_key))
      return false;
    if (!fer_sep_is_enabled())
      return false;

    m_sep_auth.restart_timeout();
    m_button_time_out.set(button_timeout);
    if (m_up_pressed) {
      send_stop();
      m_up_pressed = false;
    }
    if (!m_down_pressed) {
      m_down_pressed = true;
      send_down();
    }
    return true;
  }
  bool move_continue(u32 auth_key, int button_timeout) {
    if (!m_sep_auth.isAuthenticated(auth_key))
      return false;
    if (!fer_sep_is_enabled())
      return false;
    if (!(m_up_pressed || m_down_pressed)) {
      return false;
    }
    m_button_time_out.set(button_timeout);
    m_sep_auth.restart_timeout();
    return true;
  }
  bool move_stop(u32 auth_key) {
#if 0
    if (!m_sep_auth.isAuthenticated(auth_key))
      return false;
  #endif
    send_stop();
    m_up_pressed = m_down_pressed = false;
    m_button_time_out.clear();
    m_sep_auth.restart_timeout();

    return false;
  }

  bool move_test() {
    send_test();
    return true;
  }

  bool authenticate(u32 auth_key, int sep_mode_timeout_secs, int button_timeout_secs) {
    m_sep_auth.authenticate(auth_key, sep_mode_timeout_secs, button_timeout_secs);
    fer_sep_ENABLE_cb();
    return true;
  }

  bool deauthenticate(u32 auth_key) {
    if (!m_sep_auth.deauthenticate(auth_key))
      return false;
    uoApp_publish_fer_authState( { .auth_terminated = 1 });
    return true;
  }

  void disable(void) {
    if (m_buttons_enabled) {
      fer_sep_DISABLE_cb();
      send_stop();  // don't remove this line
      m_up_pressed = m_down_pressed = false;
      m_buttons_enabled = false;
    }
  }

  bool enable(u32 auth_key, const u32 a, const u8 g, const u8 m) {
    if (!m_sep_auth.isAuthenticated(auth_key))
      return false;
    if (fer_sep_is_enabled()) {
      return false;
    }

    // check for possible broadcast  XXX: should we also forbid any non motor address (FER_ADDR_TYPE_Receiver)?
    if (FER_U32_TEST_TYPE(a, FER_ADDR_TYPE_CentralUnit) && !((1 <= m && m <= 7) && (1 <= g && g <= 7))) {
      return false; // no broadcast allowed
    }

    m_agm.a = a;
    m_agm.g = g;
    m_agm.m = m;

    m_buttons_enabled = true;
    m_sep_auth.restart_timeout();
    fer_sep_ENABLE_cb();
    return true;
  }

public:
  void work_loop(void) {
    if (m_button_time_out.isTimeoutReached()) {
      send_stop();
    }

    if (!m_sep_auth.work_loop())
      return;

    if (m_sep_auth.hasTimedOut()) {
      send_stop();
      disable();
      return;
    }
  }

private:
  Sep_Auth m_sep_auth;
  Time_Out_Secs m_button_time_out;
  Agm m_agm;
  bool m_buttons_enabled = false;
  bool m_up_pressed = false;
  bool m_down_pressed = false;
};
