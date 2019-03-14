/*
 * fer_misc.c
 *
 * Created: 12.09.2017 01:40:05
 *  Author: bertw
 */ 



#include <stdint.h>

#include "config/config.h"
#include "fernotron/fer.h"
#include "fernotron/fer_code.h"
#include "fernotron/fer_frame.h"
#include "fernotron/fer_timings.h"
#include "main/common.h"
#include "main/debug.h"
#include "misc/int_macros.h"
#include "stdbool.h"
#include "userio/inout.h"

void ICACHE_FLASH_ATTR
frb_printPacket(const uint8_t *dg) {
	int i;

  dbg_trace();
  for (i = 0; i < bytesPerCmdPacket; ++i) {
    io_print_hex_8(dg[i], true);
  }
  io_putlf();
}

void ICACHE_FLASH_ATTR
printTimerStamp(uint8_t d[18][9], int row, int col) {
  printBCD(d[row][col+1]);
  io_puts(":");
  printBCD(d[row][col]);
}

void ICACHE_FLASH_ATTR fpr_printPrgPacketInfo(uint8_t d[FER_PRG_PACK_CT][FER_PRG_BYTE_CT], bool rtc_only) {
  int row, col;

const char *wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };

  io_puts("RTC: ");
  printBCD(d[0][fpr0_RTC_days]);
  io_puts(".");
  printBCD(d[0][fpr0_RTC_mont]);
  io_puts(".  ");
  io_puts(wdays[7 & d[0][fpr0_RTC_wday]]);
  io_puts("  ");
  printBCD(d[0][fpr0_RTC_hour]);
  io_puts(":");
  printBCD(d[0][fpr0_RTC_mint]);
  io_puts(":");
  printBCD(d[0][fpr0_RTC_secs]);
  if (GET_BIT(d[0][fpr0_FlagBits], flag_DST))
	  io_puts(" DST");
 io_putlf();



  if (rtc_only)
  return;

  if (GET_BIT(d[0][fpr0_FlagBits], flag_SunAuto)) {
    io_puts("Sun Automatic is on\n");
  }

  if (GET_BIT(d[0][fpr0_FlagBits], flag_Random)) {
    io_puts("Random Mode is on\n");
  }


  io_puts("Timer data:\n");
  for (row = FPR_TIMER_START_ROW; row < (FPR_TIMER_START_ROW + FPR_TIMER_HEIGHT); ++row) {
    for (col = 0; (col+1) < FPR_TIMER_WIDTH; col += 2) {
      printTimerStamp(d, row, col);
      io_puts(", ");
    }
    io_putlf();
  }

  io_puts("Astro data:\n");
  for (row = FPR_ASTRO_START_ROW; row < (FPR_ASTRO_START_ROW + FPR_ASTRO_HEIGHT); ++row) {
    for (col = 0; (col+1) < FPR_ASTRO_WIDTH; col += 2) {
      //    for (col = (FPR_ASTRO_WIDTH - 2); col >= 0; col -= 2) {
      printTimerStamp(d, row, col);
      io_puts(", ");
    }
    io_putlf();
  }
  
}


void ICACHE_FLASH_ATTR fmsg_print(const fer_msg *msg, fmsg_type t) {

  frb_printPacket(msg->cmd);

#ifndef FER_RECEIVER_MINIMAL
  if (t == MSG_TYPE_RTC || t == MSG_TYPE_TIMER) {
    int i, used_lines;
    fmsg_data prg = fmsg_get_data(msg);

    used_lines = t == MSG_TYPE_RTC ? FER_RTC_PACK_CT : FER_PRG_PACK_CT;
    if (C.app_verboseOutput >= vrbDebug) {
      for (i = 0; i < used_lines; ++i) {
        print_array_8(prg[i], FER_PRG_BYTE_CT);
      }
    } else {
      fpr_printPrgPacketInfo(prg, used_lines == 1);
    }

  }
#endif
}

void ICACHE_FLASH_ATTR fmsg_print_as_cmdline(const fer_msg *msg, fmsg_type t) {
  const fer_sender_basic *fsb = (fer_sender_basic*) msg;

  if (t != MSG_TYPE_PLAIN)
    return; // ignore long messages for now

  fer_cmd c = FSB_GET_CMD(fsb);
  uint32_t id = FSB_GET_DEVID(fsb);

  const char *cs = NULL;
  const char *fdt = NULL;

  if ((FSB_ADDR_IS_PLAIN(fsb) && (fdt = "plain"))
      || (FSB_ADDR_IS_CENTRAL(fsb) && (fdt = "central"))) {
    switch (c) {
    case fer_cmd_DOWN:
      cs = "down";
      break;
    case fer_cmd_UP:
      cs = "up";
      break;
    case fer_cmd_STOP:
      cs = "stop";
      break;
    default:
      cs = NULL;
      break;
    }
  } else if (FSB_ADDR_IS_SUNSENS(fsb) && (fdt = "sun")) {
    switch (c) {
    case fer_cmd_SunDOWN:
      cs = "sun-down";
      break;
    case fer_cmd_SunUP:
      cs = "sun-up";
      break;
    case fer_cmd_SunINST:
      cs = "sun-pos";
      break;    default:
      cs = NULL;
      break;
    }
  }

  if (!cs)
    return; // unsupported command

  io_puts("type="), io_puts(fdt),
  io_puts(" a="), io_print_hex(id, false);
  if (FSB_ADDR_IS_CENTRAL(fsb)) {
    uint8_t g = FSB_GET_GRP(fsb);
    uint8_t m = FSB_GET_MEMB(fsb);
    if (g != 0) {
      io_puts(" g="), io_putd(g);
      if (m != 0) {
        m -= 7;
        io_puts(" m="), io_putd(m);
      }
    }
  }
  io_puts( " c="), io_puts(cs);
  io_puts(";\n");
}


