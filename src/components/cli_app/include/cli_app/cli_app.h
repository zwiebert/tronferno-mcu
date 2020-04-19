/*
 * cli_app.h
 *
 *  Created on: 19.02.2020
 *      Author: bertw
 */

#pragma once

#include "cli_imp.h"

int process_parmSend(clpar p[], int len);
int process_parmConfig(clpar p[], int len);
int process_parmMcu(clpar p[], int len);
int process_parmTimer(clpar p[], int len);
int process_parmHelp(clpar p[], int len);
int process_parmPair(clpar p[], int len);
int process_parmShpref(clpar p[], int len);

