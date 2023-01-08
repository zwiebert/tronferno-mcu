/**
 * \file       fernotron/auto/fau_tevent.h
 * \brief      Keep track about when the next automatic movement is been scheduled.
 */

#pragma once

#include "fau_tevent.hh"

extern Fer_TimerEvent next_event_te;


/**
 * \brief   Update module internal state holding today's timer events
 * \note    Call this after modifying any timers
 */
inline void fer_am_updateTimerEvent(const time_t &now_time) {
  next_event_te.fer_am_updateTimerEvent(now_time);
}


/**
 * \brief     Do work.
 * \note      Call it periodically from main loop
 */
void fer_am_loop(void);
