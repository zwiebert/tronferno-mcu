/*
 * move_buf.h
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */

#pragma once

#include "stdbool.h"
#include <stdint.h>
#include "fernotron/types.h"

enum direction { DIRECTION_NONE=0, DIRECTION_UP=0x01, DIRECTION_DOWN=0x02, DIRECTION_SUN_UP=0x05, DIRECTION_SUN_DOWN=0x06, DIRECTION_STOP = 0x08};
#define direction_isUp(dir) ((dir) & 0x01)
#define direction_isDown(dir) ((dir) & 0x02)
#define direction_isSun(dir) ((dir) & 0x04)
#define direction_isStop(dir) ((dir) & 0x08)
#define direction_isMove(dir) ((dir) & 0x07)

struct mv {
  GmBitMask mask;
  uint32_t start_time;
  enum direction dir;
};

#define mv_getFirst() mv_getNext(0)
struct mv *mv_getNext(struct mv *pred);
struct mv *mv_calloc();
void mv_free(struct mv *mv);

