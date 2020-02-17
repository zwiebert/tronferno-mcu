#include "user_config.h"

#include "main/pairings.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "shutter_positions/shutter_state.h"


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
  
static bool
read_controller(gm_bitmask_t *gm, const char *key)
{
  esp_err_t err;
  nvs_handle handle;
  bool result = false;
  size_t len;
  
  D(ets_printf("%s: gm=%p, key=\"%s\"\n", __func__, gm, key));
  precond (gm && key);

  
  if ((err = nvs_open(CFG_NAMESPACE, NVS_READONLY, &handle)) == ESP_OK) {
    if (ESP_OK == ((len = sizeof *gm), (err = nvs_get_blob(handle, key, gm, &len)))) {
      result = true;
    }
    nvs_close(handle);
  }
  return result;
}

static bool
add_rm_controller(const char *key, u8 g, u8 m, bool remove)
{
  esp_err_t err;
  nvs_handle handle;
  bool result = false;
  size_t len;

  D(ets_printf("%s: key=\"%s\", g=%d, m=%d, remove=%d\n", __func__, key, (int)g, (int)m, (int)remove));
  precond (key && 1 <= g && g <= 7 && 1 <= m && m <= 7);

  err = nvs_open(CFG_NAMESPACE, NVS_READWRITE, &handle);
  if (err == ESP_OK) {
    gm_bitmask_t gm;
    (len = sizeof gm), (err = nvs_get_blob(handle, key, &gm, &len));
    if (err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND) {
      if (err == ESP_ERR_NVS_NOT_FOUND) {
        memset(gm, 0, sizeof gm);
      }
      PUT_BIT(gm[g], m, !remove);

      bool not_empty = false;
      for (g=0; g <= 7; ++g)
      if (gm[g] != 0) {
        not_empty = true;
        break;
      }
      if ((not_empty && (err = nvs_set_blob(handle, key, &gm, sizeof gm)) == ESP_OK)
          || (!not_empty && (ESP_OK == (err = nvs_erase_key(handle, key)))))
      {
        result = true;
        nvs_commit(handle);
      }
    }
    nvs_close(handle);
  }

  return result;
}


bool
pair_controller(u32 controller, u8 g, u8 m, bool unpair) {
  D(ets_printf("%s: g=%d, m=%d, unpair=%d\n", __func__, (int)g, (int)m, (int)unpair));
  precond ((controller & 0xff000000) == 0);
  precond (1 <= g && g <= 7 && 1 <= m && m <= 7);

  char key[] = "cpair_10abcd";
  ltoa(controller, key+6, 16);


  return add_rm_controller(key, g, m, unpair);
}

bool
read_pairings(gm_bitmask_t *gm, u32 controller) {
  precond (gm && (controller & 0xff000000) == 0);

  char key[] = "cpair_10abcd";
  ltoa(controller, key+6, 16);

  return read_controller(gm, key);
}


