#pragma once
#include <app_uout/so_types.h>
#include <fernotron_uout/fer_uo_publish.h>

class UoutWriter;

void soMsg_fw_start_msg_print(const UoutWriter &td);
void soMsg_status_ok(const UoutWriter &td);
void soMsg_status_error(const UoutWriter &td);
void soMsg_mcu_begin(const UoutWriter &td);
void soMsg_mcu_run_time(const UoutWriter &td);
void soMsg_mcu_version(const UoutWriter &td);
void soMsg_mcu_ota(const UoutWriter &td, const char *url);
void soMsg_mcu_ota_state(const UoutWriter &td);
void soMsg_mcu_boot_count(const UoutWriter &td);
void soMsg_mcu_end(const UoutWriter &td);
void soMsg_cfgpasswd_ok(const UoutWriter &td);
void soMsg_cfgpasswd_wrong(const UoutWriter &td);
void soMsg_cfgpasswd_missing(const UoutWriter &td);
void soMsg_shpref_obj_begin(const UoutWriter &td);
void soMsg_shpref_obj_end(const UoutWriter &td);
void soMsg_shpref_obj_gm_begin(const UoutWriter &td, const so_arg_gm_t a);
void soMsg_shpref_obj_gm_end(const UoutWriter &td);
void soMsg_KVS_begin(const class UoutWriter &td);
void soMsg_KVS_end(const class UoutWriter &td);
bool soMsg_KVS_print(const class UoutWriter &td, const char *key);

void soMsg_inet_print_address(const UoutWriter &td);
void soMsg_gpio_pin(const UoutWriter &td, const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(const UoutWriter &td, const so_arg_kvd_t a);
void soMsg_print_kvs(const UoutWriter &td, so_arg_kvs_t a);

void soMsg_kv(const UoutWriter &td, const char *key, const char *val);
void soMsg_kv(const UoutWriter &td, const char *key, int val);
void soMsg_kv(const UoutWriter &td, const char *key, bool val);



