/*
 * cli_setcmdline.c
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#include "user_config.h"
#include "cli_imp.h"
#include <string.h>

char * ICACHE_FLASH_ATTR
set_commandline(const char *src, uint8_t len) {  // FIXME: make sure cmd_buf is not in use by get_commandline()
  if (len >= CMD_BUF_SIZE)
    return 0;

  memcpy(cmd_buf, src, len);
  cmd_buf[len] = '\0';
  return cmd_buf;
}
