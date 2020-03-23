
#include "app_config/proj_app_cfg.h"
#include "net/ipnet.h"
#include "config/config.h"
#include "misc/int_types.h"
#include "esp_netif_ip_addr.h"

extern esp_ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;

void
ipnet_addr_as_string(char *buf, unsigned buf_len) {
  char *ip4addr_ntoa_r(const u32 *addr, char *buf, int buflen);
  ip4addr_ntoa_r(&ip4_address, buf, buf_len);
}

u32 get_ip4addr_local() { return ip4_address.addr; }
u32 get_ip4addr_gateway() { return ip4_gateway_address.addr; }
u32 get_ip4netmask() { return ip4_netmask.addr; }
