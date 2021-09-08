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
#include "move.h"

enum direction { DIRECTION_NONE=0, DIRECTION_UP=0x01, DIRECTION_DOWN=0x02, DIRECTION_SUN_UP=0x05, DIRECTION_SUN_DOWN=0x06, DIRECTION_STOP = 0x08};

inline bool direction_isUp(enum direction dir) {
  return dir & 0x01;
}
inline bool direction_isDown(enum direction dir) {
  return dir & 0x02;
}
inline bool direction_isSun(enum direction dir) {
  return dir & 0x04;
}
inline bool direction_isStop(enum direction dir) {
  return dir & 0x08;
}
inline bool direction_isMove(enum direction dir) {
  return dir & 0x07;
}

inline bool direction_isEndPos(enum direction dir, uint8_t pct) {
  return ((direction_isUp(dir) && pct == PCT_UP) || (!direction_isUp(dir) && pct == PCT_DOWN));
}

struct Fer_Move {
  Fer_GmSet mask;
  uint32_t start_time;
  enum direction dir;
};

#define fer_mv_getFirst() fer_mv_getNext(0)
struct Fer_Move *fer_mv_getNext(struct Fer_Move *pred);
struct Fer_Move *fer_mv_calloc();
void fer_mv_free(struct Fer_Move *Fer_Move);
