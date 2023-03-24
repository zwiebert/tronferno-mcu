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

#include <fcntl.h>

static const char *TAG="APP";

#ifdef CONFIG_APP_USE_WS
/*
 *
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
  size_t json_len;
  int fd;
};

static void ws_async_broadcast(void *arg) {
  auto a = static_cast<ws_send_arg *>(arg);
  httpd_ws_frame_t ws_pkt = {  .final = true, .type = HTTPD_WS_TYPE_TEXT, .payload = (uint8_t*)a->json, .len = a->json_len };

  for (int fd = 0; fd < ws_nfds; ++fd) {
    if (!FD_ISSET(fd, &ws_fds))
      continue;
    esp_err_t res = httpd_ws_send_frame_async(a->hd, fd, &ws_pkt);
    if (res != ESP_OK) {
      ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d (%s)", res, esp_err_to_name(res));
      FD_CLR(fd, &ws_fds);
      if (ws_nfds == fd + 1)
        ws_nfds = fd;
    }
  }
  free(a->json);
  free(a);
}

static esp_err_t ws_trigger_send(httpd_handle_t handle, const char *json, size_t len, int fd = -1) {
  struct ws_send_arg *arg = static_cast<struct ws_send_arg *>(malloc(sizeof(struct ws_send_arg)));
  arg->hd = handle;
  arg->json = strdup(json);
  arg->json_len = len;
  arg->fd = fd;
  return httpd_queue_work(handle, ws_async_broadcast, arg);
}

static void ws_send_json(const char *json, ssize_t len) {
  ws_trigger_send(hts_server, json, len >= 0 ? len : strlen(json));
}

static int ws_write(void *req, const char *s, ssize_t s_len = -1, bool final = true) {
  const size_t len = s_len < 0 ? strlen(s) : (size_t)s_len;

  httpd_ws_frame_t ws_pkt = {  .final = final, .type = HTTPD_WS_TYPE_TEXT, .payload = (uint8_t*)s, .len = len };
  if (auto res = httpd_ws_send_frame((httpd_req_t *)req, &ws_pkt); res != ESP_OK) {
    ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d (%s)", res, esp_err_to_name(res));
    return -1;
  }

  return len;
}

#endif
/////////////////////////////////////////////////////////////////////////
int ht_write(void *req, const char *s, size_t len) {
  if (ESP_OK == httpd_resp_send_chunk(static_cast<httpd_req_t *>(req), s, len))
    return len;
  return -1;
}


////////////////////////// URI handlers /////////////////////////////////
static esp_err_t handle_uri_cmd_json(httpd_req_t *req) {
  char buf[256];
  int ret, remaining = req->content_len;
  int result = ESP_OK;

  if (!check_access_allowed(req))
    return ESP_FAIL;

  if ((ret = httpd_req_recv(req, buf, MIN(remaining, (sizeof buf)-1))) <= 0) {
    return ESP_FAIL;
  }

  { LockGuard lock(cli_mutex); 
    buf[ret] = '\0';

    httpd_resp_set_type(req, "application/json") == ESP_OK || (result = ESP_FAIL);
#if 0
    UoutWriter td { req, SO_TGT_HTTP | SO_TGT_FLAG_JSON, ht_write };
    cli_process_json(buf, td); // parse and process received command
    td.sj().write_json() >= 0 || (result = ESP_FAIL);
    httpd_resp_send_chunk(req, 0, 0);
#else
    UoutWriter td { SO_TGT_HTTP | SO_TGT_FLAG_JSON};
    cli_process_json(buf, td); // parse and process received command
    httpd_resp_sendstr(req, td.sj().get_json()) == ESP_OK || (result = ESP_FAIL);
#endif
  }

  return result;
}


static esp_err_t respond_file(httpd_req_t *req, const struct file_map *fm) {
  esp_err_t result = ESP_OK;

  if (fm->type)
    httpd_resp_set_type(req, fm->type);
  if (fm->wc.content_encoding)
    httpd_resp_set_hdr(req, "content-encoding", fm->wc.content_encoding);

  if (fm->wc.content_length == ~0U) {
    // send an vfs-file
    const char *file_name = fm->wc.content;
    if (int fd = ::open(file_name, O_RDONLY); fd >= 0) {
      ESP_LOGI("respond_file", "fd=%d", fd);
      for (int i = 0; result == ESP_OK; ++i) {
        char buf[256];

        if (const auto bytes_read = ::read(fd, buf, sizeof buf); bytes_read >= 0) {
          ESP_LOGI("respond_file", "read_data=<%*s>", bytes_read, buf);

          if (bytes_read == sizeof buf) {
            httpd_resp_send_chunk(req, buf, bytes_read);
            if (fm->type)
              result = httpd_resp_set_type(req, fm->type);
            if (fm->wc.content_encoding)
              httpd_resp_set_hdr(req, "content-encoding", fm->wc.content_encoding);
          } else {
            if (i) {
              result = httpd_resp_send_chunk(req, buf, bytes_read);
              httpd_resp_send_chunk(req, buf, 0);
            } else {
              result = httpd_resp_send(req, buf, bytes_read);
            }
            break;
          }
        } else {
          ESP_LOGE("respond_file", "read error");
          result = ESP_FAIL;
          break;
        }
      }
      ::close(fd);
    } else {
      ESP_LOGE("respond_file", "cannot open file: <%s>", file_name);
      result = ESP_ERR_NOT_FOUND;
    }
  } else if (fm->wc.content_length) {
    result = httpd_resp_send(req, fm->wc.content, fm->wc.content_length);
  } else {
    result = httpd_resp_sendstr(req, fm->wc.content);
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
      ESP_LOGI(TAG, "Handshake done, the new connection was opened");
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
    ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d (%s)", ret, esp_err_to_name(ret));
      return ret;
  }

  if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) { // XXX: does nothing. remove this
    ESP_LOGI(TAG, "Close fd: %d", fd);
    FD_CLR(fd, &ws_fds);
  }


  {
    LockGuard lock(cli_mutex);
    UoutWriterWebsocket td { req, SO_TGT_WS | SO_TGT_FLAG_JSON, ws_write };
    cli_process_json((char*)buf, td);// parse and process received command
  }
  return ret;
}
#endif
////////////////////////// URI definitions ////////////////////////////////
static const httpd_uri_t httpd_uris[] = {
    { .uri = "/cmd.json", .method = HTTP_POST, .handler = handle_uri_cmd_json },
    { .uri = "/f/*", .method = HTTP_GET, .handler = handle_uri_get_file },
    { .uri = "/", .method = HTTP_GET, .handler = handle_uri_get_file },
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

