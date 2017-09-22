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

#if 0
// astro data for berlin germany
const uint8_t astro_data[12][8] = {
  {0x34, 0x16, 0x36, 0x16, 0x36, 0x16, 0x38, 0x16 },
  {0x40, 0x16, 0x44, 0x16, 0x48, 0x16, 0x52, 0x16 },
  {0x58, 0x16, 0x04, 0x17, 0x10, 0x17, 0x16, 0x17 },
  {0x24, 0x17, 0x30, 0x17, 0x38, 0x17, 0x46, 0x17 },
  {0x52, 0x17, 0x00, 0x18, 0x08, 0x18, 0x18, 0x18 },
  {0x26, 0x18, 0x34, 0x18, 0x42, 0x18, 0x50, 0x18 },
  {0x58, 0x18, 0x06, 0x1f, 0x16, 0x1f, 0x24, 0x1f },
  {0x32, 0x1f, 0x42, 0x1f, 0x50, 0x1f, 0x58, 0x1f },
  {0x08, 0x20, 0x16, 0x20, 0x24, 0x20, 0x32, 0x20 },
  {0x40, 0x20, 0x46, 0x20, 0x54, 0x20, 0x00, 0x21 },
  {0x06, 0x21, 0x10, 0x21, 0x14, 0x21, 0x18, 0x21 },
  {0x20, 0x21, 0x20, 0x21, 0x20, 0x21, 0x20, 0x21 },
};
#endif

void ICACHE_FLASH_ATTR
calc_sunrise_sunset(float *sunrise, float *sunset, float timezone, int day_of_year, float dgrad_longitude, float dgrad_latidude)
{
  int T = day_of_year;
  float B = M_PI * dgrad_latidude / 180;
  float h = -0.0145;
  float Deklination = 0.4095*sin(0.016906*(T-80.086));
  float Zeitdifferenz = 12*acos((sin(h) - sin(B)*sin(Deklination)) / (cos(B)*cos(Deklination)))/M_PI;
  float Zeitgleichung = -0.171*sin(0.0337*T + 0.465) - 0.1299*sin(0.01787*T - 0.168);

  if (sunrise) {
    float sunrise_woz = (12 - Zeitdifferenz - Zeitgleichung);
    float sunrise_gmt = sunrise_woz + dgrad_longitude/15;
    *sunrise = sunrise_gmt + timezone;
  }
  
  if (sunset) {
    float sunset_woz = (12 + Zeitdifferenz - Zeitgleichung);
    float sunset_gmt = sunset_woz + dgrad_longitude/15;
    *sunset = sunset_gmt + timezone;
  }
}

void ICACHE_FLASH_ATTR
time_to_bcd(uint8_t *dstMinutes, uint8_t *dstHours, float time)
{
  double integral, fractional;
  
  fractional = modf(time, &integral);
  
  if (dstHours) {
    *dstHours = dec2bcd_special((uint8_t)integral);
  }
  if (dstMinutes) {
    *dstMinutes = dec2bcd_special((uint8_t)(fractional * 60));
  }
}

void ICACHE_FLASH_ATTR
write_astro_(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{
	int i, j;
  time_t day_stamp = 355 * ONE_DAY;
 
    for (i=0; i < 12; ++i) {
      for (j=0; j < 8; ++j, (day_stamp += 329400)) {
        time_t sunset = sun_set(&day_stamp) + (mint_offset * SECS_PER_MINT);
        struct tm *t = localtime(&sunset); // should be localtime
        dst[i][j] = dec2bcd_special(t->tm_min);
        dst[i][++j] = dec2bcd_special(t->tm_hour);
      }
    }
}  


void ICACHE_FLASH_ATTR
write_astro(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{ int i, j;
  float sunset = 0, day = 355;
  
    for (i=0; i < 12; ++i) {
      for (j=0; j < 4; ++j, (day += 3.8125)) {
        calc_sunrise_sunset(NULL, &sunset, C.timezone + (mint_offset / 60.0f), ((int)day) % 366, C.longitude, C.latitude);
        time_to_bcd(&dst[i][j*2],&dst[i][j*2+1], sunset);
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
  calc_sunrise_sunset(&rise, &set, C.timezone, doy, C.longitude, C.latitude);
  
  fill_astro(data, C.timezone, 0, C.longitude, C.latitude);
  

  return rise == set;
}

#endif
