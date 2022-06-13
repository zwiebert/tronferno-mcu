/* \file: auth_button.cc
 * \brief: authenticate user by physical hardware button press
 */

#pragma once

#include <utils_time/time_out_secs.hh>

class Auth_Button {
public:
  typedef bool (*get_gpio_funT)();
  Auth_Button(get_gpio_funT get_gpio_fun, char tag = 'a');
  void log_in(unsigned button_timeout_secs);
  void log_out();
  void work_loop();

public:
  bool isLoggedIn() const {
    return m_logged_in;
  }

private:
  Time_Out_Secs m_button_pre_test_timeout; ///< check for GPIO have pullup R and is n.o.
  Time_Out_Secs m_auth_button_timeout;
  const get_gpio_funT m_get_gpio_fun;
  char m_tag;
  bool m_logged_in :1 = false;
  bool m_timed_out :1 = false;
  bool m_button_error :1 = false;
};
