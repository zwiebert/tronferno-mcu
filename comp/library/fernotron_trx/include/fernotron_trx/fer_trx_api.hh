/**
 * \file fernotron_trx/fer_trx_api.hh
 * \brief C++-API for sending and receiving Fernotron RF messages
 */
#pragma once

#include <utils_misc/int_types.h>
#include <fernotron_trx/fer_trx_c_api.h>
#include <fernotron_trx/astro.h>

/**
 * \class Fer_Trx_API
 *
 * \ingroup fernotron_trx
 *
 * \brief User interface for sending and receiving Fernotron protocol messages via RF
 *
 * \note The corrosponding C API is defined in fer_trx_c_api.h
 *
 * \author Bert Winkelmann
 *
 */
class Fer_Trx_API {
public:

  /// type of a message
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

  /// get data from current received message. Use it in event functions.
  Fer_MsgPlainCmd get_msg() const;
  /// get data from current received message. Use it in event functions.
  u32 get_a() const;
  /// get data from current received message. Use it in event functions.
  u8 get_g() const;
  /// get data from current received message. Use it in event functions.
  u8 get_m() const;
  /// get data from current received message. Use it in event functions.
  fer_if_cmd get_cmd() const;
  /// get data from current received message. Use it in event functions.
  MsgKind get_msgKind() const;
  /// get data from current received message. Use it in event functions.
  const void * get_raw() const;
  /// get data from current received message. Use it in event functions.
  bool is_centralUnit() const;

  static int get_rssi();

public:
  ////////////////////////////////////////////////////////////////
  ///////// get notified if a message was received ///////////////
  ////////////////////////////////////////////////////////////////

  /// event: plain message was received (no doubles). Override it.
  virtual void event_plain_message_was_received() {  }
  /// event: double of a plain message was received. Override it.
  virtual void event_plain_double_message_was_received() {  }
  /// event: rtc only message was received. Override it.
  virtual void event_rtc_message_was_received() {  }
  /// event: a full timer message was received. Override it.
  virtual void event_timer_message_was_received() {  }
  /// event: a message of any kind was received. Override it.
  virtual void event_any_message_was_received() {  }

public:
  ////////////////////////////////////////////////////////////////
  ///////// get notified if a message is about to be send ////////
  ////////////////////////////////////////////////////////////////

  /// event: just before any queued message will be sent first (no repeats). Override it.
  virtual void event_first_message_will_be_sent() {   }
  /// event: just before any queued message will be sent (inclusive repeats). Override it.
  virtual void event_any_message_will_be_sent() {  }


public:
  ////////////////////////////////////////////////////////////////
  ///////// transmit messages ////////////////////////////////////
  ////////////////////////////////////////////////////////////////

  /// push a message to the RF send queue.
  static bool send_cmd(const Fer_MsgCmd &msg);
  /// push a message to the RF send queue.
  static bool send_cmd(u32 a, u8 g, u8 m, fer_if_cmd cmd, u8 repeats = 0, u16 delay = 0, u16 stopDelay = 0);

  /// push a message to the RF send queue.
  static bool send_rtc(const Fer_MsgRtc &msg);
  /// push a message to the RF send queue.
  static bool send_rtc(u32 a, u8 g, u8 m, time_t rtc);

  /// push a message to the RF send queue.
  static bool send_timer(const Fer_MsgTimer &msg);
  /// push a message to the RF send queue.
  static bool send_timer(u32 a, u8 g, u8 m, time_t rtc, const Fer_TimerData &td);

  /// push a message to the RF send queue.
  static bool send_empty_timer(const Fer_MsgRtc &msg);
  /// push a message to the RF send queue.
  static bool send_empty_timer(u32 a, u8 g, u8 m, time_t rtc);

public:
  ////////////////////////////////////////////////////////////////
  ///////// timer interrupt for RF transceiver ///////////////////
  ////////////////////////////////////////////////////////////////
  static void isr_handle_rx(bool rx_pin_lvl); ///< call this from timer ISR every (200/INTR_TICK_FREQ_MULT)us

  static bool isr_get_tx_level(); ///< call this son top of timer ISR
  static void isr_handle_tx();  ///< call this from timer ISR every 200us

public:
  ////////////////////////////////////////////////////////////////
  /////////////// run the action /////////////////////////////////
  ////////////////////////////////////////////////////////////////

  // getting notified if there is something to do

  /// event: there is something to do for the related worker function. Override it.
  virtual void event_ready_to_transmit() { }  // something to do for loop_tx()
  // getting notified from ISR by callbacks (no virtual functions possible from ISR)
  // note: the callback functions must be located in IRAM (use ATTR_IRAM)

  /// event: there is something to do for the related worker function. Called from ISR. Provide an ATTR_IRAM function.
  static void register_callback_msgReceived_ISR(CallBackFnType cb);

  /// event: there is something to do for the related worker function. Called from ISR. Provide an ATTR_IRAM function.
  static void register_callback_msgTransmitted_ISR(CallBackFnType cb);

  /// worker function for Receiver. Call after related events.
  static void loop_rx();

  /// worker function for Transmitter. Call after related events.
  static void loop_tx();

public:
  /// private: called internally
  static void push_event(struct Fer_Trx_IncomingEvent *evt);
private:
  struct Fer_Trx_IncomingEvent *myEvt;
  Fer_MsgPlainCmd myMsg;
};

typedef Fer_Trx_API::MsgKind fer_msg_kindT;

#include "fer_trx_api__inline.hh"
