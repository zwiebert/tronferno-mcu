/*
 * config_spiffs.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"

#if ENABLE_SPIFFS

#include "config_spiffs.h"




#include "config/config.h"
#include "storage/spiffs_fs.h"

#define CFG_FILE "mcucfg.bin"
#define DATA_TYPE config
#define DATA_PTR (&C)

void spiffs_print_errno() {
  io_puts("spiffs:errno: "), io_putl(SPIFFS_errno(fs_A), 10),  io_puts("\n");
}

int 
config_read(void) {
  spiffs_file file;
  s32_t nmb_read = 0;

  if ((file = SPIFFS_open(fs_A, CFG_FILE, SPIFFS_O_RDONLY, 0)) < 0) {
      spiffs_print_errno();
      return false; // error
  }

  nmb_read = SPIFFS_read(fs_A, file, DATA_PTR, sizeof (DATA_TYPE));
  SPIFFS_close(fs_A, file);
  if (nmb_read < 0) {
    io_puts("spiffs:errno: "), io_putl(SPIFFS_errno(fs_A), 10),  io_puts("\n");
  }
  io_putd(nmb_read), io_puts("<-read-\n");

  return true;
}


// call read_data() at least once in the program before the first calling of save_data()!
int 
config_save(void) {
  spiffs_file file;
  s32_t nmb_written;

  if ((file = SPIFFS_open(fs_A, CFG_FILE, SPIFFS_O_TRUNC | SPIFFS_O_CREAT | SPIFFS_O_WRONLY, 0)) < 0) {
      spiffs_print_errno();
      return false; // error
  }

  nmb_written = SPIFFS_write(fs_A, file, DATA_PTR, sizeof (DATA_TYPE));
  SPIFFS_close(fs_A, file);
  if (nmb_written < 0) {
    io_puts("spiffs:errno: "), io_putl(SPIFFS_errno(fs_A), 10),  io_puts("\n");
  }

  io_putd(nmb_written), io_puts("<-written-\n");
  return true;
}

#endif /* ENABLE_SPIFFS */
