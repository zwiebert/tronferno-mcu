#pragma once

#include <stdint.h>

struct fer_configT {
  uint32_t cu;
};

void fer_setup(const fer_configT &ferConfig, bool reinit = false);
