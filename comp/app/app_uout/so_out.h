#include "app/uout/status_output.h"
#include <uout/so_out.h>

#ifndef SO_MSG_T
#define SO_MSG_T so_msg_t
#endif

const char * gk(SO_MSG_T so_key);
SO_MSG_T so_parse_config_key(const char *k);


