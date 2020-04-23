#include "config.h"
#include "config_defaults.h"
#include "app/fernotron.h"
#include "misc/int_types.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

config C;

#ifdef POSIX_TIME
double tz2offset(const char *tz) {
  double offset = 0.0;

  for (const char *p=tz; *p; ++p) {
    if (isalpha(*p))
      continue;
    offset = strtod(p, 0) * -1;
    return offset;
  }
  return 0;
}

#endif

