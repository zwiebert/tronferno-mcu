/*
 * storage.c
 *
 *  Created on: 13.09.2018
 *      Author: bertw
 */

#include "user_config.h"
#if ENABLE_SPIFFS



#include "config/config.h"
#include "spiffs.h"
#include "main/storage.h"
#include "main/debug.h"

#define TEST_THIS_MODULE 0

#if TEST_THIS_MODULE
#define DB(x) x
#define DB2(x)
#else
#define DB(x) ((C.app_verboseOutput >= vrbDebug) && (x),1)
#define DB2(x) DB(x)
#endif



///////////// implement read/ write from storage.h ////////////////////////

bool ICACHE_FLASH_ATTR
write_to_file(const char *path, const void *src, size_t len) {
  spiffs_file file;
   s32_t nmb_written;


   if (!path)
     return false;

   if ((file = SPIFFS_open(fs_A, path, SPIFFS_O_TRUNC | SPIFFS_O_CREAT | SPIFFS_O_WRONLY, 0)) < 0) {
       spiffs_print_errno();
       return false; // error
   }

   nmb_written = SPIFFS_write(fs_A, file, src, len);
   SPIFFS_close(fs_A, file);
   if (nmb_written < 0) {
     DB((io_puts("spiffs:errno: "), io_putl(SPIFFS_errno(fs_A), 10),  io_puts("\n")));
   }

   DB((io_putd(nmb_written), io_puts("<-written-\n")));
   return true;

  DB2(printf("file saved: fid=%s\n", path));

  return true;

}

bool ICACHE_FLASH_ATTR
read_from_file(const char *path, void *dst, size_t len) {
  spiffs_file file;
    s32_t nmb_read = 0;

    if (!path)
      return false;

    if ((file = SPIFFS_open(fs_A, path, SPIFFS_O_RDONLY, 0)) < 0) {
        return false;
    }

    nmb_read = SPIFFS_read(fs_A, file, dst, len);
    SPIFFS_close(fs_A, file);

    if (nmb_read < 0) {
      DB((io_puts("spiffs:errno: "), io_putl(SPIFFS_errno(fs_A), 10),  io_puts("\n")));
      return false;
    }

   DB(printf("file read: file=%s\n", path));

   return true;
}

bool ICACHE_FLASH_ATTR
delete_file(const char *path) {
  return (SPIFFS_remove(fs_A, path) >= 0);
}



#endif /* ENABLE_SPIFFS */
