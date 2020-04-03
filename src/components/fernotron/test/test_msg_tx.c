#include "unity.h"
#include "fer_msg_tx.h"



void test_txq_repeats() {

  fsbT fsb_ = { .data =  {0x80, 0xab, 0xcd, 0x19, 0x24 } };
  fsbT *fsb = &fsb_;

  TEST_ASSERT_EQUAL(0, ftx_get_msgPendingCount());
  TEST_ASSERT_FALSE(ftx_messageToSend_isReady);

  fer_send_msg(fsb, MSG_TYPE_PLAIN, 2);
  TEST_ASSERT_EQUAL(1, ftx_get_msgPendingCount());

  fer_send_msg(fsb, MSG_TYPE_PLAIN, 1);
  TEST_ASSERT_EQUAL(2, ftx_get_msgPendingCount());

  fer_tx_loop();
  TEST_ASSERT_EQUAL(2, ftx_get_msgPendingCount());
  TEST_ASSERT_TRUE(ftx_messageToSend_isReady);

  ftx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(2, ftx_get_msgPendingCount());
  TEST_ASSERT_TRUE(ftx_messageToSend_isReady);

  ftx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(1, ftx_get_msgPendingCount());
  TEST_ASSERT_TRUE(ftx_messageToSend_isReady);

  ftx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(1, ftx_get_msgPendingCount());
  TEST_ASSERT_TRUE(ftx_messageToSend_isReady);

  ftx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(0, ftx_get_msgPendingCount());
  TEST_ASSERT_TRUE(ftx_messageToSend_isReady);


  fer_send_msg_with_stop(fsb, 0, 50, 0);
  TEST_ASSERT_EQUAL(2, ftx_get_msgPendingCount());

  ftx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(1, ftx_get_msgPendingCount());
  TEST_ASSERT_TRUE(ftx_messageToSend_isReady);

}


TEST_CASE("tx: repeats and queue", "[fernotron]")
{
  test_txq_repeats();
}

#ifdef TEST_HOST
volatile bool ftx_messageToSend_isReady;
volatile u16 ftx_messageToSend_wordCount;
volatile uint32_t run_time_s_;
volatile uint32_t run_time_ts_;
uint32_t run_time_ts() { return 0; }
uint32_t run_time_s() { return 0; }
void mcu_delay_us(u32 us) {}
#endif
