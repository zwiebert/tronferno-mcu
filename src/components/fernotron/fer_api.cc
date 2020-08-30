#include "fer_api.h"
#include "misc/bcd.h"
#include "fernotron/fer_rawmsg_buffer.h"
#include "misc/int_macros.h"
#include <string.h>

bool
timerString2bcd(const char *src, u8 *dst, u16 size_dst) {
  char buf[3];
  int i;

  buf[2] = 0;

  for (i = 0; i < size_dst && src[0]; ++i) {
    switch (*src) {
    case '-': // timer off
      dst[i] = (i % 2 == 0) ? 0xff : 0x0f;
      if (!(++i < size_dst))
        return false;
      ;
      dst[i] = (i % 2 == 0) ? 0xff : 0x0f;
      ++src;
      break;
    case '+': // copy previous day on+off timer
      if (i < FPR_TIMER_STAMP_WIDTH || i + FPR_TIMER_STAMP_WIDTH > size_dst)
        return false;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH];
      ++src;
      break;

    default:
      buf[0] = *src++;
      buf[1] = *src++;
      if (!(++i < size_dst))
        return false;
      ;
      dst[i] = dec2bcd(atoi(buf));
      buf[0] = *src++;
      buf[1] = *src++;
      dst[i - 1] = dec2bcd(atoi(buf));

    }

  }

  return i == size_dst;
}

bool fill_rtc_buf(fsbT *fsb, time_t rtc) {
  fmsg_raw_init(txmsg);
  fmsg_raw_from_rtc(txmsg, rtc, true);
  return true;
}

bool fill_timer_buf(fsbT *fsb, time_t rtc, timer_data_t *tdr) {

  fmsg_raw_init(txmsg);

  fmsg_raw_from_rtc(txmsg, rtc, false);


  if (td_is_weekly(tdr)) {
    u8 weekly_data[FPR_TIMER_STAMP_WIDTH * 7];
    if (!timerString2bcd(tdr->weekly, weekly_data, sizeof weekly_data))
      return false;
    fmsg_raw_from_weeklyTimer(txmsg, weekly_data);
  }

  if (td_is_daily(tdr)) {
    u8 daily_data[FPR_TIMER_STAMP_WIDTH];
    if (!timerString2bcd(tdr->daily, daily_data, sizeof daily_data))
      return false;
    fmsg_raw_from_dailyTimer(txmsg, daily_data);
  }

  if (td_is_astro(tdr)) {
    fmsg_raw_from_astro(txmsg, tdr->astro);
  }

  if (td_is_random(tdr)) {
    fmsg_raw_from_flags(txmsg, BIT(flag_Random), BIT(flag_Random));
  }

  if (td_is_sun_auto(tdr)) {
    fmsg_raw_from_flags(txmsg, BIT(flag_SunAuto), BIT(flag_SunAuto));
  }

  fmsg_raw_footerCreate(txmsg, fsb);

  return true;
}

bool send_rtc_message(fsbT *fsb, time_t rtc) {
  bool success = fill_rtc_buf(fsb, rtc) && fer_send_msg(fsb, MSG_TYPE_RTC, 0);
  return success;
}

bool send_timer_message(fsbT *fsb, time_t rtc, timer_data_t *td) {
  bool success = fill_timer_buf(fsb, rtc, td) && fer_send_msg(fsb, MSG_TYPE_TIMER, 0);
  return success;
}

bool send_empty_timer_message(fsbT *fsb, time_t rtc) {
  timer_data_t tde = td_initializer;
  bool success = fill_timer_buf(fsb, rtc, &tde) && fer_send_msg(fsb, MSG_TYPE_TIMER, 0); // XXX: wasteful
  return success;
}



