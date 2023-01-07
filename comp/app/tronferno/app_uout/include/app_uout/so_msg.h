#pragma once
#include <app_uout/so_types.h>
#include <fernotron_uout/fer_uo_publish.h>

struct TargetDesc;

void soMsg_fw_start_msg_print(const TargetDesc &td);
void soMsg_status_ok(const TargetDesc &td);
void soMsg_status_error(const TargetDesc &td);
void soMsg_mcu_begin(const TargetDesc &td);
void soMsg_mcu_run_time(const TargetDesc &td);
void soMsg_mcu_version(const TargetDesc &td);
void soMsg_mcu_ota(const TargetDesc &td, const char *url);
void soMsg_mcu_ota_state(const TargetDesc &td);
void soMsg_mcu_boot_count(const TargetDesc &td);
void soMsg_mcu_end(const TargetDesc &td);
void soMsg_cfgpasswd_ok(const TargetDesc &td);
void soMsg_cfgpasswd_wrong(const TargetDesc &td);
void soMsg_cfgpasswd_missing(const TargetDesc &td);
void soMsg_shpref_obj_begin(const TargetDesc &td);
void soMsg_shpref_obj_end(const TargetDesc &td);
void soMsg_shpref_obj_gm_begin(const TargetDesc &td, const so_arg_gm_t a);
void soMsg_shpref_obj_gm_end(const TargetDesc &td);
void soMsg_KVS_begin(const struct TargetDesc &td);
void soMsg_KVS_end(const struct TargetDesc &td);
bool soMsg_KVS_print(const struct TargetDesc &td, const char *key);

void soMsg_inet_print_address(const TargetDesc &td);
void soMsg_gpio_pin(const TargetDesc &td, const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(const TargetDesc &td, const so_arg_kvd_t a);
void soMsg_print_kvs(const TargetDesc &td, so_arg_kvs_t a);

void soMsg_kv(const TargetDesc &td, const char *key, const char *val);
void soMsg_kv(const TargetDesc &td, const char *key, int val);
void soMsg_kv(const TargetDesc &td, const char *key, bool val);



