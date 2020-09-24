#pragma once

#include <misc/int_types.h>
#include <fernotron/fer_msg_plain_enum.h>
#include <fernotron/timer_data.h>
#include <time.h>

struct Fer_MsgCmd {
  u32 a:24;
  u32 g :4;
  u8 m :4;
  fer_cmd cmd :4;
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
