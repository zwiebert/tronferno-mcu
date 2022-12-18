/*
 * \file  fernotron/pos/move_buf.h
 * \brief fixed size list of position movement objects
 */

#pragma once

#include "stdbool.h"
#include <stdint.h>
#include "fernotron/fer_pct.h"
#include "move.h"

enum direction {
  DIRECTION_NONE = 0, DIRECTION_UP = 0x01, DIRECTION_DOWN = 0x02, DIRECTION_SUN_UP = 0x05, DIRECTION_SUN_DOWN = 0x06, DIRECTION_STOP = 0x08
};

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

/**
 * \brief Maximal number of active items
 */
constexpr unsigned fer_mv_SIZE = 8;

/**
 * \brief  Item which represents a movement of one ore more Members (all started at the same time and with same direction)
 */
struct Fer_Move {
  Fer_GmSet mask; ///< bitmask of members which are in move (and tracked by this object)
  uint32_t start_time; ///< time when movement started
  enum direction dir; ///< direction/kind of movement
};

/**
 * \brief   Get next active item in movement list
 * \return  next item or nullptr if there is none
 */
struct Fer_Move* fer_mv_getNext(struct Fer_Move *pred);

/**
 * \brief   Get first active item in movement list
 * \return  first item or nullptr if list was empty
 */
inline struct Fer_Move* fer_mv_getFirst() {
  return fer_mv_getNext(0);
}

/**
 * \brief  Create new active item and append it to our list (start movement timer if list was empty)
 * \return newly created item (all fields set to zero) or nullptr if maximum number of active items reached
 */
struct Fer_Move* fer_mv_calloc();

/**
 * \brief Remove an ative item from our list (stop movement timer if list becomes empty)
 * \param Fer_Move   Item  wich was allocated by \ref fer_mv_calloc
 */
void fer_mv_free(struct Fer_Move *Fer_Move);

