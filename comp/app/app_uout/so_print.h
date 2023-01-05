#include <utils_misc/int_types.h>
class Fer_GmSet;

void so_gmbitmask_to_str(char *dst, Fer_GmSet *mm);
void so_print_gmbitmask(Fer_GmSet *mm);
void so_print_timer(uint8_t g, uint8_t m);
void so_timer_to_json(uint8_t g, uint8_t m, bool wildcard);
void so_print_timer_as_text(uint8_t g, uint8_t m, bool wildcard);
void so_print_timer_event_minutes(uint8_t g, uint8_t m);
void print_timer_event_minute(const char *label, uint16_t mins);
void so_print_startup_info(void);
