#define CONFIG_HTTPD_WS_SUPPORT 1

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <mbedtls/base64.h>
#include "net/http/server/http_server.h"
#include "userio/status_json.h"
#include "config/config.h"
#include "cli_app/cli_imp.h"
#include "misc/int_types.h"
#include "cli/mutex.h"
#include "net/http/server/esp32/register_uris.h"


static const char *TAG="APP";

/*
 * Structure holding server handle
 * and internal socket fd in order
 * to use out of request send
 */
struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};

static fd_set ws_fds;
static int ws_nfds;
struct ws_send_arg {
  httpd_handle_t hd;
  char *json;
};

static void ws_send(void *arg) {
  struct ws_send_arg *a = arg;
  httpd_ws_frame_t ws_pkt = { .type = HTTPD_WS_TYPE_TEXT, .payload = (u8*)a->json, .len = strlen(a->json), .final = true};

  for (int fd = 0; fd < ws_nfds; ++fd) {
    if (!FD_ISSET(fd, &ws_fds))
      continue;
    esp_err_t res = httpd_ws_send_frame_async(a->hd, fd, &ws_pkt);
    if (res != ESP_OK) {
      FD_CLR(fd, &ws_fds);
      if (ws_nfds == fd + 1)
        ws_nfds = fd;
    }
  }
  free(a->json);
  free(a);
}

static esp_err_t ws_trigger_send(httpd_handle_t handle, const char *json) {
  struct ws_send_arg *arg = malloc(sizeof(struct ws_send_arg));
  arg->hd = handle;
  arg->json = strdup(json);
  return httpd_queue_work(handle, ws_send, arg);
}

void ws_send_json(const char *json) {
  ws_trigger_send(hts_server, json);
}

/*
 * async send function, which we put into the httpd work queue
 */
static void ws_async_send(void *arg)
{
  ESP_LOGI(TAG, "ws_async_send");
    static const char * data = "Async data";
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    ws_pkt.final = true;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
  ESP_LOGI(TAG, "trigger_async_send");
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}

//////////////////// static files ///////////////////////////////////////
extern const u8 text_tfmcu_html[] asm("_binary_tfmcu_html_start");
extern const char text_tfmcu_js[] asm("_binary_tfmcu_js_start");
extern const char text_tfmcu_js_map[] asm("_binary_tfmcu_js_map_start");;

////////////////////////// URI handlers /////////////////////////////////
static esp_err_t handle_uri_cmd_json(httpd_req_t *req) {
  char buf[256];
  int ret, remaining = req->content_len;

  if (!check_access_allowed(req))
    return ESP_FAIL;

  if ((ret = httpd_req_recv(req, buf, MIN(remaining, (sizeof buf)-1))) <= 0) {
    return ESP_FAIL;
  }

  if (mutex_cliTake()) {
    buf[ret] = '\0';
    hts_query0(HQT_NONE, buf); // parse and process received command

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, sj_get_json());

    sj_free_buffer();
    mutex_cliGive();
  }

  return ESP_OK;
}

const struct {
  const char *uri, *type, *file;
} uri_file_map[] = { { .uri = "/f/js/tfmcu.js", .type = "text/javascript", .file = text_tfmcu_js }, //
    //   { .uri = "/tfmcu.js.map", .type = "text/javascript",  .file = text_tfmcu_js_map }, //
    //   { .uri = "", .file = "" }, //

    { .uri = "/f/cli/help/send", .file = cli_help_parmSend }, //
    { .uri = "/f/cli/help/auto", .file = cli_help_parmTimer }, //
    { .uri = "/f/cli/help/config", .file = cli_help_parmConfig }, //
    { .uri = "/f/cli/help/mcu", .file = cli_help_parmMcu }, //
    { .uri = "/f/cli/help/pair", .file = cli_help_parmPair }, //
    { .uri = "/f/cli/help/shpref", .file = cli_help_parmShpref }, //
    { .uri = "/f/cli/help/help", .file = cli_help_parmHelp }, //
    };


static esp_err_t handle_uri_get_file(httpd_req_t *req) {
  if (!check_access_allowed(req))
    return ESP_FAIL;

  for (int i = 0; i < sizeof(uri_file_map) / sizeof(uri_file_map[0]); ++i) {
    if (strcmp(req->uri, uri_file_map[i].uri) != 0)
      continue;
    const char *type = (uri_file_map[i].type) ? uri_file_map[i].type : "text/plain;charset=\"UTF-8\"";
    httpd_resp_set_type(req, type);
    httpd_resp_sendstr(req, uri_file_map[i].file);
    return ESP_OK;
  }

  return ESP_FAIL;
}

static esp_err_t handle_uri_tfmcu_html(httpd_req_t *req) {
  if (!check_access_allowed(req))
    return ESP_FAIL;

  httpd_resp_set_type(req, "text/html");
  httpd_resp_sendstr(req, (const char*) req->user_ctx);

  return ESP_OK;
}

static esp_err_t handle_uri_ws(httpd_req_t *req) {
  int fd = httpd_req_to_sockfd(req);
  FD_SET(fd, &ws_fds);
  if (ws_nfds <= fd)
    ws_nfds = 1 + fd;
  ESP_LOGI(TAG, "handle /ws using fd: %d", fd);
  uint8_t buf[128] = { 0 };

  httpd_ws_frame_t ws_pkt = {.payload = buf };
  esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, (sizeof buf)-1);

  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
      return ret;
  }
  ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
  ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);

  if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) { // XXX: does nothing. remove this
    ESP_LOGI(TAG, "Close fd: %d", fd);
    FD_CLR(fd, &ws_fds);
  }


  if (mutex_cliTake()) {
    buf[ws_pkt.len] = '\0';
    hts_query0(HQT_NONE, (char*)buf); // parse and process received command

    ws_pkt.payload = (u8*)sj_get_json();
    ws_pkt.len = strlen((char*)ws_pkt.payload);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    ret = httpd_ws_send_frame(req, &ws_pkt);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }

    sj_free_buffer();
    mutex_cliGive();
  }
  return ret;
}

////////////////////////// URI definitions ////////////////////////////////


static const httpd_uri_t httpd_uris[] = {
    { .uri = "/cmd.json", .method = HTTP_POST, .handler = handle_uri_cmd_json, .user_ctx = NULL, },
    { .uri = "/f/*", .method = HTTP_GET, .handler = handle_uri_get_file},
    { .uri = "/", .method = HTTP_GET, .handler = handle_uri_tfmcu_html, .user_ctx = (void*) text_tfmcu_html, },
    { .uri = "/ws", .method = HTTP_GET, .handler = handle_uri_ws, .user_ctx = NULL, .is_websocket = true},
};


///////// public ///////////////
void hts_register_uri_handlers(httpd_handle_t server) {
  int i;


  ESP_LOGI(TAG, "Registering URI handlers");
  for (i = 0; i < sizeof(httpd_uris) / sizeof(httpd_uri_t); ++i) {
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &httpd_uris[i]));
  }

}
