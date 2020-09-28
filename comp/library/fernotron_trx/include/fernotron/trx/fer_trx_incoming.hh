#pragma once

#include <misc/int_types.h>

class Fer_Trx_IncomingMsg {

public:
  virtual ~Fer_Trx_IncomingMsg() = default;
  static void setup(Fer_Trx_IncomingMsg *derived_object);

public:
  u32 get_a() const;
  u8 get_g() const;
  u8 get_m() const;


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
  static void push_event(struct Fer_Trx_IncomingEvent *evt);

private:
  struct Fer_Trx_IncomingEvent *myEvt;
};
