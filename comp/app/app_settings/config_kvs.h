/*
 * config_kvs.h
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#pragma once

#include "app/config/proj_app_cfg.h"
#include "app/settings/config.h"

#ifndef CFG_NAMESPACE
#define CFG_NAMESPACE "config"
#endif

#if 1
#define cfg_key(cb) config_get_kvs_key(cb)



unsigned nvsStr(void *handle, const char *key, void *dst, size_t dst_len, bool save);
unsigned nvsBlob(void *handle, const char *key, void *dst, size_t dst_len, bool save);
#define cfg_get(DT,name) kvs_get_##DT(h, cfg_key(CB_##name), MY_##name, 0)

#define kvsR(DT, cb, val)  do { val = kvs_get_##DT(h, cfg_key(cb), val, 0); } while(0)
#define kvsRead(DT, VT, cb, val)  do { val = static_cast<VT>(kvs_get_##DT(h, cfg_key(cb), static_cast<DT>(val), 0)); } while(0)
#define kvsW(DT, cb, val)  do { kvs_set_##DT(h,cfg_key(cb), val); } while(0)
#define kvsRs(cb, val)  kvs_rw_str(h, cfg_key(cb), val, sizeof val, false)
#define kvsRb(cb, val)  kvs_rw_blob(h, cfg_key(cb), &val, sizeof val, false)
#define kvsWs(cb, val)  kvs_rw_str(h, cfg_key(cb), val, 0, true)
#define kvsWb(cb, val)  kvs_rw_blob(h, cfg_key(cb), &val, sizeof val, true)
#endif
