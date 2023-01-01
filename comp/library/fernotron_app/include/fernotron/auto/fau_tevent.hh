/**
 * \file       fernotron/auto/fau_tevent.h
 * \brief      Keep track about when the next automatic movement is been scheduled.
 */

#pragma once

#include "fau_tminutes.h"
#include <time.h>

/**
 * \brief    When and where a movement will happen.
 */
class Fer_TimerEvent {
private:
  time_t next_event_time = 0;
  Fer_GmSet member_mask[2]; ///< Array of g/m-sets. 0=up, 1=down.
  fer_au_minutesT next_event = MINUTES_DISABLED; ///< Minute of day this event will occur.

public:
  Fer_TimerEvent() = default;

  /**
   * \brief             Get events left for today (events occurring between now and midnight)
   * \param now_time    Current time.  Time span is NOW_TIME...midnight
   * \return            true if there were any matching events
   */
  bool fer_am_get_next_timer_event(const time_t *now_time);

public:
  Fer_GmSet* te_getMaskUp() { ///< Get set of g/m which will move upward at this event
    return (&member_mask[0]);
  }
  Fer_GmSet* te_getMaskDown() { ///< Get set of g/m which will move downward at this event
    return (&member_mask[1]);
  }
  bool te_isDisabled() const { ///<  Test if this event is disabled
    return (next_event == MINUTES_DISABLED);
  }

  void fer_am_updateTimerEventTime(const time_t &now_time);
  bool te_matchMinute(const time_t &now_time);
  fer_au_minutesT te_getEventMinute() const { return next_event; }
  time_t te_getEventTime() const { return next_event_time; }
  void fer_am_updateTimerEvent(const time_t &now);

  /**
   * \brief     Test if there is an event NOW.
   * \note      Call it periodically from main loop
   * \param now Use this time to compare the next event too. If the next event is older than NOW, call \ref fer_am_updateTimerEvent and return false
   * \return    true if there is an event this minute
   */
  bool te_isEventNow(const time_t &now);
};
