/**
 * \file   fernotron_trx/timer_data.h
 * \brief  vocabulary types to program Fernotron automatic (timers)
 */


#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <utils_misc/cstring_utils.hh>
#include <debug/dbg.h>

/// Vocabulary type to represent Fernotron automatic movements
struct Fer_TimerData {
  enum { DAILY_MAX_LEN = (8), WEEKLY_MAX_LEN = (8 * 7)};

  static bool validateTime(const char *t) {
    if (strlen(t) < 4)
      return false;
    const int h10 = t[0] - '0';
    const int h1 = t[1] - '0';
    const int m10 = t[2] - '0';
    const int m1 = t[3] - '0';

    return (0 <= h10 && h10 <= 2) && (0 <= h1 && h1 <= 9) && (h10 != 2 || h1 <= 3) //
        && (0 <= m10 && m10 <= 5) && (0 <= m1 && m1 <= 9);
  }

  static bool validateTimePair(const char *tp) {
    if (strncmp(tp, "--", 2) == 0)
      return false;
    if (strlen(tp) < 5)
      return false;


    if (tp[0] == '-') {
      return validateTime(tp+1);
    }
    if (!validateTime(tp))
      return false;

    return tp[4] == '-' || validateTime(tp + 4);
  }

  /**
   * \brief Set daily timer.
   *
   * T  - T is a 8 digit time string like 07302000. The four left digits are the up-time. The four on the right the down-time. A minus sign can replace 4 digits, which means the timer is cleared.
   *
   * timer daily=07302000;   up 07:30, down 20:00
   * timer daily=0730-;      up 07:30, not down
   * timer daily=-2000;      not up,   down 20:00
   */

  bool putDaily(const char *dt) {
    precond(!dt || strlen(dt) <= DAILY_MAX_LEN);
    if (!dt || !*dt || strcmp(dt, "null") == 0  || strcmp(dt, "--") == 0) {
      daily[0] = '\0';
      return true;
    }
    if (!validateDaily(dt))
      return false;

    STRLCPY(daily, dt, sizeof daily);
    return true;
  }
  bool hasDaily() const {
    return daily[0] != '\0';
  }
  const char* getDaily() const {
    return daily;
  }

  bool validateDaily(const char *dt) {
    int count_minus = 0;
    int count_digit = 0;

    int i = 0;
    for (const char *p = dt; *p; ++p, ++i) {
      if (*p == '-') {
        ++count_minus;
      } else if (isdigit(*p)) {
        if ((count_digit & 3) == 0 && !validateTime(p))
          return false;

        ++count_digit;
      } else {
        return false;
      }
    }
    if (DAILY_MAX_LEN != (count_digit + 4 * count_minus))
      return false;

    if (count_digit == 0)
      return false;

    if (!validateTimePair(dt))
      return false;

    return true;
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
  bool putWeekly(const char *wt) {
    precond(!wt || strlen(wt) <= WEEKLY_MAX_LEN);

    if (!wt || !*wt || strcmp(wt, "null") == 0 || strcmp(wt, "--++++++") == 0) {
      weekly[0] = '\0';
      return true;
    }

    if (!validateWeekly(wt))
      return false;

    STRLCPY(weekly, wt, sizeof weekly);
    return true;
  }
  
  bool hasWeekly() const {
    return weekly[0] != '\0';
  }
  const char* getWeekly() const {
    return weekly;
  }
  bool validateWeekly(const char *wt) {
    int count_minus = 0;
    int count_plus = 0;
    int count_digit = 0;


    int i = 0;
    for (const char *p = wt; *p; ++p, ++i) {
      if (*p == '-') {
        ++count_minus;
      } else if (*p == '+') {
        if (i == 0)
         return false;
        else if ((count_digit + 4 * count_minus) & 7)
         return false;

        ++count_plus;
      } else if (isdigit(*p)) {
        ++count_digit;
      } else {
        return false;
      }
    }
    if (WEEKLY_MAX_LEN != (count_digit + 4 * count_minus + 8 * count_plus))
      return false;

    return true;
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
    flags.disableAstro = !enable;
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

public:
  int to_json(char *dst, size_t dst_size, bool f_manual) const {
    auto n = snprintf(dst, dst_size, //
        R"({"f":"%c%c%c%c%c%c","daily":"%s","weekly":"%s","astro":%d})", //

        f_manual ? 'M' : 'm', getRandom() ? 'R' : 'r', getSunAuto() ? 'S' : 's', //
        hasAstro() ? 'A' : 'a', hasDaily() ? 'D' : 'd', hasWeekly() ? 'W' : 'w', //
        daily, weekly, astro);

    return n < dst_size ? n : 0;
  }
#if 0
  bool from_json(const char *json);
  bool from_json(JsmnBase::Iterator &it);
#endif
};
