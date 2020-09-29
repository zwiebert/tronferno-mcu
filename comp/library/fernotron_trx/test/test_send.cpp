#include <unity.h>
#include <fernotron/trx/fer_trx_api.hh>
#include "fernotron/trx/raw/fer_msg_tx.h"

int first_msg_count, any_msg_count;
Fer_MsgPlainCmd first_msg_cmd, any_msg_cmd;

static class FA : public Fer_Trx_API {

  virtual void event_first_message_will_be_sent() { // no repeats
    ++first_msg_count;
    first_msg_cmd = Fer_MsgPlainCmd { get_a(), get_g(), get_m(), get_cmd() };
  }

  virtual void event_any_message_will_be_sent() { // first + repeats
    ++any_msg_count;
    any_msg_cmd = Fer_MsgPlainCmd { get_a(), get_g(), get_m(), get_cmd() };
  }

} MyFa;



void test_send() {

  Fer_MsgPlainCmd m { 0x804243, 5, 8, fer_if_cmd_STOP };

  Fer_Trx_API::send_cmd(m.a, m.g, m.m, m.cmd );
  TEST_ASSERT_EQUAL(first_msg_count, 0);
  fer_tx_loop();
  TEST_ASSERT_EQUAL(first_msg_count, 1);
  TEST_ASSERT_EQUAL(any_msg_count, 1);
  TEST_ASSERT_EQUAL_MEMORY(&m, &first_msg_cmd, sizeof m);
  TEST_ASSERT_EQUAL_MEMORY(&m, &any_msg_cmd, sizeof m);

}





TEST_CASE("send fernotron message", "[fernotron]")
{

   test_send();
}






void setUp() {
  Fer_Trx_API::setup(&MyFa);

}
