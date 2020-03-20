#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <mbedtls/base64.h>

#include <esp_http_server.h>

#include "app_config/proj_app_cfg.h"
#include "http_server.h"
#include "userio/status_json.h"
#include "config/config.h"
#include "cli_app/cli_imp.h"
#include "http_server_content/esp32/http_uri_handlers.h"

static const char *TAG="APP";




//////////////////////////Authorization//////////////////////
#define HTTP_USER C.http_user
#define HTTP_USER_LEN (strlen(HTTP_USER))
#define HTTP_PW C.http_password
#define HTTP_PW_LEN  (strlen(HTTP_PW))

static bool verify_userName_and_passWord(const char *up, size_t up_len) {

  if (HTTP_USER_LEN + 1 + HTTP_PW_LEN != up_len)
    return false;

  return strncmp(HTTP_USER, up, HTTP_USER_LEN) == 0 && strncmp(HTTP_PW, up + (up_len - HTTP_PW_LEN), HTTP_PW_LEN) == 0;
}

static bool verify_authorization(httpd_req_t *req) {
  bool login_ok = false;
  char *login = 0;

  size_t login_len = httpd_req_get_hdr_value_len(req, "Authorization");
  if (login_len && (login = malloc(login_len + 1))) {
    esp_err_t err = httpd_req_get_hdr_value_str(req, "Authorization", login, login_len + 1);
    if (err == ESP_OK) {
      unsigned char dst[128];
      size_t olen = 0;
      if (0 == mbedtls_base64_decode(dst, sizeof dst, &olen, (unsigned char*) login + 6, login_len - 6))
        login_ok = verify_userName_and_passWord((char*)dst, olen);
    }

    free(login);
  }

  return login_ok;
}

static bool is_access_allowed(httpd_req_t *req) {
  return (*HTTP_USER == '\0' && *HTTP_PW == '\0') || verify_authorization(req);
}

static void reqest_authorization(httpd_req_t *req) {
  httpd_resp_set_status(req, "401 Unauthorized");
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"tronferno-mcu.all\"");
  httpd_resp_sendstr(req, "<p>please login</p>");
}

bool check_access_allowed(httpd_req_t *req) {
  if (!is_access_allowed(req)) {
    reqest_authorization(req);
   return false;
  }
  return true;
}



static httpd_handle_t start_webserver(void) {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_open_sockets = 6;

  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
  }

  hts_register_uri_handlers(server);
  return server;
}

///////// public interface ///////////////////

void hts_enable_http_server(bool enable) {
  static httpd_handle_t server;

  if (enable && !server) {
    server = start_webserver();
  }

  if (!enable && server) {
    httpd_stop(server);
    server = NULL;
  }
}


