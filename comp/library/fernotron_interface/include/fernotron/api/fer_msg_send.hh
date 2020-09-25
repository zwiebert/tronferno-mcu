#pragma once

#include <misc/int_types.h>
#include <fernotron/timer_data.h>
#include <time.h>

#ifndef FER_ADDR_TYPE_PlainSender
////// device type
#define FER_ADDR_TYPE_PlainSender  0x10
#define FER_ADDR_TYPE_SunSensor     0x20
#define FER_ADDR_TYPE_CentralUnit   0x80
#define FER_ADDR_TYPE_Receiver      0x90 // 0x9xxxxx (code written on motor label)
#define FER_U32_TEST_TYPE(u32_a, t) (((u32_a) >> 16) == (t))
#endif

typedef enum {
  fer_if_cmd_None,
  fer_if_cmd_1,
  fer_if_cmd_2,
  fer_if_cmd_STOP,   // stop motor
  fer_if_cmd_UP,     // run motor 'up'
  fer_if_cmd_DOWN,   // run motor 'down'
  fer_if_cmd_SunDOWN,  // run motor 'down' until sun position is reached (works only if sun auto is enabled)
  fer_if_cmd_SunUP,   // run motor 'up' (run motor up if both prior command was SunDOWN and motor position is above or equal sun-position)
  fer_if_cmd_SunINST,  // set current position as sun position
  fer_if_cmd_EndPosUP, // move motor 'up' until 'stop' is sent (the position at stop is saved as end position)
  fer_if_cmd_EndPosDOWN, // move motor 'down' until 'stop' is sent (the position at 'stop' is saved as end position)
  fer_if_cmd_ChangeRotationDirection, // toggles motor rotation direction (used to match up/down commands with real shutter movement)
  fer_if_cmd_0xc,
  fer_if_cmd_SET,   // activates set function to add or remove a controller
  fer_if_cmd_0xe,
  fer_if_cmd_Program // RTC/TImer data (or Sun-Test if dat_MEMB=fer_memb_SUN)
} fer_if_cmd;


struct Fer_MsgCmd {
  u32 a:24;
  u32 g :4;
  u8 m :4;
  fer_if_cmd cmd :4;
  i8 repeats;
  u16 delay;
  u16 stopDelay;
};

struct Fer_MsgRtc  {
  u32 a:24;
  u32 g :4;
  u8 m :4;
  time_t rtc;
};

struct Fer_MsgTimer {
  u32 a:24;
  u32 g :4;
  u8 m :4;
  time_t rtc;
  Fer_TimerData td;
};

class Fer_SendMsg {
public:
  Fer_SendMsg() = default;
  bool send(const Fer_MsgCmd &msg);
  bool send(const Fer_MsgRtc &msg);
  bool send(const Fer_MsgTimer &msg);
  bool send_empty_timer(const Fer_MsgRtc &msg);
};

extern Fer_SendMsg fer_api_tx;
