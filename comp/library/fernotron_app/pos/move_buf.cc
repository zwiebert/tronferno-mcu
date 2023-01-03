/*
 * move_buf.c
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */

#include "move.hh"
#include "move_buf.h"

#include "stdbool.h"
#include <stdint.h>
#include <array>
#include "fernotron/fer_pct.h"
#include "utils_misc/int_macros.h"
#include "utils_misc/int_types.h"
#include "main_loop/main_queue.hh"


struct std::array<Fer_Move, fer_mv_SIZE> moving;
static unsigned moving_mask;
static void *moving_timer;

struct Fer_Move* fer_mv_getNext(struct Fer_Move *pred) {
  if (!moving_mask)
    return 0;

  int mvi = pred ? std::distance(moving.begin(), pred) + 1 : 0;
  u8 msk = moving_mask >> mvi;

  for (; msk; ++mvi, (msk >>= 1)) {
    if (msk & 0x01)
      return &moving[mvi];
  }
  return 0;
}

struct Fer_Move* fer_mv_calloc() {
  u8 mvi;

  for (mvi = 0; mvi < fer_mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi))
      continue;

    SET_BIT(moving_mask, mvi);
    moving[mvi] = (struct Fer_Move ) { };
    if (!moving_timer) {
      moving_timer = mainLoop_callFunByTimer(fer_pos_checkStatus_whileMoving, 200, true);
    }
    return &moving[mvi];
  }
  return 0;
}

void fer_mv_free(struct Fer_Move *Fer_Move) {
  const int mvi = std::distance(moving.begin(), Fer_Move);
  CLR_BIT(moving_mask, mvi);
  if (!moving_mask)
    if (moving_timer) {
      mainLoop_stopFun(moving_timer);
      moving_timer = nullptr;
    }
}

