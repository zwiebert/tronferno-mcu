/*
 * mqtt.c
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */
#include "user_config.h"
#include "userio/mqtt.h"

#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "cli/cli.h"
#include "config/config.h"

static const char *TAG = "MQTT_EXAMPLE";

#ifdef DISTRIBUTION
#define CONFIG_MQTT_CLIENT_ID "tronferno42"
#else
#define CONFIG_MQTT_CLIENT_ID "tfdbg"
#endif




static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {
  esp_mqtt_client_handle_t client = event->client;
  int msg_id;
  // your_context_t *context = event->context;
  switch (event->event_id) {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    io_mqtt_connected();
    break;
    
  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
    io_mqtt_disconnected();
    break;

  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
    io_mqtt_subscribed(event->topic, event->topic_len);
    break;

  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);   io_mqtt_unsubscribed(event->topic, event->topic_len);
    
    break;
  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
    io_mqtt_published(event->msg_id);
    break;
    
  case MQTT_EVENT_DATA:
    ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);
    
    io_mqtt_received(event->topic, event->topic_len, event->data, event->data_len);
 
    break;
  case MQTT_EVENT_ERROR:
    ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
    break;
  default:
    ESP_LOGI(TAG, "Other event id:%d", event->event_id);
    break;
  }
  return ESP_OK;
}

static esp_mqtt_client_handle_t client;

void io_mqtt_subscribe(const char *topic, int qos) {
  int msg_id = esp_mqtt_client_subscribe(client, topic, qos);
}

void io_mqtt_unsubscribe(const char *topic) {
  int msg_id = esp_mqtt_client_unsubscribe(client, topic);
}

void io_mqtt_publish(const char *topic, const char *data) {
  ESP_LOGI(TAG, "MQTT_PUBLISH, topic=%s, data=%s", topic, data);
   int msg_id = esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}

static void io_mqtt_create_client(void) {
  esp_mqtt_client_config_t mqtt_cfg = {
      .uri = C.mqtt_url,
  //  .host = CONFIG_MQTT_HOST,
  //  .port = CONFIG_MQTT_PORT,
      .event_handle = mqtt_event_handler,
      .username = C.mqtt_user,
      .password = C.mqtt_password,
      .client_id = CONFIG_MQTT_CLIENT_ID,
  // .user_context = (void *)your_context
      };

    client = esp_mqtt_client_init(&mqtt_cfg);

}

void io_mqtt_create_and_start(void) {
  if (client)
    io_mqtt_stop_and_destroy();
  io_mqtt_create_client();
  esp_mqtt_client_start(client);
}

void io_mqtt_stop_and_destroy(void) {
  if (client) {
    esp_mqtt_client_stop(client);
    esp_mqtt_client_destroy(client);
    client = 0;
  }
}


void setup_mqtt(void) {
  if (C.app_verboseOutput > 5) {
    ets_printf("\n\n----#####################################----------\n\n");
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
  }
  if (C.mqtt_enable) {
    C.mqtt_enable = 0; //FIXME: kludge to avoid endless reboots when url parsing exception
    save_config(CONFIG_MQTT_ENABLE);
    io_mqtt_enable(true);
    C.mqtt_enable = 1;
    save_config(CONFIG_MQTT_ENABLE);
  }
}