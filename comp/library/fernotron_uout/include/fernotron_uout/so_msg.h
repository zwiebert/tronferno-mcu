#pragma once

class UoutWriter;



void soMsg_sep_obj_begin(const UoutWriter &td);
void soMsg_sep_obj_end(const UoutWriter &td);
void soMsg_sep_enable(const UoutWriter &td);
void soMsg_sep_disable(const UoutWriter &td);
void soMsg_sep_button_pressed_error(const UoutWriter &td);
;
void soMsg_cuas_start(const UoutWriter &td, uint16_t id);
void soMsg_cuas_timeout(const UoutWriter &td);
void soMsg_cuas_done(const UoutWriter &td);
void soMsg_cuas_state(const UoutWriter &td, int state);
void soMsg_pras_start_listening(const UoutWriter &td, uint16_t id);
void soMsg_pras_stop_listening(const UoutWriter &td);
void soMsg_pras_timeout(const UoutWriter &td);
void soMsg_pras_done(const UoutWriter &td, bool success, bool unpair);

int soMsg_timer_print_begin(const UoutWriter &td, const char *tag = "auto");
void soMsg_timer_print_end(const UoutWriter &td);
int soMsg_timer_begin(const UoutWriter &td, const so_arg_gm_t a);
void soMsg_timer_end(const UoutWriter &td);
void soMsg_astro_minutes_print(const UoutWriter &td, int am);

void soMsg_pos_print_gmp(const UoutWriter &td, const so_arg_gmp_t a);
void soMsg_pos_print_gmpa(const UoutWriter &td, const so_arg_gmp_t *a);
void soMsg_pos_print_mmp(const UoutWriter &td, const so_arg_mmp_t a);
void soMsg_pos_begin(const UoutWriter &td);
void soMsg_pos_end(const UoutWriter &td);
void soMsg_pair_begin(const UoutWriter &td);
void soMsg_pair_end(const UoutWriter &td);
void soMsg_pair_all_begin(const UoutWriter &td);
void soMsg_pair_all_end(const UoutWriter &td);
void soMsg_pair_print_kmm(const UoutWriter &td, const so_arg_kmm_t a);
void soMsg_pair_print_kmm_single(const UoutWriter &td, const so_arg_kmm_t a);
void soMsg_repeater_begin(const UoutWriter &td);
void soMsg_repeater_end(const UoutWriter &td);

void soMsg_kv(const UoutWriter &td, const char *key, const char *val);
void soMsg_kv(const UoutWriter &td, const char *key, int val);
void soMsg_kv(const UoutWriter &td, const char *key, bool val);



