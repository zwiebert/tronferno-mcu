#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif
#include "fernotron_trx/raw/fer_msg_tx.h"
#include <fernotron_trx/fer_trx_api.hh>
#include <fernotron_trx/timer_data.h>



void test_put_weekly() {
  Fer_TimerData tda;
  TEST_ASSERT_FALSE(tda.hasWeekly());

  {
    bool res = tda.putWeekly("--++++++");
    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_FALSE(tda.hasWeekly());
  }

  {
    bool res = tda.putWeekly("0700-++++++");
    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_TRUE(tda.hasWeekly());
  }

  {
    bool res = tda.putWeekly("0700-+++++");
    TEST_ASSERT_FALSE(res);
    TEST_ASSERT_TRUE(tda.hasWeekly());
  }

  {
    bool res = tda.putWeekly("null");
    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_FALSE(tda.hasWeekly());
  }
}

void test_put_daily() {
  Fer_TimerData tda;
  TEST_ASSERT_FALSE(tda.hasDaily());

  {
    bool res = tda.putDaily("--");
    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_FALSE(tda.hasDaily());
  }
  {
    bool res = tda.putDaily("0730-");
    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_TRUE(tda.hasDaily());
  }
  {
    bool res = tda.putDaily("07302230");
    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_TRUE(tda.hasDaily());
  }
  {
    bool res = tda.putDaily("2730-");
    TEST_ASSERT_FALSE(res);
    TEST_ASSERT_TRUE(tda.hasDaily());
  }
}



TEST_CASE("tx: repeats and queue", "[fernotron]")
{
  test_put_weekly();
  test_put_daily();
}

#ifdef TEST_HOST
//volatile bool fer_tx_messageToSend_isReady;
//volatile uint32_t run_time_s_;
//volatile uint32_t run_time_ts_;
uint32_t run_time_ts() { return 0; }
uint32_t run_time_s() { return 0; }
bool mcu_get_rxPin() { return false; }
void mcu_delay_us(uint32_t us) {}
#endif
