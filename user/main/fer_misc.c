/*
 * fer_misc.c
 *
 * Created: 12.09.2017 01:40:05
 *  Author: bertw
 */ 

#include "all.h"

void ICACHE_FLASH_ATTR
frb_printPacket(const uint8_t *dg) {
	int i;

  dbg_trace();
  for (i = 0; i < bytesPerCmdPacket; ++i) {
    io_print_hex_8(dg[i], true);
  }
  io_puts("\n");
}

void ICACHE_FLASH_ATTR
printTimerStamp(uint8_t d[18][9], int row, int col) {
  printBCD(d[row][col+1]);
  io_puts(":");
  printBCD(d[row][col]);
}

void ICACHE_FLASH_ATTR fpr_printPrgPacketInfo(uint8_t d[FER_PRG_PACK_CT][FER_PRG_BYTE_CT], bool rtc_only) {
  const char *const lf = "\n";
  int row, col;

const char *wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };

  io_puts("RTC: ");
  printBCD(d[0][fpr0_RTC_days]);
  io_puts(".");
  printBCD(d[0][fpr0_RTC_mont]);
  io_puts(".  ");
  io_puts(wdays[d[0][GET_LOW_NIBBLE(fpr0_RTC_wday)]]);
  io_puts("  ");
  printBCD(d[0][fpr0_RTC_hour]);
  io_puts(":");
  printBCD(d[0][fpr0_RTC_mint]);
  io_puts(":");
  printBCD(d[0][fpr0_RTC_secs]);
  io_puts(lf);

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
    io_puts(lf);
  }

  io_puts("Astro data:\n");
  for (row = FPR_ASTRO_START_ROW; row < (FPR_ASTRO_START_ROW + FPR_ASTRO_HEIGHT); ++row) {
    for (col = 0; (col+1) < FPR_ASTRO_WIDTH; col += 2) {
      //    for (col = (FPR_ASTRO_WIDTH - 2); col >= 0; col -= 2) {
      printTimerStamp(d, row, col);
      io_puts(", ");
    }
    io_puts(lf);
  }
  
}

