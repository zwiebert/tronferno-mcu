
#if 0
#include "app_config/proj_app_cfg.h"
#include "net/ipnet.h"
#include "userio_app/status_output.h"
#include "config/config.h"
#endif
#include "ip_addr.h"

extern u32 ip4_address, ip4_gateway_address, ip4_netmask;
typedef unsigned char u8_t;
typedef unsigned long u32_t;

char *
ip4addr_ntoa_r(const u32 *addr, char *buf, int buflen)
{
  u32_t s_addr;
  char inv[3];
  char *rp;
  u8_t *ap;
  u8_t rem;
  u8_t n;
  u8_t i;
  int len = 0;

  s_addr = *addr;

  rp = buf;
  ap = (u8_t *)&s_addr;
  for (n = 0; n < 4; n++) {
    i = 0;
    do {
      rem = *ap % (u8_t)10;
      *ap /= (u8_t)10;
      inv[i++] = (char)('0' + rem);
    } while (*ap);
    while (i--) {
      if (len++ >= buflen) {
        return NULL;
      }
      *rp++ = inv[i];
    }
    if (len++ >= buflen) {
      return NULL;
    }
    *rp++ = '.';
    ap++;
  }
  *--rp = 0;
  return buf;
}



void
ipnet_addr_as_string(char *buf, unsigned buf_len) {
  char *ip4addr_ntoa_r(const u32 *addr, char *buf, int buflen);
  ip4addr_ntoa_r(&ip4_address, buf, buf_len);
}
