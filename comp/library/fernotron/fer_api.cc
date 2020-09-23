#include "fer_api.h"
#include "misc/bcd.h"
#include "fernotron/fer_rawmsg_buffer.h"
#include "misc/int_macros.h"
#include <stdlib.h>
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
      if (i < FER_FPR_TIMER_STAMP_WIDTH || i + FER_FPR_TIMER_STAMP_WIDTH > size_dst)
        return false;
      dst[i] = dst[i - FER_FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FER_FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FER_FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FER_FPR_TIMER_STAMP_WIDTH];
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

bool fill_rtc_buf(fer_sbT *fsb, time_t rtc) {
  (void)fsb;
  fer_msg_raw_init(fer_tx_msg);
  fer_msg_raw_from_rtc(fer_tx_msg, rtc, true);
  return true;
}

bool fill_timer_buf(fer_sbT *fsb, time_t rtc, Fer_TimerData *tdr) {

  fer_msg_raw_init(fer_tx_msg);

  fer_msg_raw_from_rtc(fer_tx_msg, rtc, false);


  if (fer_td_is_weekly(tdr)) {
    u8 weekly_data[FER_FPR_TIMER_STAMP_WIDTH * 7];
    if (!timerString2bcd(tdr->weekly, weekly_data, sizeof weekly_data))
      return false;
    fer_msg_raw_from_weeklyTimer(fer_tx_msg, weekly_data);
  }

  if (fer_td_is_daily(tdr)) {
    u8 daily_data[FER_FPR_TIMER_STAMP_WIDTH];
    if (!timerString2bcd(tdr->daily, daily_data, sizeof daily_data))
      return false;
    fer_msg_raw_from_dailyTimer(fer_tx_msg, daily_data);
  }

  if (fer_td_is_astro(tdr)) {
    fer_msg_raw_from_astro(fer_tx_msg, tdr->astro);
  }

  if (fer_td_is_random(tdr)) {
    fer_msg_raw_from_flags(fer_tx_msg, BIT(flag_Random), BIT(flag_Random));
  }

  if (fer_td_is_sun_auto(tdr)) {
    fer_msg_raw_from_flags(fer_tx_msg, BIT(flag_SunAuto), BIT(flag_SunAuto));
  }

  fer_msg_raw_footerCreate(fer_tx_msg, fsb);

  return true;
}

bool fer_send_rtc_message(fer_sbT *fsb, time_t rtc) {
  bool success = fill_rtc_buf(fsb, rtc) && fer_send_msg(fsb, MSG_TYPE_RTC, 0);
  return success;
}

bool fer_send_timer_message(fer_sbT *fsb, time_t rtc, Fer_TimerData *td) {
  bool success = fill_timer_buf(fsb, rtc, td) && fer_send_msg(fsb, MSG_TYPE_TIMER, 0);
  return success;
}

bool fer_send_empty_timer_message(fer_sbT *fsb, time_t rtc) {
  Fer_TimerData tde = fer_td_initializer;
  bool success = fill_timer_buf(fsb, rtc, &tde) && fer_send_msg(fsb, MSG_TYPE_TIMER, 0); // XXX: wasteful
  return success;
}



