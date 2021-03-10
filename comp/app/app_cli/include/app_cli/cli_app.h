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

enum mainLoop {
  mainLoop_mcuRestart,
  mainLoop_configGPIO,
  mainLoop_configCC1101,
  mainLoop_configAstro,
  mainLoop_configEthernet,
  mainLoop_configMqttAppClient,
  mainLoop_configHttpServer,
  mainLoop_configTxtio,
};

/// \brief provide a pointer to set flags for main loop execution
extern void (*cli_run_mainLoop_cb)(enum mainLoop req);
inline void cli_run_main_loop(enum mainLoop req) {
  if (!cli_run_mainLoop_cb)
    return;
  cli_run_mainLoop_cb(req);
}


/**
 * \brief setup this component
 */
void cliApp_setup();




