#include "../web_content.hh"
#include "../../app_cli/include/app_cli/cli_app.hh"

#include <sys/param.h>

#include "net_http_server/http_server_setup.h"
#include "uout/status_json.hh"
#include "fernotron_uout/fer_uo_publish.h"
#include "app_settings/config.h"

#include "utils_misc/int_types.h"
#include "cli/mutex.hh"
#include "net_http_server/esp32/http_server_esp32.h"

#include <mbedtls/base64.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <functional>
#include <fcntl.h>

using namespace std::placeholders;

#ifdef CONFIG_NET_HTTP_SERVER_DEBUG
#define D(x) x
#else
#define D(x)
#endif

static const char *logtag = "http_handler";

////////////////////////// URI handlers /////////////////////////////////

static int response_callback(httpd_req_t *req, const char *src, ssize_t src_len, int chunk_status) {
  if (src_len < 0)
    src_len = strlen(src);

  //const bool first_chunk = chunk_status == 0 || chunk_status == -1;
  const bool single_chunk = chunk_status == 0;
  const bool final_chunk = 0 <= chunk_status; // 0 or total number of chunks

  D(ESP_LOGW(logtag, "writer for /cmd.json: chunk_status=%d src_ptr=%p, src_len=%d src=<%*s>",
          chunk_status, src, src_len, src_len, src));

  // send in one go
  if (single_chunk) {
    if (!(ESP_OK == httpd_resp_set_type(req, "application/json") && ESP_OK == httpd_resp_send(req, src, src_len)))
      return -1;
    return src_len;
  }

  // send chunks. last chunk needs to have size zero
  if (!(ESP_OK == httpd_resp_set_type(req, "application/json") && ESP_OK == httpd_resp_send_chunk(req, src, src_len)))
    return -1;
  if (final_chunk)
    if (!(ESP_OK == httpd_resp_set_type(req, "application/json") && ESP_OK == httpd_resp_send_chunk(req, src, 0)))
      return -1;
  return src_len;
}

static esp_err_t handle_uri_cmd_json(httpd_req_t *req) {
  char buf[256];
  int ret, remaining = req->content_len;
  int result = ESP_OK;

  if (!check_access_allowed(req))
    return ESP_FAIL;

  if ((ret = httpd_req_recv(req, buf, MIN(remaining, (sizeof buf) - 1))) <= 0) {
    return ESP_FAIL;
  }

  {
    LockGuard lock(cli_mutex);
    auto td = UoutWriterFunction( { std::bind(response_callback, req, _1, _2, _3) }, SO_TGT_HTTP | SO_TGT_FLAG_JSON);

    buf[ret] = '\0'; // terminate command
    cli_process_json(buf, td); // parse and process command
  }

  return result;
}

static esp_err_t handle_uri_get_file(httpd_req_t *req) {
  if (!check_access_allowed(req))
    return ESP_FAIL;

  if (auto fm = wc_getContent(req->uri); fm) {
    return respond_file(req, fm);
  }
  return ESP_FAIL;
}

#ifdef CONFIG_APP_USE_WS
static esp_err_t handle_uri_ws(httpd_req_t *req) {

  if (req->method == HTTP_GET) {
      ESP_LOGI(logtag, "Handshake done, the new web-socket is connected");
      return ESP_OK;
  }

  int fd = httpd_req_to_sockfd(req);
  FD_SET(fd, &ws_fds);
  if (ws_nfds <= fd)
    ws_nfds = 1 + fd;
  uint8_t buf[256] = { 0 };

  httpd_ws_frame_t ws_pkt = {.payload = buf };
  esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, (sizeof buf)-1);

  if (ret != ESP_OK) {
    ESP_LOGE(logtag, "httpd_ws_recv_frame failed with %d (%s)", ret, esp_err_to_name(ret));
      return ret;
  }

  if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) { // XXX: does nothing. remove this
    ESP_LOGI(logtag, "Closed web-socket descriptor: %d", fd);
    FD_CLR(fd, &ws_fds);
  }


  {
    LockGuard lock(cli_mutex);
    auto td = UoutWriterFunction({std::bind(ws_write, req, _1, _2, _3)}, SO_TGT_WS | SO_TGT_FLAG_JSON);
    cli_process_json((char*)buf, td);// parse and process received command
  }
  return ret;
}
#endif
////////////////////////// URI definitions ////////////////////////////////
static const httpd_uri_t httpd_uris[] = { { .uri = "/cmd.json", .method = HTTP_POST, .handler = handle_uri_cmd_json }, { .uri = "/f/*", .method = HTTP_GET,
    .handler = handle_uri_get_file }, { .uri = "/", .method = HTTP_GET, .handler = handle_uri_get_file },
#ifdef CONFIG_APP_USE_WS
    { .uri = "/ws", .method = HTTP_GET, .handler = handle_uri_ws, .user_ctx = NULL, .is_websocket = true },
#endif
    };

///////// public ///////////////
void hts_register_uri_handlers(httpd_handle_t server) {
  for (int i = 0; i < sizeof(httpd_uris) / sizeof(httpd_uri_t); ++i) {
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &httpd_uris[i]));
  }

}

void hts_setup_content() {
  hts_register_uri_handlers_cb = hts_register_uri_handlers;
#ifdef CONFIG_APP_USE_WS
  uo_flagsT flags;
  flags.tgt.websocket = true;
  flags.evt.pin_change = true;
  flags.evt.uo_evt_flag_pctChange = true;
  flags.evt.gen_app_state_change = true;
  flags.evt.gen_app_error_message = true;
  flags.evt.gen_app_log_message = true;
  flags.evt.async_http_resp = true;
  flags.fmt.json = true;
  uoCb_subscribe([](const uoCb_msgT msg) {
    if (auto json = uoCb_jsonFromMsg(msg))
      ws_send_json(json, -1);
  }, flags);
#endif
}

