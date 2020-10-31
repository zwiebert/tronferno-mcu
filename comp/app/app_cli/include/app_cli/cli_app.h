/**
 * \file      cli/cli_app.h
 * \brief     Component to insert parameter handlers into cli framework
 */


#pragma once
#include "cli/cli.h"

extern const char cli_help_parmSend[]; ///< help text for CLI command
extern const char cli_help_parmTimer[]; ///< help text for CLI command
extern const char cli_help_parmConfig[]; ///< help text for CLI command
extern const char cli_help_parmMcu[]; ///< help text for CLI command
extern const char cli_help_parmPair[]; ///< help text for CLI command
extern const char cli_help_parmShpref[]; ///< help text for CLI command
extern const char cli_help_parmHelp[]; ///< help text for CLI command

/// \brief provide a pointer to MCU restart function here
extern void (*mcu_restart_cb)();



/**
 * \brief setup this component
 */
void cliApp_setup();
