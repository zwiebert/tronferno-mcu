/*
 * Sep_Auth.hh
 *
 *  Created on: Jun 10, 2022
 *      Author: bertw
 */

#pragma once

#include "auth_button.hh"

class Sep_Auth {

public:
  Sep_Auth() :
      m_auth_button(mcu_get_buttonPin, 'e') {

  }

public:
  void restart_timeout() {
    m_auth_timeout.set(m_auth_timeout_secs);
  }
  //sep_auth.restart_timeout();
public:
  bool authenticate(u32 auth_key, unsigned auth_timeout_secs = fer_sep_TIMEOUT, unsigned auth_button_timeout_secs = SEP_AUTH_BUTTON_TIMEOUT_SECS) {
    m_has_timed_out = false;
    setKey(auth_key);
    m_auth_timeout_secs = auth_timeout_secs;
    m_auth_button.log_in(auth_button_timeout_secs);
    fer_sep_ENABLE_cb();
    return true;
  }

  bool deauthenticate(u32 auth_key) {
    if (!isKeyMatched(auth_key))
      return false;
    m_auth_button.log_out();
    fer_sep_DISABLE_cb();
    return true;
  }

  bool work_loop() {
    if (!m_auth_button.isLoggedIn())
      m_auth_button.work_loop();
    if (!m_auth_button.isLoggedIn())
      return false;

    if (m_auth_timeout.isTimeoutReached()) {
      m_has_timed_out = true;
    }
    return true;
  }

  bool hasTimedOut() const {

    return m_has_timed_out;
  }

  bool isKeyMatched(u32 auth_key) const {
    if (!m_auth_key)
      return false;
    if (auth_key != m_auth_key)
      return false;
    return true;
  }

  bool isAuthenticated(u32 auth_key) const {
    if (!isKeyMatched(auth_key))
      return false;

    if (!m_auth_button.isLoggedIn())
      return false;
    return true;
  }
private:
  void setKey(u32 auth_key) {
    m_auth_key = auth_key;
  }
private:
  u32 m_auth_key = 0;
  Auth_Button m_auth_button;
  Time_Out_Secs m_auth_timeout;
  i16 m_auth_timeout_secs = 0;
  bool m_has_timed_out = false;
};
