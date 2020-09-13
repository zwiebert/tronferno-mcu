#pragma once
#include <uout_app/so_types.h>

void soMsg_fw_start_msg_print();
void soMsg_status_ok();
void soMsg_status_error();
void soMsg_mcu_begin();
void soMsg_mcu_run_time();
void soMsg_mcu_version();
void soMsg_mcu_ota(const char *url);
void soMsg_mcu_ota_state();
void soMsg_mcu_boot_count();
void soMsg_mcu_end();
void soMsg_sep_enable();
void soMsg_sep_disable();
void soMsg_sep_button_pressed_error();
void soMsg_cfgpasswd_ok();
void soMsg_cfgpasswd_wrong();
void soMsg_cfgpasswd_missing();
void soMsg_cuas_start(uint16_t id);
void soMsg_cuas_timeout();
void soMsg_cuas_done();
void soMsg_cuas_state(int state);
void soMsg_pras_start_listening(uint16_t id);
void soMsg_pras_stop_listening();
void soMsg_pras_timeout();
void soMsg_pras_done(bool success, bool unpair);
void soMsg_timer_event_print(const so_arg_gm_t a);
int soMsg_timer_print_begin();
void soMsg_timer_print_end();
int soMsg_timer_begin(const so_arg_gm_t a);
void soMsg_timer_end();
void soMsg_astro_minutes_print(int am);
void soMsg_shpref_obj_begin();
void soMsg_shpref_obj_end();
void soMsg_shpref_obj_gm_begin(const so_arg_gm_t a);
void soMsg_shpref_obj_gm_end();

void soMsg_pos_print_gmp(const so_arg_gmp_t a, bool broadcast = false);
void soMsg_pos_print_gmpa(const so_arg_gmp_t *a, bool broadcast = false);
void soMsg_pos_print_mmp(const so_arg_mmp_t a);
void soMsg_pos_begin();
void soMsg_pos_end();
void soMsg_pair_begin();
void soMsg_pair_end();
void soMsg_pair_all_begin();
void soMsg_pair_all_end();
void soMsg_pair_print_amm(const so_arg_amm_t a);
void soMsg_pair_print_kmm(const so_arg_kmm_t a);
void soMsg_pair_print_kmm_single(const so_arg_kmm_t a);
void soMsg_inet_print_address();
void soMsg_gpio_pin(const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(const so_arg_kvd_t a);
void soMsg_print_kvs(so_arg_kvs_t a);

void soMsg_kv(const char *key, const char *val);
void soMsg_kv(const char *key, int val);
void soMsg_kv(const char *key, bool val);



