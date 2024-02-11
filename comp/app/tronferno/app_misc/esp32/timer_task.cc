/*
 * timer.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */



#include "app_misc/rtc.h"
#include "app_misc/timer.h"
#include "app_settings/config.h"
#include "fernotron_trx/isr_timer_config.h"
#include <fernotron_trx/fer_trx_api.hh>
#include "txtio/inout.h"

#include "esp_sleep.h"
#include "driver/gptimer.h"
#include "esp_types.h"
#include "esp_intr_alloc.h"
#include "hal/timer_types.h"
#include "utils_misc/int_types.h"
#include "sdkconfig.h"
#include "soc/timer_group_struct.h"
#include <esp32/rom/ets_sys.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define E1 0 // experimental: 2=enable pulse on TX-GPIO to show interrupt frequency and duration, 1=toggle tx each call of interrupt (useless)

struct rxTxTask_msg {
    bool rx_pin_level :1;
    bool run_rx :1;
    bool run_tx :1;
};

static bool intTimer_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data);

static TaskHandle_t xHandle;
#define STACK_SIZE  4096
#define TASK_PRIO 3

static void intTimer_init(void *user_data) {
  gptimer_handle_t gptimer = NULL;
  gptimer_config_t timer_config = { .clk_src = GPTIMER_CLK_SRC_DEFAULT, .direction = GPTIMER_COUNT_UP, .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
      };
  ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

  gptimer_alarm_config_t config = { .alarm_count = FER_ISR_TICK_US, .reload_count = 0, .flags = { .auto_reload_on_alarm = 1 } };

  ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &config));
  gptimer_event_callbacks_t cbs = { .on_alarm = intTimer_isr };
  ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, user_data));
  ESP_ERROR_CHECK(gptimer_enable(gptimer));
  ESP_ERROR_CHECK(gptimer_start(gptimer));
}

static void rxTxTask_main(void *param) {
  QueueHandle_t rxTxTask_message_queue;


  if ((rxTxTask_message_queue = xQueueCreate(10, sizeof(rxTxTask_msg)))) {

    intTimer_init((void*) rxTxTask_message_queue);
  } else {

    ESP_LOGE("rxTxTask", "Creating queue failed");
    goto error;
  }

  for (;;) {
    rxTxTask_msg msg = { };
    if (pdTRUE == xQueueReceive(rxTxTask_message_queue, &msg, pdMS_TO_TICKS(60 * 1000))) {
#ifdef CONFIG_APP_USE_FER_TRANSMITTER
      if (msg.run_tx) {
        Fer_Trx_API::isr_handle_tx();
      }
#endif
#ifdef CONFIG_APP_USE_FER_RECEIVER
      if (msg.run_rx) {
        Fer_Trx_API::isr_handle_rx(msg.rx_pin_level);
      }
#endif

    }
  }

  error:
  return;
}

static bool rxTxTask_setup() {

  if (xHandle) {
    vTaskDelete(xHandle);
    xHandle = NULL;
  }
  xTaskCreate(rxTxTask_main, "rf_transceiver", STACK_SIZE, NULL, TASK_PRIO, &xHandle);
  return true;
}

//////////////////////////////////////////////////////////////////////////
#ifndef CONFIG_APP_USE_ESP_GET_TIME
volatile uint32_t run_time_s_, run_time_ts_;
#endif

IRAM_ATTR static bool intTimer_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data) {
  QueueHandle_t queue = (QueueHandle_t) user_data;
  rxTxTask_msg msg = { };
  static uint_fast8_t tick_count;
  ++tick_count;

#ifdef CONFIG_APP_USE_FER_TRANSMITTER
#if E1 == 1
  static bool e_tx_level;
  mcu_put_txPin (e_tx_level);
  e_tx_level = !e_tx_level;
#elif E1 == 2
  mcu_put_txPin(true);
#else
  mcu_put_txPin(Fer_Trx_API::isr_get_tx_level());
#endif
#endif
#ifdef CONFIG_APP_USE_FER_RECEIVER
  bool rx_pin_lvl = mcu_get_rxPin();
#endif



#ifdef CONFIG_APP_USE_FER_TRANSMITTER
  {
    if (0 == (tick_count & ((FER_ISR_FMULT / FER_TX_FMULT) - 1))) {
      msg.run_tx = true;
    }
  }
#endif
#ifdef CONFIG_APP_USE_FER_RECEIVER
  if (0 == (tick_count & ((FER_ISR_FMULT / FER_RX_FMULT) - 1))) {
    msg.run_rx = true;
    msg.rx_pin_level = rx_pin_lvl;
  }
#endif

#ifndef CONFIG_APP_USE_ESP_GET_TIME
  {
    static uint32_t s10_ticks = TICK_FREQ_HZ / 10;
    static int8_t s_ticks = 10;

    if (s10_ticks-- == 0) {
      s10_ticks = TICK_FREQ_HZ / 10;
      ++run_time_ts_;
      if (!s_ticks--) {
        s_ticks = 10;
        ++run_time_s_;
      }
    }
  }
#endif

#if E1 == 2
  mcu_put_txPin(false);
#endif

  if (msg.run_rx || msg.run_tx) {
    BaseType_t high_task_awoken = pdFALSE;
    xQueueSendFromISR(queue, &msg, &high_task_awoken);
    return high_task_awoken == pdTRUE;
  }
  return false;
}

void app_timerISR_setup(void) {
  rxTxTask_setup();
}

