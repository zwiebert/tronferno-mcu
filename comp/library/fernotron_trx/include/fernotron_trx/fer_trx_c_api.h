/**
 * \file        fernotron_trx/fer_trx_c_api.h
 * \brief       C-API for sending and receiving Fernotron RF messages.
 */

#pragma once

#ifdef __cplusplus
  extern "C" {
#endif

#include <utils_misc/int_types.h>
#include <fernotron_trx/timer_data.h>
#include <time.h>

#define FER_ADDR_TYPE_PlainSender  0x10 ///< Fernotron device type plain sender
#define FER_ADDR_TYPE_SunSensor     0x20 ///< Fernotron device type sun-sensor
#define FER_ADDR_TYPE_CentralUnit   0x80 ///< Fernotron device type central unit
#define FER_ADDR_TYPE_Receiver      0x90 ///< Fernotron device type motor (0x9xxxxx: code written on motor label)

/**
 * \brief       Match type of device address
 * \param a     device address
 * \param t     type
 * \return      true for match
 */
inline bool FER_U32_TEST_TYPE(uint32_t a, unsigned t) {
  return (a >> 16) == t;
}

/// \brief Fernotron command codes.
typedef enum
#ifdef __cplusplus
: u8
#endif
  {
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
  fer_if_cmd_ToggleRotationDirection, ///< toggles motor rotation direction (used to match up/down commands with real shutter movement)
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
  u8 repeats; ///< Repeat the transmission REPEATS times.
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

/**
 * \brief    Enqueue a message to be transmitted
 * \return   message successfully enqueued
 */
bool fer_trx_send_cmd(const Fer_MsgCmd *msg);

/**
 * \brief    Enqueue a message to be transmitted
 * \return   message successfully enqueued
 */
bool fer_trx_send_rtc(const Fer_MsgRtc *msg);

/**
 * \brief    Enqueue a message to be transmitted
 * \return   message successfully enqueued
 */
bool fer_trx_send_timer(const Fer_MsgTimer *msg);

/**
 * \brief    Enqueue a message to be transmitted
 * \return   message successfully enqueued
 */
bool fer_trx_send_empty_timer(const Fer_MsgRtc *msg);

typedef void (*CallBackFnType)(void);

/**
 * \brief  Event callback:  A message has been received
 * \note   The callback must be placed in IRAM, because its called from ISR
 */
extern void (*fer_rx_MSG_RECEIVED_ISR_cb)(void);

/**
 * \brief  Event callback:  A message has been completely transmitted
 * \note   The callback must be placed in IRAM, because its called from ISR
 */
extern void (*fer_tx_MSG_TRANSMITTED_ISR_cb)(void);

/**
 * \brief          Will be called if the next delayed message is ready for transmission in TIME_TS
 *
 * \note           You can use this to start a timer interval which will call \link fer_tx_loop \endlink  after TIME_TS.
 *                 Until then there is no work to do. So calling fer_tx_loo would be wasteful.
 *
 * \param time_ts  Duration (in s/10) until the next message is ready for transmission
 */
extern void (*fer_tx_READY_TO_TRANSMIT_cb)(uint32_t time_ts);

/**
 * \brief   Do work.
 * \note    Call this from main thread in reaction to \link fer_tx_MSG_TRANSMITTED_ISR_cb  \endlink calls.
 * \note    Call this also from a timer interval in main thread, after pushing a delayed message (to avoid polling)
 * \note    Setup that interval in reaction to \link fer_tx_READY_TO_TRANSMIT_cb \endlink
 */
void fer_tx_loop(void);

/**
 * \brief  Do some work
 * \note   Should be called from main thread as reaction to \link fer_rx_MSG_RECEIVED_ISR_cb \endlink.
 */
void fer_rx_loop(void);

/**
 * \brief           Set RF output pin according to the state in the transmitter
 * \note            Calls this from the top of a precise timer ISR.
 */
bool fer_tx_setOutput(void);

/**
 * \brief            Do some work in receiving a message
 * \param rx_pin_lvl Level of RX pin sampled at begin of timer ISR callback
 * \note             Call this from timer ISR with 200us / FREQ_MULT (= 5kHz * FREQ_MULT).
 */
void fer_rx_tick(bool rx_pin_lvl);


/**
 * \brief     Do some work in transmitting a message
 * \note      Should be called from a timer ISR with  200us (= 5kHz)
 * \note      Timing is less critical, because the output pin is set separately by \link fer_tx_setOutput \endlink.
 */
void fer_tx_dck(void);


#ifdef __cplusplus
  }
#endif
