/*
 * move_buf.c
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */

#include "move.h"
#include "move_buf.h"

#include "stdbool.h"
#include <stdint.h>
#include "fernotron/types.h"
#include "misc/int_macros.h"
#include "misc/int_types.h"

#define fer_mv_SIZE 8
struct Fer_Move moving[fer_mv_SIZE];
u8 moving_mask;


void (*fer_pos_POSITIONS_MOVE_cb)(bool has_unsaved);
static inline void fer_pos_HAS_MOVING_cb() {
  if (fer_pos_POSITIONS_MOVE_cb)
    fer_pos_POSITIONS_MOVE_cb(true);
}
static inline void fer_pos_HAS_NO_MOVING_cb() {
  if (fer_pos_POSITIONS_MOVE_cb)
    fer_pos_POSITIONS_MOVE_cb(false);
}



struct Fer_Move* fer_mv_getNext(struct Fer_Move *pred) {
  if (!moving_mask)
    return 0;

  int mvi = pred ? (pred - moving) + 1 : 0;
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
    fer_pos_HAS_MOVING_cb();
    return &moving[mvi];
  }
  return 0;
}

void fer_mv_free(struct Fer_Move *Fer_Move) {
  int mvi = Fer_Move - moving;
  CLR_BIT(moving_mask, mvi);
  if (!moving_mask)
    fer_pos_HAS_NO_MOVING_cb();
}

