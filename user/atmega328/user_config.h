#define MCU_TYPE "atmega328"

extern const char *const build_time;
#define ISO_BUILD_TIME build_time

#define DB_NO_LIGHTS

#define SAVE_TIMER_DATA_FUN(a,g,m) false
#define READ_TIMER_DATA_FUN(a,g,m,w) false
