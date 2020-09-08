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

#define mv_SIZE 8
struct mv moving[mv_SIZE];
u8 moving_mask;


void (*fpos_POSITIONS_MOVE_cb)(bool has_unsaved);
static inline void fpos_HAS_MOVING_cb() {
  if (fpos_POSITIONS_MOVE_cb)
    fpos_POSITIONS_MOVE_cb(true);
}
static inline void fpos_HAS_NO_MOVING_cb() {
  if (fpos_POSITIONS_MOVE_cb)
    fpos_POSITIONS_MOVE_cb(false);
}



struct mv* mv_getNext(struct mv *pred) {
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

struct mv* mv_calloc() {
  u8 mvi;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi))
      continue;

    SET_BIT(moving_mask, mvi);
    moving[mvi] = (struct mv ) { };
    fpos_HAS_MOVING_cb();
    return &moving[mvi];
  }
  return 0;
}

void mv_free(struct mv *mv) {
  int mvi = mv - moving;
  CLR_BIT(moving_mask, mvi);
  if (!moving_mask)
    fpos_HAS_NO_MOVING_cb();
}

