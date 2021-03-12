#pragma once

int main_setup(void);
void mcu_restart(void);

#include <app_cli/cli_app.h>
void cli_run_mainLoop(enum mainLoop req);

