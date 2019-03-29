/*
 * http_server.c
 *
 *  Created on: 26.03.2019
 *      Author: bertw
 */


/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include <esp_http_server.h>

#include "user_config.h"
#include "userio/http_server.h"
#include "userio/status_output.h"
#include "userio/status_json.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 * The examples use simple WiFi configuration that you can set via
 * 'make menuconfig'.
 * If you'd rather not, just change the below entries to strings
 * with the config you want -
 * ie. #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

static const char *TAG="APP";



/* An HTTP POST handler */
esp_err_t post_handler_json(httpd_req_t *req) {
  char buf[100];
  int ret, remaining = req->content_len;

  if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
    return ESP_FAIL;
  }

  // apply JSON command object
  hts_query(HQT_NONE, buf, ret);

  { // send back configuration in JSON (???)
    uint16_t js_size = 512;
    char *js = malloc(js_size);
    if (js) {
      if (sj_config2json_buf(js, js_size, SO_CFG_all) >= 0)
        httpd_resp_sendstr(req, js);
      free(js);
    }
  }

  return ESP_OK;
}

httpd_uri_t echo = {
    .uri       = "/cmd.json",
    .method    = HTTP_POST,
    .handler   = post_handler_json,
    .user_ctx  = NULL
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// /config.json
esp_err_t get_handler_config_json(httpd_req_t *req) {

  /* Set some custom headers */

  httpd_resp_set_type(req, "application/json");

  {
    uint16_t js_size = 512;
    char *js = malloc(js_size);
    if (js) {
      if (sj_config2json_buf(js, js_size, SO_CFG_all) >= 0)
        httpd_resp_sendstr(req, js);
      free(js);
    }
  }
  //httpd_resp_sendstr(req, (const char*) test_html);

  return ESP_OK;
}

httpd_uri_t config_json = {
    .uri       = "/config.json",
    .method    = HTTP_GET,
    .handler   = get_handler_config_json,
    .user_ctx  = NULL
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// get .js
esp_err_t get_handler_js(httpd_req_t *req) {

  httpd_resp_set_type(req, "text/javascript");
  httpd_resp_sendstr(req, (const char*) req->user_ctx);

  return ESP_OK;
}

extern const char tfmcu_js[];

httpd_uri_t uri_tfmcu_js = {
    .uri       = "/tfmcu.js",
    .method    = HTTP_GET,
    .handler   = get_handler_js,
    .user_ctx  = (void*)tfmcu_js,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// get .js
esp_err_t get_handler_html(httpd_req_t *req) {

  httpd_resp_set_type(req, "text/html");
  httpd_resp_sendstr(req, (const char*) req->user_ctx);

  return ESP_OK;
}

extern const char tfmcu_html[];

httpd_uri_t uri_tfmcu_html = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = get_handler_html,
    .user_ctx  = (void*)tfmcu_html,
};



httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &config_json);
        httpd_register_uri_handler(server, &uri_tfmcu_js);
        httpd_register_uri_handler(server, &uri_tfmcu_html);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static httpd_handle_t server;

void hts_start_server() {
  server = start_webserver();
}

void hts_stop_server() {
  stop_webserver(server);
  server = NULL;
}


