/**
 * \file    webapp/content.h
 * \brief   Definitions to make embedded files accessible from C code
 * \note    Mandatory files should be compressed by gzip, because its widely supported.  Brotli may be used for source maps.
 */
#pragma once

#include <net_http_server/content.hh>

extern const web_content wapp_html_gz_fm;
extern const web_content wapp_js_gz_fm;
extern const web_content wapp_js_map_gz_fm;
extern const web_content wapp_css_gz_fm;
extern const web_content wapp_css_map_gz_fm;

extern const web_content wapp_html_br_fm;
extern const web_content wapp_js_br_fm;
extern const web_content wapp_js_map_br_fm;
extern const web_content wapp_css_br_fm;
extern const web_content wapp_css_map_br_fm;

