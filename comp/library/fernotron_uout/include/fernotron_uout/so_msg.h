#pragma once

class UoutWriter;



void soMsg_sep_obj_begin(UoutWriter &td);
void soMsg_sep_obj_end(UoutWriter &td);
void soMsg_sep_enable(UoutWriter &td);
void soMsg_sep_disable(UoutWriter &td);
void soMsg_sep_button_pressed_error(UoutWriter &td);
;
void soMsg_cuas_start(UoutWriter &td, uint16_t id);
void soMsg_cuas_timeout(UoutWriter &td);
void soMsg_cuas_done(UoutWriter &td);
void soMsg_cuas_state(UoutWriter &td, int state);
void soMsg_pras_start_listening(UoutWriter &td, uint16_t id);
void soMsg_pras_stop_listening(UoutWriter &td);
void soMsg_pras_timeout(UoutWriter &td);
void soMsg_pras_done(UoutWriter &td, bool success, bool unpair);

int soMsg_timer_print_begin(UoutWriter &td, const char *tag = "auto");
void soMsg_timer_print_end(UoutWriter &td);
int soMsg_timer_begin(UoutWriter &td, const so_arg_gm_t a);
void soMsg_timer_end(UoutWriter &td);
void soMsg_astro_minutes_print(UoutWriter &td, int am);

void soMsg_pos_print_gmp(UoutWriter &td, const so_arg_gmp_t a);
void soMsg_pos_print_gmpa(UoutWriter &td, const so_arg_gmp_t *a);
void soMsg_pos_print_mmp(UoutWriter &td, const so_arg_mmp_t a);
void soMsg_pos_begin(UoutWriter &td);
void soMsg_pos_end(UoutWriter &td);
void soMsg_pair_begin(UoutWriter &td);
void soMsg_pair_end(UoutWriter &td);
void soMsg_pair_all_begin(UoutWriter &td);
void soMsg_pair_all_end(UoutWriter &td);
void soMsg_pair_print_kmm(UoutWriter &td, const so_arg_kmm_t a);
void soMsg_pair_print_kmm_single(UoutWriter &td, const so_arg_kmm_t a);
void soMsg_repeater_begin(UoutWriter &td);
void soMsg_repeater_end(UoutWriter &td);

void soMsg_kv(UoutWriter &td, const char *key, const char *val);
void soMsg_kv(UoutWriter &td, const char *key, int val);
void soMsg_kv(UoutWriter &td, const char *key, bool val);



