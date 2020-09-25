/*
* astro.c
*
* Created: 24.08.2017 22:02:54
*  Author: bertw
*/

#include <fernotron/fer_msg_plain.h>
#include <fernotron/fer_msg_attachment.h>
#include <stdlib.h>
#include <math.h>
#include "stdbool.h"

#include "fer_app_cfg.h"
#include "fer_api.h"

#include "fernotron/astro.h"
#include "misc/sun.h"
#include "misc/bcd.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"

#include <string.h>

static struct cfg_astro astro_config;
#define astro_cfg (&astro_config)

/*
 *
 * force_even_minutes  - if true, no odd number will be outputted in dstMinutes
 */
static void 
time_to_bcd(u8 *bcdMinutes, u8 *bcdHours, double time, bool force_even_minutes) {
  double integral, fractional;

  fractional = modf(time, &integral);

  u8 hours = (u8) integral;
  u8 minutes = (u8) (fractional * 60);

  if (force_even_minutes && minutes == 59) {
    ++hours;
    minutes = 0;
  }


  if (bcdHours) {
    *bcdHours = dec2bcd_special(hours);
  }

  if (bcdMinutes) {
    *bcdMinutes = dec2bcd_special((force_even_minutes ? ~1 : ~0) & (1+(u8) minutes));
  }

}

static u16 astro_minutes[48];

static int astroTableIndex_from_tm(const struct tm *tm) {
  int m = tm->tm_mon + 1;
  int d = tm->tm_mday;

  int feb = m > 2 ? -2 : 0; // february has only 28 days

  int idx = ((m - 1) * 30 + feb + d + 2) / 4 + 2;
  if (idx < 0 || idx > 47)
    idx = ((6 - (m - 1)) * 30 - d + 1) / 4 + 48 - 5;
  if (idx < 0)
    idx = idx * -1;
  return idx;
}

void astro_populate_astroMinutes_from_astroBcd() {
  u8 row, col, midx=0;

  fer_astro_byte_data bcds;

  fer_astro_write_data(bcds, 0);

  for (row = 0; row < FER_FPR_ASTRO_HEIGHT; ++row) {
    for (col = 0; (col+1) < FER_FPR_ASTRO_WIDTH; col += 2) {
      u8 min = bcd2dec(bcds[row][col]);
      u8 hour = bcd2dec(bcds[row][col+1]);
     astro_minutes[midx++] = hour * 60 + min;
    }
  }
}

u16 fer_astro_calc_minutes(const struct tm *tm) {
  int idx = astroTableIndex_from_tm(tm);
  assert(0 <= idx && idx <= 47);
  return astro_minutes[idx] + (tm->tm_isdst ? 60 : 0);
}


#if 1

/*
 *  Calculate civil dusk for configured location on earth and fill in "astro"/dusk table ready to send to Fernotron receiver
 *
 *
 *  there is a problem because of how the table is interpreted by the motor:
 *
 *  the table contains only half a year. one entry for span of usually 4 days.
 *
 *  these spans are shared for both half years.
 *  Unfortunately the shared dates are not always matching in times for sunrise/sunset.
 *
 *  e.g. the entry for the span January 14..17 is shared with November 26..29, which differs by 20 minutes at the place where I live.
 *  This function calculates the average for each values to prevent the error from becoming too big.
 *
 *
 */
static void 
math_write_astro(fer_astro_byte_data dst, int mint_offset) {
  int i, j;
  double dusk = 0, duskf = 0, duskr = 0, last_dusk = -1;
  int dayf, dayr;

  dayf = dayr = 354;

  for (i = 0; i < FER_FPR_ASTRO_HEIGHT; ++i) {
    for (j = 0; j < 4; ++j) {
      sun_calculateDuskDawn(NULL, &duskf, astro_cfg->geo_timezone + (mint_offset / 60.0f), dayf, astro_cfg->geo_longitude, astro_cfg->geo_latitude, CIVIL_TWILIGHT_RAD);
      sun_calculateDuskDawn(NULL, &duskr, astro_cfg->geo_timezone + (mint_offset / 60.0f), dayr, astro_cfg->geo_longitude, astro_cfg->geo_latitude, CIVIL_TWILIGHT_RAD);

      switch (astro_cfg->astroCorrection) {
      case acAverage:
        dusk = (duskf + duskr) / 2;
        break;
      case acMinimum:
        dusk = MIN(duskf, duskr);
        break;
      case acMaximum:
        dusk = MAX(duskf, duskr);
        break;
      }

      if (dusk < last_dusk) {
        dusk = last_dusk;
      } else {
        last_dusk = dusk;
      }
      time_to_bcd(&dst[i][j * 2], &dst[i][j * 2 + 1], dusk, true);

      dayf += 4;
      if (dayf > 365)
        dayf = 2;
      dayr -= 4;
    }
  }
}
#elif 1


#if 0
u16 fer_astro_calc_minutes(const struct tm *tm) {
  double dusk;
  i16 old_yd, yd = old_yd = (tm->tm_mon * 30 + tm->tm_mday);
  i16 idx;



  if (1 <= yd && yd <= 173) {
    idx = (yd + 2) / 4 + 2;
    yd = 352 - (4 * idx);
  } else {
    idx = abs((352 - yd) / 4);
    yd = yd / 4 * 4;
  }

  ets_printf("idx: %d, yd: %d (old: %d)\n", idx, yd, old_yd);

  assert(0 <= idx && idx <= 47);

  double dayofy = yd * 1.0139;
  sun_calculateDuskDawn(NULL, &dusk, astro_cfg->geo_timezone + (tm->tm_isdst ? 1 : 0), dayofy, astro_cfg->geo_longitude, astro_cfg->geo_latitude, CIVIL_TWILIGHT_RAD);
  u16 minutes = dusk * 60;
  return minutes;
}
#endif

static void 
math_write_astro(fer_astro_byte_data dst, int mint_offset) {
  int i, j, yd;

  for (i = 0, yd=88; i < FER_FPR_ASTRO_HEIGHT; ++i) { // 4*88=352
    for (j = 0; j < 4; ++j) {
    double dusk;
    double dayofy = (4 * yd--) * 1.0139; // 360 => 365 days per year

    sun_calculateDuskDawn(NULL, &dusk, astro_cfg->geo_timezone + (mint_offset / 60.0f), dayofy, astro_cfg->geo_longitude, astro_cfg->geo_latitude, CIVIL_TWILIGHT_RAD);
    time_to_bcd(&dst[i][j * 2], &dst[i][j * 2 + 1], dusk, true);
    }
  }
 }

#else

// experimental code

static int get_yday(int mon, int day)
{
    static const int days[13] = {
        0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
    };
    return days[mon] + day;
}


static void 
math_write_astro(fer_astro_byte_data dst, int mint_offset) {
  int i, j, month, mday;
  double sunset = 17.0;

  month = 12;
  mday = 20;

  for (i = 0; i < FER_FPR_ASTRO_HEIGHT; ++i) {
    for (j = 0; j < 4; ++j) {
      sun_calculateDuskDawn(NULL, &sunset, astro_cfg->geo_timezone + (mint_offset / 60.0f), get_yday(month, mday), astro_cfg->geo_longitude, astro_cfg->geo_latitude, CIVIL_TWILIGHT_RAD);

      time_to_bcd(&dst[i][j * 2], &dst[i][j * 2 + 1], sunset, true);

      mday -= 4;
      if (mday < 2) {
        month -= 1;
        mday = (month == 9 || month == 6) ? 28 : 30;
      }
    }
  }
}

#endif
/////////////////////////////////////////////////////////////////////////////////////


void  fer_astro_write_data(u8 d[FER_FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset) {
  math_write_astro(d, mint_offset);
}

void fer_astro_init_and_reinit(const struct cfg_astro *cfg_astro) {
  if (cfg_astro) {
    memcpy(&astro_config, cfg_astro, sizeof astro_config);
  }

  astro_populate_astroMinutes_from_astroBcd();
}


/////////////////////////////////////////////////////////////////////////////////////



// real times: 2017-10-22T18:27:00 (DST)  // the device interpolates times from the table

#if 0
// table generated by original hardware for berlin, germany
// table should contain only even minutes
// each table entry should be greater or equal than its predecessor
const u8 ad_plz_10[12][8]  = {
//
{0x34, 0x16, 0x36, 0x16, 0x36, 0x16, 0x38, 0x16, }, // jan1, dec2
//
{0x40, 0x16, 0x44, 0x16, 0x48, 0x16, 0x52, 0x16, }, // jan2, dec1
//
{0x58, 0x16, 0x04, 0x17, 0x10, 0x17, 0x16, 0x17, }, // feb1, nov2
//
{0x24, 0x17, 0x30, 0x17, 0x38, 0x17, 0x46, 0x17, }, // feb2, nov1
//
{0x52, 0x17, 0x00, 0x18, 0x08, 0x18, 0x18, 0x18, }, // mar1, oct2
//
{0x26, 0x18, 0x34, 0x18, 0x42, 0x18, 0x50, 0x18, }, // mar2, oct1
//
{0x58, 0x18, 0x06, 0x1f, 0x16, 0x1f, 0x24, 0x1f, }, // apr1, sept2
//
{0x32, 0x1f, 0x42, 0x1f, 0x50, 0x1f, 0x58, 0x1f, }, // apr2, sept1
//
{0x08, 0x20, 0x16, 0x20, 0x24, 0x20, 0x32, 0x20, }, // may1, aug2
//
{0x40, 0x20, 0x46, 0x20, 0x54, 0x20, 0x00, 0x21, }, // may2, aug1
//
{0x06, 0x21, 0x10, 0x21, 0x14, 0x21, 0x18, 0x21, }, // jun1, jul2
//
{0x20, 0x21, 0x20, 0x21, 0x20, 0x21, 0x20, 0x21, }, // jun2, jul1
};

#endif

// berlin actual civil dusk: 16:33 ... 21:23 (GMT + 1)
// berlin fernotron table:  16:34 ... 21:20

/*
 * table found by trying every entry.
 * the problem: the table is only half a year long and the
 * days which share the same entry does not really have the same
 * time for sunset according to the formula and tables on the internet.
 *
 * e.g.
 *
 *  civil dusk in my town on:
 * january 14: 17:00
 * november 26:  16:40
 *
 * a difference by 20 minutes, but these share the same table entry
 *
 *
 *
 *
 * start values when counting days backwards
 * month: start-value
 * 12: 20
 * 11: 30
 * 10: 30
 * 09: 28
 * 08: 30
 * 07: 30
 * 06: 28
 *
*/

 /*

 350    354    358    2     // year of day based on 30 day per month
12/20  12/24  12/28  01/02  // dates from 12/20..23 ... 06/20..23 forward
17:00, 17:06, 17:12, 17:18, // (generated test times. unimportant)
12/20  12/16  12/12  12/08  // dates from 12/23..20 ... 06/27..24 backward
 350    346    342    338   // year of day based on 30 day per month
 352    348  ...             // yod indices

  6      10     14     18
01/06  01/10  01/14  01/18
17:24, 17:30, 17:36, 17:42,
12/04  11/30  11/26  11/22
 334    330    326    322

  22     26     30     34
01/22  01/26  01/30  02/04
17:48, 17:54, 18:00, 18:06,
11/18  11/14  11/10  11/06
 318    314    310    306

 38    42      46     50
02/08  02/12  02/16  02/20
18:12, 18:18, 18:24, 18:30,
11/02  10/30  10/26  10/22
 302    300    296    292

  54     58     64    68
02/24  02/28  03/04  03/08
18:36, 18:42, 18:48, 18:54,
10/18  10/14  10/10  10/06
 288    284    280    276

  72     76     80    84
03/12  03/16  03/20  03/24
1f:00, 1f:06, 1f:12, 1f:18,
10/02  09/28  09/24  09/20
 272    268    264    260

######middle################

  88            94    98
03/28         04/04  04/08   --- skipped entry
1f:24, 1f:30, 1f:36, 1f:42,
09/16  09/12  09/08  09/04
 256    252    248    244

 102    106    110    114
04/12  04/16  04/20  04/24
1f:48, 1f:54, 20:00, 20:06,
08/30  08/26  08/22  08/18
 240    236    232    228


 118    122    126    130
04/28  05/02  05/06  05/10
20:12, 20:18, 20:24, 20:30,
08/14  08/10  08/06  08/02   ---- 07/30 ---> 08/02
  224   220    216    212
               218    214   // end of yod indexes witch +=4

 134    138    142    146
05/14  05/18  05/22  05/26
20:36, 20:42, 20:48, 20:54,
07/30  07/26  07/22  07/18
 210    206    202    198

 150    154    158    162
05/30  06/04  06/08  06/12
21:00, 21:06, 21:12, 21:18,
07/14  07/10  07/06  07/02
 194     190    186   182

 164    168   (172)  (176) // begin of yod indices
 166    170
06/16  06/20
21:24, 21:30, 21:36, 21:42,
06/28  06/24
 178    174

 */


