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

//////////////////// static files ///////////////////////////////////////
#include "../webapp/build/wapp.html.gz.c"
#include "../webapp/build/wapp.js.gz.c"
#include "../webapp/build/wapp.css.gz.c"
#ifdef SERVE_JS_MAP
#include "../webapp/build/wapp.js.map.br.c"
#endif


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



struct file_map { const char *uri, *type, *file; u32 file_size; };

#ifdef SERVE_BR
const struct file_map uri_file_map_br[] = {
#ifdef SERVE_JS_MAP
    { .uri = "/f/js/wapp.js.map", .type = "text/javascript",  .file = build_wapp_js_map_br, .file_size = sizeof build_wapp_js_map_br }, //
#endif
    };
#endif

const struct file_map uri_file_map_gz[] = {
    { .uri = "/f/css/wapp.css", .type = "text/css", .file = build_wapp_css_gz, .file_size = sizeof build_wapp_css_gz }, //
    { .uri = "/", .type = "text/html", .file = build_wapp_html_gz, .file_size = sizeof build_wapp_html_gz }, //
    { .uri = "/f/js/wapp.js", .type = "text/javascript", .file = build_wapp_js_gz, .file_size = sizeof build_wapp_js_gz }, //
    };

const struct file_map uri_file_map[] =  {
        { .uri = "/f/cli/help/send", .file = cli_help_parmSend }, //
        { .uri = "/f/cli/help/auto", .file = cli_help_parmTimer }, //
        { .uri = "/f/cli/help/config", .file = cli_help_parmConfig }, //
        { .uri = "/f/cli/help/mcu", .file = cli_help_parmMcu }, //
        { .uri = "/f/cli/help/pair", .file = cli_help_parmPair }, //
        { .uri = "/f/cli/help/shpref", .file = cli_help_parmShpref }, //
        { .uri = "/f/cli/help/help", .file = cli_help_parmHelp }, //
};


static esp_err_t respond_file(httpd_req_t *req, const struct file_map *fm, const char *content_encoding) {
  httpd_resp_set_type(req, fm->type ? fm->type : "text/plain;charset=\"UTF-8\"");
  if (content_encoding)
    httpd_resp_set_hdr(req, "content-encoding", content_encoding);
  if (fm->file_size)
    httpd_resp_send(req, fm->file, fm->file_size);
  else
    httpd_resp_sendstr(req, fm->file);
  return ESP_OK;
}


static esp_err_t handle_uri_get_file(httpd_req_t *req) {
  if (!check_access_allowed(req))
    return ESP_FAIL;

#ifdef SERVE_BR
  for (int i = 0; i < sizeof(uri_file_map_br) / sizeof(uri_file_map_br[0]); ++i) {
    if (strcmp(req->uri, uri_file_map_br[i].uri) == 0)
      return respond_file(req, &uri_file_map_br[i], "br");
  }
#endif

  for (int i = 0; i < sizeof(uri_file_map_gz) / sizeof(uri_file_map_gz[0]); ++i) {
    if (strcmp(req->uri, uri_file_map_gz[i].uri) == 0)
      return respond_file(req, &uri_file_map_gz[i], "gzip");
  }

  for (int i = 0; i < sizeof(uri_file_map) / sizeof(uri_file_map[0]); ++i) {
    if (strcmp(req->uri, uri_file_map[i].uri) == 0)
      return respond_file(req, &uri_file_map[i], "");
  }

  return ESP_FAIL;
}

static esp_err_t handle_uri_ws(httpd_req_t *req) {
  int fd = httpd_req_to_sockfd(req);
  FD_SET(fd, &ws_fds);
  if (ws_nfds <= fd)
    ws_nfds = 1 + fd;
  uint8_t buf[128] = { 0 };

  httpd_ws_frame_t ws_pkt = {.payload = buf };
  esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, (sizeof buf)-1);

  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
      return ret;
  }

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
    { .uri = "/cmd.json", .method = HTTP_POST, .handler = handle_uri_cmd_json },
    { .uri = "/f/*", .method = HTTP_GET, .handler = handle_uri_get_file },
    { .uri = "/", .method = HTTP_GET, .handler = handle_uri_get_file },
    { .uri = "/ws", .method = HTTP_GET, .handler = handle_uri_ws, .user_ctx = NULL, .is_websocket = true },
};


///////// public ///////////////
void hts_register_uri_handlers(httpd_handle_t server) {
  for (int i = 0; i < sizeof(httpd_uris) / sizeof(httpd_uri_t); ++i) {
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &httpd_uris[i]));
  }

}
