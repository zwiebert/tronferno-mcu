/**
 * \brief      Keep track about when the next automatic movement is been scheduled.
 */

#pragma once

#include "fau_tminutes.h"
#include <time.h>

/**
 * \brief    When and where a movement will happen.
 */
typedef struct {
  Fer_GmSet member_mask[2]; ///< Array of g/m-sets. 0=up, 1=down.
  fer_au_minutesT next_event; ///< Minute of day this event will occur.
} Fer_TimerEvent;

inline Fer_GmSet * te_getMaskUp(Fer_TimerEvent *te) { return (&(te)->member_mask[0]); } ///< Get set of g/m which will move upward at this event
inline Fer_GmSet * te_getMaskDown(Fer_TimerEvent *te) { return (&(te)->member_mask[1]); } ///< Get set of g/m which will move downward at this event
inline bool te_isDisabled(Fer_TimerEvent *te) { return ((te)->next_event == MINUTES_DISABLED); }  ///<  Test if this event is disabled



// returns false, if no timer event occurs until midnight
bool fer_am_get_next_timer_event(Fer_TimerEvent *evt, const time_t *now_time);

void fer_am_updateTimerEvent();
void fer_am_loop_old(void);
void fer_am_loop(void);
