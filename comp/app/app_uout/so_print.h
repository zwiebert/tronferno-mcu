#include <utils_misc/int_types.h>
class Fer_GmSet;

void so_gmbitmask_to_str(char *dst, Fer_GmSet *mm);
void so_print_gmbitmask(Fer_GmSet *mm);
void so_print_timer(u8 g, u8 m);
void so_timer_to_json(u8 g, u8 m, bool wildcard);
void so_print_timer_as_text(u8 g, u8 m, bool wildcard);
void so_print_timer_event_minutes(u8 g, u8 m);
void print_timer_event_minute(const char *label, uint16_t mins);
void so_print_startup_info(void);
