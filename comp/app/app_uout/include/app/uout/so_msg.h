#pragma once
#include <app/uout/so_types.h>

void soMsg_fw_start_msg_print(const struct TargetDesc &td);
void soMsg_status_ok(const struct TargetDesc &td);
void soMsg_status_error(const struct TargetDesc &td);
void soMsg_mcu_begin(const struct TargetDesc &td);
void soMsg_mcu_run_time(const struct TargetDesc &td);
void soMsg_mcu_version(const struct TargetDesc &td);
void soMsg_mcu_ota(const struct TargetDesc &td, const char *url);
void soMsg_mcu_ota_state(const struct TargetDesc &td);
void soMsg_mcu_boot_count(const struct TargetDesc &td);
void soMsg_mcu_end(const struct TargetDesc &td);
void soMsg_sep_enable(const struct TargetDesc &td);
void soMsg_sep_disable(const struct TargetDesc &td);
void soMsg_sep_button_pressed_error(const struct TargetDesc &td);
void soMsg_cfgpasswd_ok(const struct TargetDesc &td);
void soMsg_cfgpasswd_wrong(const struct TargetDesc &td);
void soMsg_cfgpasswd_missing(const struct TargetDesc &td);
void soMsg_cuas_start(const struct TargetDesc &td, uint16_t id);
void soMsg_cuas_timeout(const struct TargetDesc &td);
void soMsg_cuas_done(const struct TargetDesc &td);
void soMsg_cuas_state(const struct TargetDesc &td, int state);
void soMsg_pras_start_listening(const struct TargetDesc &td, uint16_t id);
void soMsg_pras_stop_listening(const struct TargetDesc &td);
void soMsg_pras_timeout(const struct TargetDesc &td);
void soMsg_pras_done(const struct TargetDesc &td, bool success, bool unpair);
void soMsg_timer_event_print(const struct TargetDesc &td, const so_arg_gm_t a);
int soMsg_timer_print_begin(const struct TargetDesc &td);
void soMsg_timer_print_end(const struct TargetDesc &td);
int soMsg_timer_begin(const struct TargetDesc &td, const so_arg_gm_t a);
void soMsg_timer_end(const struct TargetDesc &td);
void soMsg_astro_minutes_print(const struct TargetDesc &td, int am);
void soMsg_shpref_obj_begin(const struct TargetDesc &td);
void soMsg_shpref_obj_end(const struct TargetDesc &td);
void soMsg_shpref_obj_gm_begin(const struct TargetDesc &td, const so_arg_gm_t a);
void soMsg_shpref_obj_gm_end(const struct TargetDesc &td);

void soMsg_pos_print_gmp(const struct TargetDesc &td, const so_arg_gmp_t a);
void soMsg_pos_print_gmpa(const struct TargetDesc &td, const so_arg_gmp_t *a);
void soMsg_pos_print_mmp(const struct TargetDesc &td, const so_arg_mmp_t a);
void soMsg_pos_begin(const struct TargetDesc &td);
void soMsg_pos_end(const struct TargetDesc &td);
void soMsg_pair_begin(const struct TargetDesc &td);
void soMsg_pair_end(const struct TargetDesc &td);
void soMsg_pair_all_begin(const struct TargetDesc &td);
void soMsg_pair_all_end(const struct TargetDesc &td);
void soMsg_pair_print_amm(const struct TargetDesc &td, const so_arg_amm_t a);
void soMsg_pair_print_kmm(const struct TargetDesc &td, const so_arg_kmm_t a);
void soMsg_pair_print_kmm_single(const struct TargetDesc &td, const so_arg_kmm_t a);
void soMsg_inet_print_address(const struct TargetDesc &td);
void soMsg_gpio_pin(const struct TargetDesc &td, const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(const struct TargetDesc &td, const so_arg_kvd_t a);
void soMsg_print_kvs(const struct TargetDesc &td, so_arg_kvs_t a);

void soMsg_kv(const struct TargetDesc &td, const char *key, const char *val);
void soMsg_kv(const struct TargetDesc &td, const char *key, int val);
void soMsg_kv(const struct TargetDesc &td, const char *key, bool val);



