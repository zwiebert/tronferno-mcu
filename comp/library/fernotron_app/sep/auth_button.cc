#include "auth_button.hh"
//#include "app_uout/status_output.h"
#include "fernotron_uout/fer_uo_publish.h"

constexpr unsigned BUTTON_PRE_TEST_SECS = 5;

Auth_Button::Auth_Button(get_gpio_funT get_gpio_fun, char tag) :
    m_get_gpio_fun(get_gpio_fun), m_tag(tag) {

}

void Auth_Button::log_in(unsigned button_timeout_secs) {
  if (isLoggedIn()) {
    uoApp_publish_fer_sepState( { .auth_success = 1 });
    return;
  }
  (void)(*m_get_gpio_fun)(); // clear previously pressed button event
  m_button_error = false;
  m_button_pre_test_timeout.set(BUTTON_PRE_TEST_SECS);
  m_auth_button_timeout.set(button_timeout_secs);
}

void Auth_Button::log_out() {
  m_button_pre_test_timeout.clear();
  m_auth_button_timeout.clear();
  m_logged_in = false;

}

void Auth_Button::work_loop() {
  if (isLoggedIn())
    return;

  if (m_button_error)
    return;

  if (m_button_pre_test_timeout.isTimeoutRunning()) {
    if ((*m_get_gpio_fun)()) {
      m_button_error = true;
      uoApp_publish_fer_sepState( { .auth_button_error = 1 });
    }
    (void) m_button_pre_test_timeout.isTimeoutReached();
    return;
  }

  if (m_auth_button_timeout.isTimeoutReached()) {
    log_out();
    m_timed_out = true;
    uoApp_publish_fer_sepState( { .auth_button_timeout = 1 });
    return;
  }

  if (m_auth_button_timeout.isTimeoutRunning()) {
    if ((*m_get_gpio_fun)()) {
      m_logged_in = true;
      uoApp_publish_fer_sepState( { .auth_success = 1 });
      return;
    }
  }
}
