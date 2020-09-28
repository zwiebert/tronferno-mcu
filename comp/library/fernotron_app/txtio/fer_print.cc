/*
 * fer_misc.c
 *
 * Created: 12.09.2017 01:40:05
 *  Author: bertw
 */ 




#include <stdint.h>
#include "stdbool.h"

#include "fernotron/trx/raw/fer_radio_timings.h"
#include "fernotron/trx/raw/fer_fsb.h"
#include "fernotron/trx/raw/fer_msg_attachment.h"
#include "misc/int_macros.h"
#include "txtio/inout.h"
#include <fernotron/trx/fer_msg_send.hh>
#include <fernotron/trx/fer_trx_incoming.hh>

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;


// diagnostic output
static void frb_printPacket(const union fer_cmd_row *cmd);
static void fpr_printPrgPacketInfo(uint8_t d[FER_PRG_PACK_CT][FER_PRG_BYTE_CT], bool rtc_only);

static void frb_printPacket(const union fer_cmd_row *cmd) {
  int i;

  for (i = 0; i < FER_CMD_BYTE_CT; ++i) {
    io_print_hex_8(cmd->bd[i], true);
  }
  io_putlf();
}

static void
printTimerStamp(u8 d[18][9], int row, int col) {
  printBCD(d[row][col+1]);
  io_puts(":");
  printBCD(d[row][col]);
}

static void  fpr_printPrgPacketInfo(u8 d[FER_PRG_PACK_CT][FER_PRG_BYTE_CT], bool rtc_only) {
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
  for (row = FER_FPR_TIMER_START_ROW; row < (FER_FPR_TIMER_START_ROW + FER_FPR_TIMER_HEIGHT); ++row) {
    for (col = 0; (col+1) < FER_FPR_TIMER_WIDTH; col += 2) {
      printTimerStamp(d, row, col);
      io_puts(", ");
    }
    io_putlf();
  }

  io_puts("Astro data:\n");
  for (row = FER_FPR_ASTRO_START_ROW; row < (FER_FPR_ASTRO_START_ROW + FER_FPR_ASTRO_HEIGHT); ++row) {
    for (col = 0; (col+1) < FER_FPR_ASTRO_WIDTH; col += 2) {
      //    for (col = (FPR_ASTRO_WIDTH - 2); col >= 0; col -= 2) {
      printTimerStamp(d, row, col);
      io_puts(", ");
    }
    io_putlf();
  }
  
}

//cast data (message after cmd) to byte array
#define fer_msg_get_data(msg) ((u8(*)[FER_PRG_BYTE_CT])(msg)->rtc.bd)
typedef u8(*fer_msg_data)[FER_PRG_BYTE_CT];

void  fer_msg_print(const char *tag, const fer_rawMsg *msg, fer_msg_kindT t, bool verbose) {

  io_puts(tag);
  frb_printPacket(&msg->cmd);

#ifndef FER_RECEIVER_MINIMAL
  if (t == fer_msg_kindT::MSG_TYPE_RTC || t == fer_msg_kindT::MSG_TYPE_TIMER) {
    int i, used_lines;
    fer_msg_data prg = fer_msg_get_data(msg);

    used_lines = t == fer_msg_kindT::MSG_TYPE_RTC ? FER_RTC_PACK_CT : FER_PRG_PACK_CT;
    if (verbose) {
      for (i = 0; i < used_lines; ++i) {
        print_array_8(prg[i], FER_PRG_BYTE_CT);
      }
    } else {
      fpr_printPrgPacketInfo(prg, used_lines == 1);
    }

  }
#endif
}

void  fer_msg_print_as_cmdline(const char *tag, const fer_rawMsg *msg, fer_msg_kindT t) {
  const fer_sbT *fsb = (fer_sbT*) msg;

  if (t != fer_msg_kindT::MSG_TYPE_PLAIN && t !=  fer_msg_kindT::MSG_TYPE_PLAIN_DOUBLE)
    return; // ignore long messages for now



  fer_if_cmd c = (fer_if_cmd)FER_SB_GET_CMD(fsb);
  u32 id = FER_SB_GET_DEVID(fsb);

  const char *cs = 0;
  const char *fdt = 0;

  if ((FER_SB_ADDR_IS_PLAIN(fsb) && (fdt = "plain"))
      || (FER_SB_ADDR_IS_CENTRAL(fsb) && (fdt = "central"))) {
    switch (c) {
    case fer_if_cmd_DOWN:
      cs = "down";
      break;
    case fer_if_cmd_UP:
      cs = "up";
      break;
    case fer_if_cmd_STOP:
      cs = "stop";
      break;
    default:
      cs = 0;
      break;
    }
  } else if (FER_SB_ADDR_IS_SUNSENS(fsb) && (fdt = "sun")) {
    switch (c) {
    case fer_if_cmd_SunDOWN:
      cs = "sun-down";
      break;
    case fer_if_cmd_SunUP:
      cs = "sun-up";
      break;
    case fer_if_cmd_SunINST:
      cs = "sun-pos";
      break;    default:
      cs = 0;
      break;
    }
  }

  if (!cs)
    return; // unsupported command

  io_puts(tag);

  io_puts("type="), io_puts(fdt),
  io_puts(" a="), io_print_hex(id, false);
  if (FER_SB_ADDR_IS_CENTRAL(fsb)) {
    u8 g = FER_SB_GET_GRP(fsb);
    u8 m = FER_SB_GET_MEMB(fsb);
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


