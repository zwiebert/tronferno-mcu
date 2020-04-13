#ifndef COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_ESP32_ESP32_CALLBACKS_H_
#define COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_ESP32_ESP32_CALLBACKS_H_

#include "../callbacks.h"

void loop_setBit_rxLoop_fromISR(void);
#define frx_MSG_RECEIVED_ISR_cb loop_setBit_rxLoop_fromISR

void loop_setBit_txLoop_fromISR(void);
#define ftx_MSG_TRANSMITTED_ISR_cb  loop_setBit_txLoop_fromISR

void loop_setBit_txLoop(void);
#define ftx_READY_TO_TRANSMIT_cb loop_setBit_txLoop

#ifdef USE_SEP
void loop_setPerBit_sepLoop(void);
void loop_clrPerBit_sepLoop(void);
#define sep_ENABLE_cb loop_setPerBit_sepLoop
#define sep_DISABLE_cb loop_clrPerBit_sepLoop
#endif

#ifdef USE_PAIRINGS
void loop_setPerBit_pairLoop(void);
void loop_clrPerBit_pairLoop(void);
#define pair_ENABLE_cb loop_setPerBit_pairLoop
#define pair_DISABLE_cb loop_clrPerBit_pairLoop
#endif

#ifdef USE_CUAS
void loop_setPerBit_cuasLoop(void);
void loop_clrPerBit_cuasLoop(void);
#define cuas_ENABLE_cb loop_setPerBit_cuasLoop
#define cuas_DISABLE_cb loop_clrPerBit_cuasLoop
#endif

void loop_setBit_fauTimerDataHasChanged(void);
#define fau_TIMER_DATA_CHANGE_cb() loop_setBit_fauTimerDataHasChanged()


void ferPos_loopAutoSave();
void loop_setPerBit_loopAutoSave(void);
void loop_clrPerBit_loopAutoSave(void);
#define  fpos_POSITIONS_UNSAVED_cb loop_setPerBit_loopAutoSave
#define  fpos_POSTIONS_SAVED_cb loop_clrPerBit_loopAutoSave


void ferPos_loopCheckMoving();
void loop_setPerBit_loopCheckMoving(void);
void loop_clrPerBit_loopCheckMoving(void);
#define  fpos_HAS_MOVING_cb loop_setPerBit_loopCheckMoving
#define  fpos_HAS_NO_MOVING_cb loop_clrPerBit_loopCheckMoving



#endif /* COMPONENTS_APP_CONFIG_INCLUDE_APP_CONFIG_ESP32_ESP32_CALLBACKS_H_ */
