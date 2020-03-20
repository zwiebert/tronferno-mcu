#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <espconn.h>
#include <mem.h>

#include "driver/uart.h"
#include "config/config.h"
#include "time.h"

#include "txtio/inout.h"
#include "app/rtc.h"

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif


#define wrap_idx(idx, limit) (idx &= (limit - 1))

// circular buffers
#define URX_BUFSIZE 256  // power of 2
static u8 urx_buf[URX_BUFSIZE];
static u8 urx_head = 0, urx_tail = 0;
#define urxb_isEmpty() (urx_head == urx_tail)
#define urxb_push(c) do { urx_buf[urx_tail++] = (c); wrap_idx(urx_tail, URX_BUFSIZE);  } while(0)

int udp_available;

int  udp_getc(void) {
  int result = -1;

  if (! urxb_isEmpty()) {
    result = urx_buf[urx_head++] & 0xFF;
    wrap_idx(urx_head, URX_BUFSIZE);
    --udp_available;
  }

  return result;
}

static void 
udp_sendcb(void *arg) {
  struct espconn *pesp_conn = arg;
}

static void 
udp_recvb(void *arg, char *pdata, unsigned short len) {
  struct espconn *pesp_conn = arg;
  while (len-- > 0) {
    urxb_push(*pdata++);
  }
  udp_available += len;
}

bool sendDatagram(char *datagram, uint16 size) {
  static struct espconn sendResponse;
  static esp_udp udp;

  sendResponse.type = ESPCONN_UDP;
  sendResponse.state = ESPCONN_NONE;
  sendResponse.proto.udp = &udp;
  IP4_ADDR((ip_addr_t *)sendResponse.proto.udp->remote_ip, 192, 168, 1, 7);
  sendResponse.proto.udp->remote_port = 9876; // Remote port
  return ESPCONN_OK == espconn_create(&sendResponse) &&
  ESPCONN_OK == espconn_sendto(&sendResponse, datagram, strlen(datagram)) &&
  ESPCONN_OK == espconn_delete(&sendResponse);
}

bool 
setup_udp(void) {
  struct espconn *pesp_conn = os_zalloc((uint32 )sizeof(struct espconn));

  if (!(pesp_conn)) {
    printf("UDP server failure. Out of memory.\n");
    return false;
  }
  pesp_conn->type = ESPCONN_UDP;
  pesp_conn->state = ESPCONN_NONE;
  pesp_conn->proto.udp = (esp_udp *) os_zalloc((uint32 )sizeof(esp_udp));
  pesp_conn->proto.udp->local_port = 7777;      // server port
  return ESPCONN_OK == espconn_create(pesp_conn)
  && ESPCONN_OK == espconn_regist_recvcb(pesp_conn, udp_recvb),
  //&&  ESPCONN_OK == espconn_regist_sentcb(pesp_conn, udp_sendcb),
  1;
}




void 
udp_loop(void) {

}


