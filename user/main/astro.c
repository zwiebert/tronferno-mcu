/*
* astro.c
*
* Created: 24.08.2017 22:02:54
*  Author: bertw
*/

#include <stdlib.h>
#include <math.h>
#include "time.h"

#include "astro.h"
#include "common.h"
#include "utils.h"
#include "rtc.h"
#include "config.h"
#include "fer_code.h"
#include "fer.h"

#define float double
/*
 * horizon values for twilight (in radian)
 */
#define CIVIL_TWILIGHT_RAD         -0.10471975511966
#define NAUTICAL_TWILIGHT_RAD      -0.20943951023932
#define ASTRONOMICAL_TWILIGHT_RAD  -0.31415926535898

/*
 * calculate sunrise, sunset, twilight
 *
 * sunrise - to return value of sunrise or dawn
 * sunset - to return value of sunset or dusk
 * timezone - time offset to UTC
 * day_of_year - 1..366
 * longitude - geographical longitude (in decimal degrees)
 * latitude - geographical latitude (in decimal degrees)
 * horizon - horizon (in rad) to calculate twilight
 */
static void ICACHE_FLASH_ATTR
calc_sunrise_sunset(float *sunrise, float *sunset, float timezone, float day_of_year, float longitude_dec, float latitude_dec, float horizon) {

  float latitude_rad = M_PI * latitude_dec / 180;

  float declination_of_sun = 0.4095 * sin(0.016906 * (day_of_year - 80.086));
  float time_diff = 12 * acos((sin(horizon) - sin(latitude_rad) * sin(declination_of_sun)) / (cos(latitude_rad) * cos(declination_of_sun))) / M_PI;
  float equation_of_time = -0.171 * sin(0.0337 * day_of_year + 0.465) - 0.1299 * sin(0.01787 * day_of_year - 0.168);

  if (sunrise) {
    float sunrise_moz = (12 - time_diff - equation_of_time);
    float sunrise_gmt = sunrise_moz - longitude_dec / 15;
    *sunrise = sunrise_gmt + timezone;
  }

  if (sunset) {
    float sunset_moz = (12 + time_diff - equation_of_time);
    float sunset_gmt = sunset_moz - longitude_dec / 15;
    *sunset = sunset_gmt + timezone;
  }
}

/*
 *
 * force_even_minutes  - if true, no odd number will be outputted in dstMinutes
 */
static void ICACHE_FLASH_ATTR
time_to_bcd(uint8_t *dstMinutes, uint8_t *dstHours, float time, bool force_even_minutes) {
  double integral, fractional;

  fractional = modf(time, &integral);

  if (dstHours) {
    *dstHours = dec2bcd_special((uint8_t) integral);
  }
  if (dstMinutes) {
    *dstMinutes = dec2bcd_special((force_even_minutes ? ~1 : ~0) & (1+(uint8_t) (fractional * 60)));
  }
}


static void ICACHE_FLASH_ATTR
math_write_astro(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{ int i, j;
  float sunset = 0, day = 355, last_sunset = -1;

    for (i=0; i < 12; ++i) {
      for (j=0; j < 4; ++j, (day -= 3.8046875)) {
        calc_sunrise_sunset(NULL, &sunset, C.geo_timezone + (mint_offset / 60.0f), day, C.geo_longitude, C.geo_latitude, CIVIL_TWILIGHT_RAD);
        if (sunset < last_sunset) {
          sunset = last_sunset;
        } else {
          //last_sunset = sunset;
        }
        time_to_bcd(&dst[i][j*2],&dst[i][j*2+1], sunset, true);
      }
    }
}

static void ICACHE_FLASH_ATTR
math_write_astro2(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset) {
  int i, j;
  float sunset = 0, last_sunset = -1;
  int day = 355;

  for (i = 0; i < 12; ++i) {
    for (j = 0; j < 4; ++j, (day -= 4)) {
      calc_sunrise_sunset(NULL, &sunset, C.geo_timezone + (mint_offset / 60.0f), day, C.geo_longitude, C.geo_latitude, CIVIL_TWILIGHT_RAD);
      if (sunset < last_sunset) {
        sunset = last_sunset;
      } else {
        last_sunset = sunset;
      }
      time_to_bcd(&dst[i][j * 2], &dst[i][j * 2 + 1], sunset, true);
    }
  }
}


#if TEST_MODULE_ASTRO
uint8_t data[12][8];

bool ICACHE_FLASH_ATTR
testModule_astro()
{
  float rise, set;
  uint16_t doy = 172;
  calc_sunrise_sunset(&rise, &set, C.timezone, doy, C.geo_longitude, C.geo_latitude, CIVIL_TWILIGHT_RAD);
  
  fill_astro(data, C.geo_timezone, 0, C.geo_longitude, C.geo_latitude);
  

  return rise == set;
}

#endif


#if 0
static void
ICACHE_FLASH_ATTR tbl_write_astro(uint8_t d[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], const uint8_t ad[12][8], int mint_offset) {
  int col, line;

  mint_offset &= ~1;  // make sure the number is even


  for (line=0; line < FPR_ASTRO_HEIGHT; ++line) {
    for (col=0; col < FPR_ASTRO_WIDTH; ++col) {
      d[line][col] = dec2bcd(bcd2dec(ad[line][col]) + (mint_offset % 60));
      ++col;
      d[line][col] = dec2bcd(bcd2dec(ad[line][col]) + (mint_offset / 60));
    }
  }
}
#endif

// real times: 2017-10-22T18:27:00 (DST)  // the device interpolates times from the table

#if 0
// table generated by original hardware for berlin, germany
// table should contain only even minutes
// each table entry should be greater or equal than its predecessor
const uint8_t ad_plz_10[12][8]  = {
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

// berlin actual civl dusk: 16:33 ... 21:23 (GMT + 1)
// berlin fernotron table:  16:34 ... 21:20
void ICACHE_FLASH_ATTR write_astro(uint8_t d[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset) {
#if 0
  tbl_write_astro(d, ad_plz_10, mint_offset);
#elif 1
  math_write_astro(d, mint_offset);
#endif
}
