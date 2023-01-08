/**
 * \file     fernotron/auto/fau_tdata_store.h
 * \brief    Persistent timer data storage
 */
/*
 * timer_data.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#pragma once

#include "fernotron_trx/timer_data.h"


/**
 * \brief         Store timer data
 * \param p       Timer data object
 * \param g,m     Group/Member to which the timer data is attached to.  Wild-cards allowed.  All overshadowed timers will then be removed. (e.g. g0,m0 will erase all existing timers)
 * \return        true on success
 */
bool fer_stor_timerData_save(Fer_TimerData *p, uint8_t g, uint8_t m);

/**
 * \brief              Load timer data
 * \param p            Timer data object
 * \param[in,out] g,m  Pointer to Group/Member to match. Return the g,m the timer is attached to.
 * \param wildcard     If true, return the best matching timer (a stored g1,m0 will match a g1,m2 parameter).  If false return only an exact match.
 * \return             true on success
 */
bool fer_stor_timerData_load(Fer_TimerData *p, uint8_t *g, uint8_t *m, bool wildcard);


/**
 * \brief              Erase one or more timers
 * \param g,m          Group/members. Each one can be 0, which will match any. (e.g. g0,m0 will erase all stored timers).
 * \return             true on success
 */
bool fer_stor_timerData_erase(uint8_t g, uint8_t m);


