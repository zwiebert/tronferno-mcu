
#ifndef COMPONENTS_USERIO_INCLUDE_USERIO_STATUS_OUTPOUT_H_
#define COMPONENTS_USERIO_INCLUDE_USERIO_STATUS_OUTPOUT_H_

#include <stdint.h>

// to select target for output messages
typedef enum {
  SO_TGT_NONE = 0,
  SO_TGT_CLI_USB = (1<<0),
  SO_TGT_CLI_TCP = (1<<1),
  SO_TGT_CLI = (SO_TGT_CLI_USB|SO_TGT_CLI_TCP),
  SO_TGT_HTTP = (1<<2),
  SO_TGT_MQTT = (1<<3),
  SO_TGT_ANY = (~0)
} so_target_bits;

extern uint8_t so_target;

#define so_tgt_set(bitmask) (so_target = (bitmask))
#define so_tgt_default() (so_tgt_set(SO_TGT_ANY))
#define so_tgt_test(bitmask) (so_target & (bitmask))

#endif /* COMPONENTS_USERIO_INCLUDE_USERIO_STATUS_OUTPOUT_H_ */
