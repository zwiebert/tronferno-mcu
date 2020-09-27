#include "unity.h"
#include "fernotron/fer_msg_tx.h"



void test_txq_repeats() {

  fer_sbT fsb_ = { .data =  {0x80, 0xab, 0xcd, 0x19, 0x24 } };
  fer_sbT *fsb = &fsb_;

  TEST_ASSERT_EQUAL(0, fer_tx_get_msgPendingCount());
  TEST_ASSERT_FALSE(fer_tx_messageToSend_isReady);

  fer_send_msg(fsb, MSG_TYPE_PLAIN, 2);
  TEST_ASSERT_EQUAL(1, fer_tx_get_msgPendingCount());

  fer_send_msg(fsb, MSG_TYPE_PLAIN, 1);
  TEST_ASSERT_EQUAL(2, fer_tx_get_msgPendingCount());

  fer_tx_loop();
  TEST_ASSERT_EQUAL(2, fer_tx_get_msgPendingCount());
  TEST_ASSERT_TRUE(fer_tx_messageToSend_isReady);

  fer_tx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(2, fer_tx_get_msgPendingCount());
  TEST_ASSERT_TRUE(fer_tx_messageToSend_isReady);

  fer_tx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(1, fer_tx_get_msgPendingCount());
  TEST_ASSERT_TRUE(fer_tx_messageToSend_isReady);

  fer_tx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(1, fer_tx_get_msgPendingCount());
  TEST_ASSERT_TRUE(fer_tx_messageToSend_isReady);

  fer_tx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(0, fer_tx_get_msgPendingCount());
  TEST_ASSERT_TRUE(fer_tx_messageToSend_isReady);


  fer_send_msg_with_stop(fsb, 0, 50, 0);
  TEST_ASSERT_EQUAL(2, fer_tx_get_msgPendingCount());

  fer_tx_messageToSend_isReady = false;
  fer_tx_loop();
  TEST_ASSERT_EQUAL(1, fer_tx_get_msgPendingCount());
  TEST_ASSERT_TRUE(fer_tx_messageToSend_isReady);

  fer_tx_messageToSend_isReady = false;
  fer_tx_loop();
}


TEST_CASE("tx: repeats and queue", "[fernotron]")
{
  test_txq_repeats();
}

#ifdef TEST_HOST
//volatile bool fer_tx_messageToSend_isReady;
//volatile u16 fer_tx_messageToSend_wordCount;
//volatile uint32_t run_time_s_;
//volatile uint32_t run_time_ts_;
uint32_t run_time_ts() { return 0; }
uint32_t run_time_s() { return 0; }
bool mcu_get_rxPin() { return false; }
void mcu_delay_us(u32 us) {}
#endif
