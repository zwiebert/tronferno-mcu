#include "itoa.h"
#include "int_types.h"
#include <string.h>

char* ftoa(float f, char *buf, int n) {
  int i;
  i32 mult;
  u32 rop;
  i16 lop = (i16) f;
  char *s = buf;

  itoa(lop, s, 10);
  strcat(s, ".");
  s += strlen(s);

  f -= lop;

  mult = (lop < 0) ? -1 : 1;

  for (i = 0; i < n; ++i)
    mult *= 10;

  rop = (u32) (f * mult);
  ltoa(rop, s, 10);

  return buf;
}
