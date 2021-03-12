/**
 * \file    webapp/content.h
 * \brief   Definitions to make embedded files accessible from C code
 * \note    Mandatory files should be compressed by gzip, because its widely supported.  Brotli may be used for source maps.
 */
#pragma once


extern const char build_wapp_css_gz[]; ///< wapp.css
extern const int build_wapp_css_gz_len; ///< wapp.css size

extern const char build_wapp_html_gz[]; ///< wapp.html
extern const int build_wapp_html_gz_len; ///< wapp.html size

extern const char build_wapp_js_gz[]; ///< wapp.js
extern const int build_wapp_js_gz_len; ///< wapp.js size

extern const char build_wapp_js_map_gz[]; ///< wapp.js.map
extern const int build_wapp_js_map_gz_len; ///< wapp.js.map size
extern const char build_wapp_js_map_br[];  ///< brotli wapp.js.map
extern const int build_wapp_js_map_br_len; ///< brotli wapp.js.map size

extern const char build_wapp_css_map_gz[]; ///< wapp.css.map
extern const int build_wapp_css_map_gz_len; ///< wapp.css.map size
extern const char build_wapp_css_map_br[]; ///< brotli wapp.css.map
extern const int build_wapp_css_map_br_len;///< brotli wapp.css.map size
