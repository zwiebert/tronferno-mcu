/**
 * \file   fernotron/trx/timer_data.h
 * \brief  vocabulary types to program Fernotron automatic (timers)
 */


#pragma once
#include <app/config/proj_app_cfg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <debug/dbg.h>

/// Vocabulary type to represent Fernotron automatic movements
struct Fer_TimerData {
  enum { DAILY_MAX_LEN = (8), WEEKLY_MAX_LEN = (8 * 7)};
  /**
   * \brief Set daily timer.
   *
   * T  - T is a 8 digit time string like 07302000. The four left digits are the up-time. The four on the right the down-time. A minus sign can replace 4 digits, which means the timer is cleared.
   *
   * timer daily=07302000;   up 07:30, down 20:00
   * timer daily=0730-;      up 07:30, not down
   * timer daily=-2000;      not up,   down 20:00
   */

  void putDaily(const char *dt) {
    precond(!dt || strlen(dt) <= DAILY_MAX_LEN);
    if (!dt)
      daily[0] = '\0';
    else
      STRLCPY(daily, dt, sizeof daily);
  }
  bool hasDaily() const {
    return daily[0] != '\0';
  }
  const char* getDaily() const {
    return daily;
  }

  /**
   * \brief Set weekly timer.
   *
   * weekly=TTTTTTT - sets a timer for each week day. week days are from left to right: Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
   *
   * T - Each T is a 8 digit time string like described above with daily option.  A plus sign repeats the previous T.  So you can copy the values from Monday to Tuesday and so on.
   *
   * timer weekly=0730-++++0900-+;    up Monday-Friday at 07:30, and Saturday-Sunday at 09:00
   *
   *
   */
  void putWeekly(const char *wt) {
    precond(!wt || strlen(wt) <= WEEKLY_MAX_LEN);
    if (!wt)
      weekly[0] = '\0';
    else
      STRLCPY(weekly, wt, sizeof weekly);
  }
  bool hasWeekly() const {
    return weekly[0] != '\0';
  }
  const char* getWeekly() const {
    return weekly;
  }

  /**
   *  \brief            Set civil dusk ("astro") timer.
   *  \param minOffset  minute offset to actual civil dusk. can be positive or negative
   *
   *
   *  putAstro(-30);    closes shutter thirty minutes before civil dusk`
   *
   *  putAstro(+120);   closes shutter 2 hours after civil dusk`
   */
  void putAstro(int minOffset) {
    astro = minOffset;
    flags.disableAstro = false;
  }

  /// \brief Enable/disable civil dusk timer.
  void putAstro(bool enable, int minOffset = 0) {
    flags.disableAstro = enable;
    astro = minOffset;
  }
  bool hasAstro() const {
    if(flags.disableAstro)
      return false;
    if (astro == 20000)  // for backwards compatibility with stored timer data
      return false;
    return true;
  }
  int getAstro() const {
    return astro;
  }


  /// \brief Enable/disable built-in random timer.
  void putRandom(bool enable) {
    flags.enableRandom = enable;
  }
  bool getRandom() const {
    return flags.enableRandom;
  }

  /// \brief   Enable/disable sun shade automatic.
  void putSunAuto(bool enable) {
    flags.enableSunAuto = enable;
  }
  bool getSunAuto() const {
    return flags.enableSunAuto;
  }

private:
  int16_t astro = 20000;      // minute offset of civil dusk, or 20000 for disables astro
  union {
    /// \brief Flags to enable/disable the boolean automatics.
    struct {
      uint8_t enableRandom :1;
      uint8_t unused_1 :1;
      uint8_t dst :1;
      uint8_t unused_3 :1;
      uint8_t unused_4 :1;
      uint8_t unused_5 :1;
      uint8_t disableAstro :1;
      uint8_t enableSunAuto :1;
    } flags;
    uint8_t bf = 0;         // bitfield
  };
  char daily[DAILY_MAX_LEN + 1] = "";    // ASCII string of one daily timer
  char weekly[WEEKLY_MAX_LEN + 1] = ""; // ASCII string of seven weekly timers
};
