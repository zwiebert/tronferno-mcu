#pragma once


extern void (*fer_sep_enable_disable_cb)(bool enable);

static inline void fer_sep_ENABLE_cb() {
  if (fer_sep_enable_disable_cb)
    fer_sep_enable_disable_cb(true);
}
static inline void fer_sep_DISABLE_cb() {
  if (fer_sep_enable_disable_cb)
    fer_sep_enable_disable_cb(false);
}
