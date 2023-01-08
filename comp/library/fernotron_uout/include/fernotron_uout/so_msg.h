#pragma once

struct TargetDesc;



void soMsg_sep_obj_begin(const TargetDesc &td);
void soMsg_sep_obj_end(const TargetDesc &td);
void soMsg_sep_enable(const TargetDesc &td);
void soMsg_sep_disable(const TargetDesc &td);
void soMsg_sep_button_pressed_error(const TargetDesc &td);
;
void soMsg_cuas_start(const TargetDesc &td, uint16_t id);
void soMsg_cuas_timeout(const TargetDesc &td);
void soMsg_cuas_done(const TargetDesc &td);
void soMsg_cuas_state(const TargetDesc &td, int state);
void soMsg_pras_start_listening(const TargetDesc &td, uint16_t id);
void soMsg_pras_stop_listening(const TargetDesc &td);
void soMsg_pras_timeout(const TargetDesc &td);
void soMsg_pras_done(const TargetDesc &td, bool success, bool unpair);

int soMsg_timer_print_begin(const TargetDesc &td, const char *tag = "auto");
void soMsg_timer_print_end(const TargetDesc &td);
int soMsg_timer_begin(const TargetDesc &td, const so_arg_gm_t a);
void soMsg_timer_end(const TargetDesc &td);
void soMsg_astro_minutes_print(const TargetDesc &td, int am);

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

void soMsg_kv(const TargetDesc &td, const char *key, const char *val);
void soMsg_kv(const TargetDesc &td, const char *key, int val);
void soMsg_kv(const TargetDesc &td, const char *key, bool val);



