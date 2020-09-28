#include <app/uout/status_output.h>
#include <misc/int_types.h>
//#include <config_kvs/config.h>

#if 0
void so_output_message(so_msg_t mt, const void *arg) {}


#endif

//bool config_item_modified(enum configItem item) {}

void  mcu_restart(void) {}
bool mcu_get_buttonPin(void) {}
volatile u32 run_time_s_, run_time_ts_;
//extern const char * const config_keys[10] = {};

void setup_pin(const struct cfg_gpio *c) {}
//void   txtio_mcu_setup() {}

void mcu_put_txPin(unsigned char) {}
