/**
 * \file    app_http_server/esp32/http_uri_handlers.h
 * \brief   Register URI handlers
 * \author: bertw
 */

#pragma once

#include <esp_http_server.h>

/**
 * \brief          Register URI handlers to SERVER
 * \param server   Handle of HTTP server
 */
void hts_register_uri_handlers(httpd_handle_t server);

