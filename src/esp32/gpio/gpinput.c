#include <string.h>

#include "user_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

#include "user_config.h"
#include "main/gpinput.h"
#include "positions/shutter_state.h"
#include "debug/debug.h"


#ifndef DISTRIBUTION
#define D(x) x
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif


#define CFG_NAMESPACE "Tronferno"

extern char *itoa(int val, char *s, int radix);
extern char *ltoa(long val, char *s, int radix);



void gpi_configure(u8 gpi_nmb, u8 g, u8 m, u8 gpi_cmd) {
  char key[] = "cpair_gpi01";
  precond(gpi_nmb <= GPIO_NUM_MAX);
  precond(1 <= g && g <= 7 && 1 <= m && m <= 7);

  itoa(gpi_nmb, key + (gpi_nmb < 10 ? 10 : 9), 10);
}

void gpi_loop(void) {

}
