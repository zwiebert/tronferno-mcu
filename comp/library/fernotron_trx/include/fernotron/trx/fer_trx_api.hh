#pragma once

#include <misc/int_types.h>
#include <fernotron/trx/fer_trx_c_api.h>
#include <fernotron/astro.h>

class Fer_Trx_API {
public:
  enum MsgKind {
    MSG_TYPE_NONE, MSG_TYPE_PLAIN, MSG_TYPE_PLAIN_DOUBLE, MSG_TYPE_RTC, MSG_TYPE_TIMER
  };

public:
  virtual ~Fer_Trx_API() = default;

public:
  ////////////////////////////////////////////////////////////////
  ///////// to get events: 1. derive from this class. ////////////
  /////////                2. register your oubject here /////////
  ////////////////////////////////////////////////////////////////
  static void setup(Fer_Trx_API *derived_object);

  ///////////// setup and reinit astro data //////////////////////
  static void setup_astro(const struct cfg_astro *cfg_astro);

public:
  ////////////////////////////////////////////////////////////////
  ///////// get data of message (both rx and tx) /////////////////
  ////////////////////////////////////////////////////////////////
  u32 get_a() const;
  u8 get_g() const;
  u8 get_m() const;
  fer_if_cmd get_cmd() const;
  MsgKind get_msgKind() const;
  const void * get_raw() const; //XXX transitional code?
  const void * get_fsb() const; //XXX transitional code!
  bool is_centralUnit() const;

public:
  ////////////////////////////////////////////////////////////////
  ///////// get notified if a message was received ///////////////
  ////////////////////////////////////////////////////////////////
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
  ////////////////////////////////////////////////////////////////
  ///////// get notified if a message is about to be send ////////
  ////////////////////////////////////////////////////////////////
  virtual void event_first_message_will_be_sent() { // no repeats
  }

  virtual void event_any_message_will_be_sent() { // first + repeats
  }


public:
  ////////////////////////////////////////////////////////////////
  ///////// transmit messages ////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  static bool send_cmd(const Fer_MsgCmd &msg);
  static bool send_cmd(u32 a, u8 g, u8 m, fer_if_cmd cmd, i8 repeats, u16 delay, u16 stopDelay);

  static bool send_rtc(const Fer_MsgRtc &msg);
  static bool send_rtc(u32 a, u8 g, u8 m, time_t rtc);

  static bool send_timer(const Fer_MsgTimer &msg);
  static bool send_timer(u32 a, u8 g, u8 m, time_t rtc, const Fer_TimerData &td);

  static bool send_empty_timer(const Fer_MsgRtc &msg);
  static bool send_empty_timer(u32 a, u8 g, u8 m, time_t rtc);

public:
  ////////////////////////////////////////////////////////////////
  ///////// timer interrupt for RF transceiver ///////////////////
  ////////////////////////////////////////////////////////////////
  static void isr_sample_rx_pin(bool level);
  static void isr_handle_rx();

  static bool isr_get_tx_level();
  static void isr_handle_tx();



   ////////// implementation. ///////////////////////////////////
public:
  static void push_event(struct Fer_Trx_IncomingEvent *evt);
private:
  struct Fer_Trx_IncomingEvent *myEvt;
};

typedef Fer_Trx_API::MsgKind fer_msg_kindT;

class Fer_Trx_GPIO {
public:
  virtual ~Fer_Trx_GPIO() = default;
public:
  static void setup(Fer_Trx_GPIO *derived_object);
public:
  virtual bool get_rx() const { return false; }
  virtual void put_rx(bool level) const { }
};

