#include "config.h"

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
