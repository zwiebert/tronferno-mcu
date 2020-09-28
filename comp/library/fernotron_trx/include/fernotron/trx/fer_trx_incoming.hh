#pragma once

#include <misc/int_types.h>
#include <fernotron/trx/fer_msg_send.hh>

class Fer_Trx_IncomingMsg {
public:
  enum MsgKind {
    MSG_TYPE_NONE, MSG_TYPE_PLAIN, MSG_TYPE_PLAIN_DOUBLE, MSG_TYPE_RTC, MSG_TYPE_TIMER
  };

public:
  virtual ~Fer_Trx_IncomingMsg() = default;
  static void setup(Fer_Trx_IncomingMsg *derived_object);

public:
  u32 get_a() const;
  u8 get_g() const;
  u8 get_m() const;
  fer_if_cmd get_cmd() const;
  MsgKind get_msgKind() const;
  const void * get_raw() const; //XXX transitional code?
  const void * get_fsb() const; //XXX transitional code!
  bool is_centralUnit() const;

public:
  virtual void event_plain_message_was_received() {
  }
  virtual void event_plain_double_message_was_received() {
  }
  virtual void event_rtc_message_was_received() {
  }
  virtual void event_timer_message_was_received() {
  }
  virtual void event_any_message_was_received() {
  }

public:
  virtual void event_first_message_will_be_sent() { // no repeats
  }

  virtual void event_any_message_will_be_sent() { // first + repeats
  }


public:
  static void push_event(struct Fer_Trx_IncomingEvent *evt);


private:
  struct Fer_Trx_IncomingEvent *myEvt;
};

typedef Fer_Trx_IncomingMsg::MsgKind fer_msg_kindT;
