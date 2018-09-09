#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <espconn.h>
#include <mem.h>

#include "driver/uart.h"
#include "main/config.h"
#include "main/inout.h"
#include "time.h"
#include "main/rtc.h"

#define TCP_HARD_TIMEOUT  (60 * 10)  // terminate connections to avoid dead connections piling up
#define SERIAL_ECHO 0

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x)
#else
#define D(x)
#endif

static int (*old_io_putc_fun)(char c);
static int (*old_io_getc_fun)(void);

static void tcps_disconnect_cb(void *arg);

// TCP Server //////////////////////////////////
static struct espconn *tcpserver_espconn, *tcpc_last_received;

static time_t timeout_disconnect_all;
#define NMB_CLIENTS 6
static struct espconn *tcpclient_espconn[NMB_CLIENTS];
static int nmbConnected;

#define wrap_idx(idx, limit) (idx &= (limit - 1))

// circular buffers
#define RX_BUFSIZE 256  // power of 2
static uint8_t rx_buf[RX_BUFSIZE];
static uint8_t rx_head = 0, rx_tail = 0;
#define rxb_isEmpty() (rx_head == rx_tail)
#define  rxb_push(c) do {  rx_buf[rx_tail++] = (c);   wrap_idx(rx_tail, RX_BUFSIZE);  } while(0)
static bool ICACHE_FLASH_ATTR rxb_pop(uint8_t *res) {
  if (rxb_isEmpty())
    return false;
  *res = rx_buf[rx_head++];
  wrap_idx(rx_head, RX_BUFSIZE);
  return true;
}

#define TX_BUFSIZE 256 // power of 2
static uint8_t tx_buf[RX_BUFSIZE];
static uint8_t tx_head = 0, tx_tail = 0;
#define txb_isEmpty() (tx_head == tx_tail)
#define  txb_push(c) do {  tx_buf[tx_tail++] = (c);   wrap_idx(tx_tail, TX_BUFSIZE);  } while(0)
static bool ICACHE_FLASH_ATTR txb_pop(uint8_t *res) {
  if (txb_isEmpty())
    return false;
  *res = tx_buf[tx_head++];
  wrap_idx(tx_head, TX_BUFSIZE);
  return true;
}

static int ICACHE_FLASH_ATTR
tcpSocket_io_getc(void) {
  uint8_t c;

  if (rxb_pop(&c)) {

    if (nmbConnected <= 0) {
      if (rxb_isEmpty()) {
        io_getc_fun = old_io_getc_fun;
        D(printf("reset io_getc_fun to serial UART\n"));
        rx_head = rx_tail = 0;
      }
    }
    return c;
  } else if (old_io_getc_fun) {
    return old_io_getc_fun();
  }

  return -1;
}

static int ICACHE_FLASH_ATTR
tcpSocket_io_putc(char c) {
  uint8_t new_tail = tx_tail + 1;
  if (new_tail == TX_BUFSIZE)
    new_tail = 0;

#if SERIAL_ECHO
  if (old_io_putc_fun) {
    old_io_putc_fun(c);
  }
#endif

  if (new_tail == tx_head)
    return -1;   // the only thing we can do, as long txTask is not done by interrupt (if this is possible)

  tx_buf[tx_tail] = c;
  tx_tail = new_tail;

  return 0xff & c;
}

static int tx_pending;

static void ICACHE_FLASH_ATTR
tcps_send_cb(void *arg) {
  --tx_pending;
  D(printf("%s(%p), tx_pend=%d\n", __func__, arg, tx_pending));
  if (tx_pending < 0)
    tx_pending = 0;
}

static void ICACHE_FLASH_ATTR
tcps_tx_loop(void) {
  int i;
  static int pendingCount;

  // recover from unbalanced callbacks
  if (tx_pending && ++pendingCount < 0) {
    tx_pending = 0;
    pendingCount = 0;
    D(printf("pending unbalanced\n"));
  }

  if (!tx_pending && tx_head != tx_tail) {
    uint8_t h = tx_head, t = tx_tail, l;

    l = (h <= t) ? t - h : TX_BUFSIZE - h;

    if (tcpc_last_received) {
      espconn_send(tcpc_last_received, tx_buf + h, l);
      ++tx_pending;
    } else {
      for (i = 0; i < NMB_CLIENTS; ++i) {
        if (tcpclient_espconn[i] == 0)
          continue;
        if (tcpclient_espconn[i] == tcpc_last_received) {
          continue;
          ++tx_pending;
        }
        if (ESPCONN_OK == espconn_send(tcpclient_espconn[i], tx_buf + h, l)) {
          ++tx_pending;
          D(printf("send success. length=%d\n", (int)l));
        }
      }
    }
    tx_head = (l + h) % TX_BUFSIZE;
  }
}

static void ICACHE_FLASH_ATTR
tcps_disconnect_cb(void *arg) {
  int i;
  struct espconn *pesp_conn = arg;

  printf("tcp client disconnected: %d.%d.%d.%d:%d\n", pesp_conn->proto.tcp->remote_ip[0], pesp_conn->proto.tcp->remote_ip[1],
      pesp_conn->proto.tcp->remote_ip[2], pesp_conn->proto.tcp->remote_ip[3], pesp_conn->proto.tcp->remote_port);

  D(printf("%s(%p), clients=%d\n", __func__, arg, nmbConnected));

  for (i = 0; i < NMB_CLIENTS; ++i) {
    if (tcpclient_espconn[i] == NULL) {
      continue;
    } else {
      esp_tcp *a = pesp_conn->proto.tcp, *b = tcpclient_espconn[i]->proto.tcp;

      if (a->remote_port == b->remote_port && 0 == memcmp(a->remote_ip, b->remote_ip, sizeof(a->remote_ip))) {
        tcpclient_espconn[i] = 0;
        --nmbConnected;
        D(printf("match. connections=%d\n", (int)nmbConnected));
        break;
      }
    }
  }

  if (i == NMB_CLIENTS) {
    D(printf("no match\n"));
    return;
  }

  if (nmbConnected <= 0) {
    io_putc_fun = old_io_putc_fun;
    D(printf("reset io_putc_fun to serial UART\n"));
    nmbConnected = 0;

    while (tx_head != tx_tail) {
      io_putc(tx_buf[tx_head++]);
      wrap_idx(tx_head, TX_BUFSIZE);
    }
    tx_head = tx_tail = 0;
    tx_pending = 0;
    os_bzero(tcpclient_espconn, sizeof tcpclient_espconn);

    if (rxb_isEmpty()) {
      io_getc_fun = old_io_getc_fun;
      D(printf("reset io_getc_fun to serial UART\n"));
      rx_head = rx_tail = 0;
    }
  }
}

static void ICACHE_FLASH_ATTR
tcps_recv_cb(void *arg, char *pdata, unsigned short len) {
  tcpc_last_received = arg;

  timeout_disconnect_all = run_time(0) + TCP_HARD_TIMEOUT;

  while (len-- > 0) {
    rxb_push(*pdata++);
  }
}

static void ICACHE_FLASH_ATTR
tcps_connect_cb(void *arg) {
  int i;

  D(printf("%s(%p)\n", __func__, arg));
  struct espconn *pesp_conn = (struct espconn *) arg;

  timeout_disconnect_all = run_time(0) + TCP_HARD_TIMEOUT;

  if (pesp_conn == NULL) {
    return;
  }

  if (nmbConnected >= NMB_CLIENTS) {
    return;
  }

  for (i = 0; i < NMB_CLIENTS; ++i) {
    if (tcpclient_espconn[i] == 0) {
      tcpclient_espconn[i] = pesp_conn;
      ++nmbConnected;
      break;
    }
  }

  printf("tcp client %d connected: %d.%d.%d.%d:%d\n", nmbConnected, pesp_conn->proto.tcp->remote_ip[0], pesp_conn->proto.tcp->remote_ip[1], pesp_conn->proto.tcp->remote_ip[2],
      pesp_conn->proto.tcp->remote_ip[3], pesp_conn->proto.tcp->remote_port);

  io_getc_fun = tcpSocket_io_getc;
  io_putc_fun = tcpSocket_io_putc;
  espconn_regist_disconcb(pesp_conn, tcps_disconnect_cb);
  espconn_regist_recvcb(pesp_conn, tcps_recv_cb);
  espconn_regist_write_finish(pesp_conn, tcps_send_cb);
  espconn_regist_sentcb(pesp_conn, tcps_send_cb);

  io_putd(nmbConnected), io_puts(" tcp client(s) connected\n");
}

static bool command_processing_done;
void ICACHE_FLASH_ATTR
tcps_command_processing_hook(bool done) {
  if (!done) {

  } else {
    command_processing_done = true;
  }
}

void ICACHE_FLASH_ATTR
tcps_loop(void) {
  int i;

  tcps_tx_loop();

  if (command_processing_done && txb_isEmpty()) {
    tcpc_last_received = 0;
    command_processing_done = false;
  }

  // cleanup
  if (nmbConnected > 0 && timeout_disconnect_all < run_time(NULL)) {
    for (i = 0; i < NMB_CLIENTS; ++i) {
      if (tcpclient_espconn[i] != NULL) {
        espconn_disconnect(tcpclient_espconn[i]);
        tcpclient_espconn[i] = NULL;
      }
    }
    nmbConnected = 0;
    tx_head = tx_tail = 0;
    rx_head = rx_tail = 0;
    tx_pending = 0;
    io_getc_fun = old_io_getc_fun;
    io_putc_fun = old_io_putc_fun;
  }
}

void ICACHE_FLASH_ATTR
setup_tcp_server(void) {
  // create tcp server
  struct espconn *pesp_conn = os_zalloc((uint32 )sizeof(struct espconn));

  old_io_getc_fun = io_getc_fun;
  old_io_putc_fun = io_putc_fun;

  if (!(tcpserver_espconn = pesp_conn)) {
    printf("TCP server failure. Out of memory.\n");
    return;
  }

  pesp_conn->type = ESPCONN_TCP;
  pesp_conn->state = ESPCONN_NONE;
  pesp_conn->proto.tcp = (esp_tcp *) os_zalloc((uint32 )sizeof(esp_tcp));
  pesp_conn->proto.tcp->local_port = 7777;      // server port
  espconn_regist_connectcb(pesp_conn, tcps_connect_cb);
  espconn_tcp_set_max_con_allow(pesp_conn, NMB_CLIENTS);

  espconn_accept(pesp_conn);
  espconn_regist_time(pesp_conn, 180, 0);

}
