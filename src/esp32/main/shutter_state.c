/*
 * shutter_state.c
 *
 *  Created on: 05.03.2019
 *      Author: bertw
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define CFG_NAMESPACE "Tronferno"

#include "user_config.h"
#include "config/config.h"
#include "positions/shutter_state.h"

#define NB_SIZE 30
#define NB_PFX "GMBM_"
#define NB_N (NB_SIZE - sizeof (NB_PFX))

typedef u8 u8a8[8];

static int store (const char *name, u8a8 *gm, int count, bool write) {
  char nb[NB_SIZE] = NB_PFX;
  strncat (nb, name, NB_N);

  esp_err_t err = 0;
  nvs_handle handle;
  size_t len = sizeof(*gm) * count;

  if ((err = nvs_open(CFG_NAMESPACE, (write ? NVS_READWRITE : NVS_READONLY), &handle)) == ESP_OK) {
    if (write) {
      err = nvs_set_blob(handle, nb, gm, len);
      nvs_commit(handle);
    } else {

      err = nvs_get_blob(handle, nb, gm, &len); //FIXME: check len?
    }
    nvs_close(handle);
  }


  return err == ESP_OK;
}

int read_gm_bitmask(const char *name, const gm_bitmask_t *gm, int count) {
  return store (name, (u8a8 *)gm, count, false);
}

int save_gm_bitmask(const char *name, gm_bitmask_t *gm, int count) {
  return store (name, (u8a8 *)gm, count, true);
}


static char *g_to_name(u8 g, char *buf) {
  return "TODO"; //TODO: IMPLEMENT ME
}

 int read_g_positions(u8 g, const shutterGroupPositionsT *positions) {
   char buf[8];
  return store (g_to_name(g, buf), (u8a8 *)positions, 1, false);
}

int save_g_positions(u8 g, shutterGroupPositionsT *positions) {
  char buf[8];
  return store (g_to_name(g, buf), (u8a8 *)positions, 1, true);
}
