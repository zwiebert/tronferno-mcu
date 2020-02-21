
#include "app/proj_app_cfg.h"
#include "net/ipnet.h"
#include "userio_app/status_output.h"
#include "config/config.h"

extern u32 ip4_address, ip4_gateway_address, ip4_netmask;

void
ipnet_addr_as_string(char *buf, unsigned buf_len) {
  char *ip4addr_ntoa_r(const u32 *addr, char *buf, int buflen);
  ip4addr_ntoa_r(&ip4_address, buf, buf_len);
}
