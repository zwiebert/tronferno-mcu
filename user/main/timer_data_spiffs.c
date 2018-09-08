/*
 * timer_data_spiffs.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "../user_config.h"
#if ENABLE_SPIFFS

#include "timer_data_spiffs.h"


/*
 * config_flash2.c
 *
 * read and write multiple data objects with same size from/to flash.
 * (its an array of objects, where each can be written independently)
 *
 * object size cannot be larger than 4092 byte
 *
 * note: configure type of your data and pointer to the object in related header file

 *  Created on: 16.09.2017
 *      Author: bertw
 */

#include "main/all.h"
#include "main/config.h"
#include "user_config.h"

#define TEST_THIS_MODULE 0

#if TEST_THIS_MODULE
#define DB(x) x
#define DB2(x)
#else
#define DB(x) ((C.app_verboseOutput >= vrbDebug) && (x),1)
#define DB2(x) DB(x)
#endif
#define printf ets_uart_printf
extern int ENR; // error number
enum enr { enr_none, enr_inv_fid, enr_2, enr_3, enr_4, enr_5 };

////////////////// private ///////////////////////////////////////////////////////////////

static char * ICACHE_FLASH_ATTR
gm_to_file_name (uint8_t g, uint8_t m) {
  static char file_name[] = "tdxx.bin";
  if (g > 7 || m > 7) {
    return NULL;
  }

  file_name[2] = '0'+g;
  file_name[3] = '0'+m;

  return file_name;
}


// read data from flash and copy it to DATA_PTR
// does nothing if not data was found in flash
static bool ICACHE_FLASH_ATTR read_data2(DATA_TYPE *p, const char *file_name) {
  spiffs_file file;
   s32_t nmb_read = 0;

   if (!file_name)
     return false;

   if ((file = SPIFFS_open(fs_A, file_name, SPIFFS_O_RDONLY, 0)) < 0) {
       return false;
   }

   nmb_read = SPIFFS_read(fs_A, file, p, sizeof (DATA_TYPE));
   SPIFFS_close(fs_A, file);

   if (nmb_read < 0) {
     DB((io_puts("spiffs:errno: "), io_putl(SPIFFS_errno(fs_A), 10),  io_puts("\n")));
     return false;
   }

  DB(printf("file read: file=%s\n", file_name));

  return true;
}



static int ICACHE_FLASH_ATTR delete_shadowded_files(uint8_t group, uint8_t memb) {
  int g, m, result = 0;
  for (g = 0; g <= 7; ++g) {
    for (m = 0; m <= 7; ++m) {
      if ((group == 0 || group == g) && (memb == 0 || memb == m)) {
        if (SPIFFS_remove(fs_A, gm_to_file_name(g, m)) >= 0) {
          DB2(printf("shadow deleted: g=%d, m=%d, fid=%s\n", (int)g, (int)m, gm_to_file_name(g, m)));
          ++result;
        }
      }
    }
  }
  return result;
}

// reading data from DATA_PTR and write it to flash
// does nothing if data is unchanged
static bool ICACHE_FLASH_ATTR save_data2(DATA_TYPE *p, const char *file_name) {
  spiffs_file file;
   s32_t nmb_written;


   if (!file_name)
     return false;

   if ((file = SPIFFS_open(fs_A, file_name, SPIFFS_O_TRUNC | SPIFFS_O_CREAT | SPIFFS_O_WRONLY, 0)) < 0) {
       spiffs_print_errno();
       return false; // error
   }

   nmb_written = SPIFFS_write(fs_A, file, p, sizeof (DATA_TYPE));
   SPIFFS_close(fs_A, file);
   if (nmb_written < 0) {
     DB((io_puts("spiffs:errno: "), io_putl(SPIFFS_errno(fs_A), 10),  io_puts("\n")));
   }

   DB((io_putd(nmb_written), io_puts("<-written-\n")));
   return true;

  DB2(printf("file saved: fid=%s\n", file_name));

  return true;

}


////////////////////////////////// public ////////////////////////////////////////////////////////////////////


bool ICACHE_FLASH_ATTR save_timer_data_spiffs(DATA_TYPE *p, uint8_t g, uint8_t m) {
  bool result = false;

  delete_shadowded_files(g, m);
  result = save_data2(p, gm_to_file_name(g, m));

  return result;
}

bool ICACHE_FLASH_ATTR read_timer_data_spiffs(DATA_TYPE *p, uint8_t *g, uint8_t *m, bool wildcard) {
  bool result = read_data2(p, gm_to_file_name(*g, *m));

  if (!result && wildcard) {
    if ((result = read_data2(p, gm_to_file_name(*g, 0)))) {
      *m = 0;
    } else if ((result = read_data2(p, gm_to_file_name(0, *m)))) {
      *g = 0;
    } else if ((result = read_data2(p, gm_to_file_name(0, 0)))) {
      *g = *m = 0;
    }
  }
  return result;
}

#if TEST_THIS_MODULE
static void module_selftest(void) {

}
#endif

// initialize this software module
void ICACHE_FLASH_ATTR setup_timer_data_spiffs() {

  DB(ets_uart_printf("setup_timer_data_spiffs()\n"));
#if TEST_THIS_MODULE
  module_selftest();
#endif
}

#endif /* ENABLE_SPIFFS */
