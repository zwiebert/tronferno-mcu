#include "config.h"
#include "main/fernotron.h"

void mcu_read_config(u32 mask);
void mcu_save_config(u32 mask);

void read_config(u32 mask) {
  mcu_read_config(mask);
  fer_copyConfig();
}

void save_config(u32 mask) {
  fer_copyConfig();
  mcu_save_config(mask);
}


void save_config_item(enum configItem item) {
  save_config(1UL << item);
}

void read_config_item(enum configItem item) {
  read_config(1UL << item);
}

void save_config_all() {
  save_config(~0UL);
}

void read_config_all() {
  save_config(~0UL);
}
