
#define MCU_TYPE "esp32"

//extern const char *const build_date;
#define ISO_BUILD_TIME (__DATE__ __TIME__)


#define FER_TRANSMITTER
#define FER_RECEIVER
#define USE_NTP
#define USE_WLAN
#define CONFIG_BLOB 0
#define ACCESS_GPIO
#define CONFIG_GPIO_SIZE 40
#define POSIX_TIME 1

#define SAVE_TIMER_DATA_FUN save_timer_data_fs
#define READ_TIMER_DATA_FUN read_timer_data_fs
