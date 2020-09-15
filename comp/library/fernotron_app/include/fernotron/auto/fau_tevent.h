#pragma once

#include "fau_tminutes.h"
#include <time.h>

typedef struct {
  GmBitMask member_mask[2];
  minutes_t next_event;
} timer_event_t;
#define te_getMaskUp(te) (&(te)->member_mask[0])
#define te_getMaskDown(te) (&(te)->member_mask[1])
#define te_isDisabled(te) ((te)->next_event == MINUTES_DISABLED)

// returns false, if no timer event occurs until midnight
bool fam_get_next_timer_event(timer_event_t *evt, const time_t *now_time);

void fam_updateTimerEvent();
void fam_loop_old(void);
void fam_loop(void);
