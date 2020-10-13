/**
 * \file        fernotron/trx/fer_trx_c_api.h
 * \brief       C-API for sending and receiving Fernotron RF messages.
 */

#pragma once

#ifdef __cplusplus
  extern "C" {
#endif

#include <misc/int_types.h>
#include <fernotron/trx/timer_data.h>
#include <time.h>

#ifndef FER_ADDR_TYPE_PlainSender
////// device type
#define FER_ADDR_TYPE_PlainSender  0x10 ///< Fernotron device type plain sender
#define FER_ADDR_TYPE_SunSensor     0x20 ///< Fernotron device type sun-sensor
#define FER_ADDR_TYPE_CentralUnit   0x80 ///< Fernotron device type central unit
#define FER_ADDR_TYPE_Receiver      0x90 ///< Fernotron device type motor (0x9xxxxx: code written on motor label)

/// \brief         Test if id A belongs to device type T
/// \param U32_A   id address
/// \param t       device address type
#define FER_U32_TEST_TYPE(u32_a, t) (((u32_a) >> 16) == (t))
#endif

/// \brief Fernotron command codes.
typedef enum : u8 { //XXX: C++ only
  fer_if_cmd_None,
  fer_if_cmd_1,
  fer_if_cmd_2,
  fer_if_cmd_STOP,   ///< stop motor
  fer_if_cmd_UP,     ///< run motor 'up'
  fer_if_cmd_DOWN,   ///< run motor 'down'
  fer_if_cmd_SunDOWN,  ///< run motor 'down' until sun position is reached (works only if sun auto is enabled)
  fer_if_cmd_SunUP,   ///< run motor 'up' (run motor up if both prior command was SunDOWN and motor position is above or equal sun-position)
  fer_if_cmd_SunINST,  ///< set current position as sun position
  fer_if_cmd_EndPosUP, ///< move motor 'up' until 'stop' is sent (the position at stop is saved as end position)
  fer_if_cmd_EndPosDOWN, ///< move motor 'down' until 'stop' is sent (the position at 'stop' is saved as end position)
  fer_if_cmd_ChangeRotationDirection, ///< toggles motor rotation direction (used to match up/down commands with real shutter movement)
  fer_if_cmd_0xc,
  fer_if_cmd_SET,   ///< activates set function to add or remove a controller
  fer_if_cmd_0xe,
  fer_if_cmd_Program ///< RTC/TImer data (or Sun-Test if dat_MEMB=fer_memb_SUN)
} fer_if_cmd;

/// \brief Vocabulary type of a plain command message
struct Fer_MsgPlainCmd {
  u32 a; ///< ID/address of device
  u8 g; ///< group number (if central unit device)
  u8 m;  ///< group-member number (if central unit device)
  fer_if_cmd cmd; ///< Fernotron command code
};

/// \brief Vocabulary type of a plain command message with values to control the transmitting
struct Fer_MsgCmd {
  u32 a; ///< ID/address of device
  u8 g; ///< group number (if central unit device)
  u8 m;  ///< group-member number (if central unit device)
  fer_if_cmd cmd; ///< Fernotron command code.
  i8 repeats; ///< Repeat the transmission REPEATS times.
  u16 delay;  ///< Delay the transmission by DELAY (in s/10).
  u16 stopDelay; ///< Send a STOP command after STOP_DELAY (in s/10).
};

/// \brief Vocabulary type of a message to set the RTC
struct Fer_MsgRtc  {
  u32 a; ///< ID/address of device
  u8 g; ///< group number (if central unit device)
  u8 m;  ///< group-member number (if central unit device)
  time_t rtc; ///< time value to set the built-in RTC
};

/// \brief Vocabulary type of a message to set the RTC and any automatics/timers
struct Fer_MsgTimer {
  u32 a; ///< ID/address of device
  u8 g; ///< group number (if central unit device)
  u8 m;  ///< group-member number (if central unit device)
  time_t rtc; ///< time value to set the built-in RTC
  const struct Fer_TimerData *td; ///< automatics/timer data
};

bool fer_trx_send_cmd(const Fer_MsgCmd *msg); ///< Enqueue a message to be transmit
bool fer_trx_send_rtc(const Fer_MsgRtc *msg); ///< Enqueue a message to be transmit
bool fer_trx_send_timer(const Fer_MsgTimer *msg); ///< Enqueue a message to be transmit
bool fer_trx_send_empty_timer(const Fer_MsgRtc *msg); ///< Enqueue a message to be transmit

typedef void (*CallBackFnType)(void);
extern CallBackFnType fer_rx_MSG_RECEIVED_ISR_cb;  ///< event callback from ISR: A message was received
extern CallBackFnType fer_tx_MSG_TRANSMITTED_ISR_cb; ///< event callback from ISR: Transmitting the message has been completed
extern void (*fer_tx_READY_TO_TRANSMIT_cb)(uint32_t when_to_transmit_ts); ///< event callback: the next message is ready to transmit at time WHEN_TO_TRANSMIT_TS
void fer_tx_loop(void); ///< Do work for transmitter, like preparing data to be sent from ISR
void fer_rx_loop(void); ///< Do work for receiver, like processing messages received by ISR
void fer_rx_sampleInput(bool pin_level); ///< call this from top of timer ISR handler
bool fer_tx_setOutput(void); ///< call this from top of timer ISR handler

void fer_rx_tick(void);  ///< ISR to receive RF messages. Call it from timer tick interrupt
void fer_tx_dck(void);  ///< ISR to transmit RF messages. call it from timer tick interrupt

#ifdef __cplusplus
  }
#endif
