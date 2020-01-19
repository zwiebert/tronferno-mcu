#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <mbedtls/base64.h>

#include <esp_http_server.h>

#include "user_config.h"
#include "userio/http_server.h"
#include "userio/status_output.h"
#include "userio/status_json.h"
#include "config/config.h"


static const char *TAG="APP";

///////////////////////////Authorization//////////////////////
#define HTTP_USER C.http_user
#define HTTP_USER_LEN (strlen(HTTP_USER))
#define HTTP_PW C.http_password
#define HTTP_PW_LEN  (strlen(HTTP_PW))

static bool compare_up(const char *up, size_t up_len) {

  if (HTTP_USER_LEN + 1 + HTTP_PW_LEN != up_len)
    return false;

  return strncmp(HTTP_USER, up, HTTP_USER_LEN) == 0 && strncmp(HTTP_PW, up + (up_len - HTTP_PW_LEN), HTTP_PW_LEN) == 0;
}

static bool test_authorization(httpd_req_t *req) {
  bool login_ok = false;
  char *login = 0;

  size_t login_len = httpd_req_get_hdr_value_len(req, "Authorization");
  if (login_len && (login = malloc(login_len + 1))) {
    esp_err_t err = httpd_req_get_hdr_value_str(req, "Authorization", login, login_len + 1);
    if (err == ESP_OK) {
      unsigned char dst[128];
      size_t olen = 0;
      if (0 == mbedtls_base64_decode(dst, sizeof dst, &olen, (unsigned char*) login + 6, login_len - 6))
        login_ok = compare_up((char*)dst, olen);
    }

    free(login);
  }

  return login_ok;
}

static bool is_access_allowed(httpd_req_t *req) {
  return (*HTTP_USER == '\0' && *HTTP_PW == '\0') || test_authorization(req);
}

static void reqest_authorization(httpd_req_t *req) {
  httpd_resp_set_status(req, "401 Unauthorized");
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"tronferno-mcu.all\"");
  httpd_resp_sendstr(req, "<p>please login</p>");
}

static bool check_access_allowed(httpd_req_t *req) {
  if (!is_access_allowed(req)) {
    reqest_authorization(req);
   return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////

/* An HTTP POST handler */
esp_err_t post_handler_json(httpd_req_t *req) {
  char buf[256];
  int ret, remaining = req->content_len;

  if (!check_access_allowed(req))
    return ESP_OK;


  if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
    return ESP_FAIL;
  }

  {
    uint16_t json_size = 512;
    char *json = malloc(json_size);
    if (json) {
      sj_set_buf(json, json_size);  // reply json will now be created in our buffer
      hts_query(HQT_NONE, buf, ret); // parse and process received command

      httpd_resp_set_type(req, "application/json");
      httpd_resp_sendstr(req, *json ? json : "{\"from\":\"tfmcu\"}"); // respond with reply json or empty object json

      sj_set_buf(0, 0); // unregister our jsone buffer
      free(json);
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

  if (!check_access_allowed(req))
    return ESP_OK;

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
  if (!check_access_allowed(req))
    return ESP_OK;

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

  if (!check_access_allowed(req))
    return ESP_OK;

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


esp_err_t get_handler_test(httpd_req_t *req) {

  if (!check_access_allowed(req))
    return ESP_OK;

  httpd_resp_set_type(req, "text/html");
  httpd_resp_sendstr(req, "<p>login ok</p>");

  return ESP_OK;
}

extern const char tfmcu_test[];

httpd_uri_t uri_tfmcu_test = { .uri = "/test", .method = HTTP_GET, .handler = get_handler_test, .user_ctx = NULL, };


httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 6;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &config_json);
        httpd_register_uri_handler(server, &uri_tfmcu_js);
        httpd_register_uri_handler(server, &uri_tfmcu_html);
        httpd_register_uri_handler(server, &uri_tfmcu_test);
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

void hts_enable_http_server(bool enable) {
  if (enable && !server) {
    server = start_webserver();
  }

  if (!enable && server) {
    stop_webserver(server);
    server = NULL;
  }
}


