#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <mbedtls/base64.h>

#include <esp_http_server.h>

#include "net/http_server.h"
#include "userio_app/status_output.h"
#include "userio/status_json.h"
#include "config/config.h"
#include "cli_app/cli_imp.h"
#include "misc/int_types.h"

bool check_access_allowed(httpd_req_t *req); //XXX move this to a header file

static const char *TAG="APP";

////////////////////////// URI handlers /////////////////////////////////
static esp_err_t handle_uri_cmd_json(httpd_req_t *req) {
  char buf[256];
  int ret, remaining = req->content_len;

  if (!check_access_allowed(req))
    return ESP_FAIL;

  if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
    return ESP_FAIL;
  }

  hts_query(HQT_NONE, buf, ret); // parse and process received command

  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, sj_get_json());

  sj_free_buffer();

  return ESP_OK;
}

static esp_err_t handle_uri_tfmcu_js(httpd_req_t *req) {
  if (!check_access_allowed(req))
    return ESP_FAIL;

  httpd_resp_set_type(req, "text/javascript");
  httpd_resp_sendstr(req, (const char*) req->user_ctx);

  return ESP_OK;
}

static esp_err_t handle_uri_tfmcu_html(httpd_req_t *req) {
  if (!check_access_allowed(req))
    return ESP_FAIL;

  httpd_resp_set_type(req, "text/html");
  httpd_resp_sendstr(req, (const char*) req->user_ctx);

  return ESP_OK;
}

static esp_err_t handle_uri_doc_post(httpd_req_t *req) {
  int i;
  char buf[64];
  int ret, remaining = req->content_len;

  static struct  {
    const char *key, *txt;
  } help_txt_map [] = {
    { "cliparm_send", cli_help_parmSend},
    { "cliparm_auto", cli_help_parmTimer},
    { "cliparm_config", cli_help_parmConfig},
    { "cliparm_mcu", cli_help_parmMcu},
    { "cliparm_pair", cli_help_parmPair},
    { "cliparm_shpref", cli_help_parmShpref},
    { "cliparm_help", cli_help_parmHelp},
  };

  if (!check_access_allowed(req))
    return ESP_FAIL;

  if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
    return ESP_FAIL;
  }

  for (i = 0; i < sizeof(help_txt_map) / sizeof(help_txt_map[0]); ++i) {
    if (strncmp(buf, help_txt_map[i].key, ret) == 0) {
      httpd_resp_sendstr(req, help_txt_map[i].txt);
      httpd_resp_set_type(req, "text/plain;charset=\"ISO-8859-1\"");
      return ESP_OK;
    }
  }
  return ESP_FAIL;
}

////////////////////////// URI definitions ////////////////////////////////
extern const u8 text_tfmcu_html[] asm("_binary_tfmcu_html_start");
extern const u8 text_tfmcu_js[] asm("_binary_tfmcu_js_start");

static const httpd_uri_t httpd_uris[] = {
    { .uri = "/cmd.json", .method = HTTP_POST, .handler = handle_uri_cmd_json, .user_ctx = NULL, },
    { .uri = "/", .method = HTTP_GET, .handler = handle_uri_tfmcu_html, .user_ctx = (void*) text_tfmcu_html, },
    { .uri = "/tfmcu.js", .method = HTTP_GET, .handler = handle_uri_tfmcu_js, .user_ctx = (void*) text_tfmcu_js, },
    { .uri = "/doc", .method = HTTP_POST, .handler = handle_uri_doc_post, .user_ctx = NULL, },
};


///////// public ///////////////
void hts_register_uri_handlers(httpd_handle_t server) {
  int i;


  ESP_LOGI(TAG, "Registering URI handlers");
  for (i = 0; i < sizeof(httpd_uris) / sizeof(httpd_uri_t); ++i) {
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &httpd_uris[i]));
  }

}
