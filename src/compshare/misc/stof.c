#include "stof.h"

float  stof(const char* s) {
  int point_seen;

  float rez = 0, fact = 1;
  if (*s == '-') {
    s++;
    fact = -1;
  };
  for (point_seen = 0; *s; s++) {
    if (*s == '.') {
      point_seen = 1;
      continue;
    };
    int d = *s - '0';
    if (d >= 0 && d <= 9) {
      if (point_seen)
        fact /= 10.0f;
      rez = rez * 10.0f + (float) d;
    };
  };
  return rez * fact;
}
