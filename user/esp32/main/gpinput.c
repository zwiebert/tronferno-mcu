#include "user_config.h"
#include "main/all.h"
#include "main/gpinput.h"

#include "main/shutter_state.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"


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



void gpi_configure(uint8_t gpi_nmb, uint8_t g, uint8_t m, uint8_t gpi_cmd) {
  char key[] = "cpair_gpi01";
  precond(gpi_nmb <= GPIO_NUM_MAX);
  precond(1 <= g && g <= 7 && 1 <= m && m <= 7);

  itoa(gpi_nmb, key + (gpi_nmb < 10 ? 10 : 9), 10);
}

void gpi_loop(void) {

}
