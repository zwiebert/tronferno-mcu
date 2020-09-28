#pragma once

#include "fau_tminutes.h"
#include <time.h>

typedef struct {
  Fer_GmBitMask member_mask[2];
  fer_au_minutesT next_event;
} Fer_TimerEvent;
#define te_getMaskUp(te) (&(te)->member_mask[0])
#define te_getMaskDown(te) (&(te)->member_mask[1])
#define te_isDisabled(te) ((te)->next_event == MINUTES_DISABLED)

// returns false, if no timer event occurs until midnight
bool fer_am_get_next_timer_event(Fer_TimerEvent *evt, const time_t *now_time);

void fer_am_updateTimerEvent();
void fer_am_loop_old(void);
void fer_am_loop(void);
