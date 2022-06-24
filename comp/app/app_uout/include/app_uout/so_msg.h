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
void soMsg_sep_obj_begin(const TargetDesc &td);
void soMsg_sep_obj_end(const TargetDesc &td);
void soMsg_sep_enable(const TargetDesc &td);
void soMsg_sep_disable(const TargetDesc &td);
void soMsg_sep_button_pressed_error(const TargetDesc &td);
void soMsg_cfgpasswd_ok(const TargetDesc &td);
void soMsg_cfgpasswd_wrong(const TargetDesc &td);
void soMsg_cfgpasswd_missing(const TargetDesc &td);
void soMsg_cuas_start(const TargetDesc &td, uint16_t id);
void soMsg_cuas_timeout(const TargetDesc &td);
void soMsg_cuas_done(const TargetDesc &td);
void soMsg_cuas_state(const TargetDesc &td, int state);
void soMsg_pras_start_listening(const TargetDesc &td, uint16_t id);
void soMsg_pras_stop_listening(const TargetDesc &td);
void soMsg_pras_timeout(const TargetDesc &td);
void soMsg_pras_done(const TargetDesc &td, bool success, bool unpair);
void soMsg_timer_event_print(const TargetDesc &td, const so_arg_gm_t a);
int soMsg_timer_print_begin(const TargetDesc &td, const char *tag = "auto");
void soMsg_timer_print_end(const TargetDesc &td);
int soMsg_timer_begin(const TargetDesc &td, const so_arg_gm_t a);
void soMsg_timer_end(const TargetDesc &td);
void soMsg_astro_minutes_print(const TargetDesc &td, int am);
void soMsg_shpref_obj_begin(const TargetDesc &td);
void soMsg_shpref_obj_end(const TargetDesc &td);
void soMsg_shpref_obj_gm_begin(const TargetDesc &td, const so_arg_gm_t a);
void soMsg_shpref_obj_gm_end(const TargetDesc &td);
void soMsg_KVS_begin(const struct TargetDesc &td);
void soMsg_KVS_end(const struct TargetDesc &td);
bool soMsg_KVS_print(const struct TargetDesc &td, const char *key);

void soMsg_pos_print_gmp(const TargetDesc &td, const so_arg_gmp_t a);
void soMsg_pos_print_gmpa(const TargetDesc &td, const so_arg_gmp_t *a);
void soMsg_pos_print_mmp(const TargetDesc &td, const so_arg_mmp_t a);
void soMsg_pos_begin(const TargetDesc &td);
void soMsg_pos_end(const TargetDesc &td);
void soMsg_pair_begin(const TargetDesc &td);
void soMsg_pair_end(const TargetDesc &td);
void soMsg_pair_all_begin(const TargetDesc &td);
void soMsg_pair_all_end(const TargetDesc &td);
void soMsg_pair_print_kmm(const TargetDesc &td, const so_arg_kmm_t a);
void soMsg_pair_print_kmm_single(const TargetDesc &td, const so_arg_kmm_t a);
void soMsg_repeater_begin(const TargetDesc &td);
void soMsg_repeater_end(const TargetDesc &td);
void soMsg_inet_print_address(const TargetDesc &td);
void soMsg_gpio_pin(const TargetDesc &td, const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(const TargetDesc &td, const so_arg_kvd_t a);
void soMsg_print_kvs(const TargetDesc &td, so_arg_kvs_t a);

void soMsg_kv(const TargetDesc &td, const char *key, const char *val);
void soMsg_kv(const TargetDesc &td, const char *key, int val);
void soMsg_kv(const TargetDesc &td, const char *key, bool val);



