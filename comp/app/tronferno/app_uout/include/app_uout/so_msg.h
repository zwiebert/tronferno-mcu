#pragma once
#include <app_uout/so_types.h>
#include <fernotron_uout/fer_uo_publish.h>

class UoutWriter;

void soMsg_fw_start_msg_print(UoutWriter &td);
void soMsg_status_ok(UoutWriter &td);
void soMsg_status_error(UoutWriter &td);
void soMsg_mcu_begin(UoutWriter &td);
void soMsg_mcu_run_time(UoutWriter &td);
void soMsg_mcu_version(UoutWriter &td);
void soMsg_mcu_ota(UoutWriter &td, const char *url);
void soMsg_mcu_ota_state(UoutWriter &td);
void soMsg_mcu_boot_count(UoutWriter &td);
void soMsg_mcu_end(UoutWriter &td);
void soMsg_cfgpasswd_ok(UoutWriter &td);
void soMsg_cfgpasswd_wrong(UoutWriter &td);
void soMsg_cfgpasswd_missing(UoutWriter &td);
void soMsg_shpref_obj_begin(UoutWriter &td);
void soMsg_shpref_obj_end(UoutWriter &td);
void soMsg_shpref_obj_gm_begin(UoutWriter &td, const so_arg_gm_t a);
void soMsg_shpref_obj_gm_end(UoutWriter &td);
void soMsg_KVS_begin(class UoutWriter &td);
void soMsg_KVS_end(class UoutWriter &td);
bool soMsg_KVS_print(class UoutWriter &td, const char *key);

void soMsg_inet_print_address(UoutWriter &td);
void soMsg_gpio_pin(UoutWriter &td, const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(UoutWriter &td, const so_arg_kvd_t a);
void soMsg_print_kvs(UoutWriter &td, so_arg_kvs_t a);

void soMsg_kv(UoutWriter &td, const char *key, const char *val);
void soMsg_kv(UoutWriter &td, const char *key, int val);
void soMsg_kv(UoutWriter &td, const char *key, bool val);



