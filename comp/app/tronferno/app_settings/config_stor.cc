#include "storage/storage.h"
#include "app_settings/config.h"

#define CONFIG_FILE_NAME "mcu_cfg"

bool config_read() {
  return stor_fileRead(CONFIG_FILE_NAME, &C, sizeof C);
}

bool config_save() {
  return stor_fileWrite(CONFIG_FILE_NAME, &C, sizeof C);
}
