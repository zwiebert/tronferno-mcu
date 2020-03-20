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

#define TCP_HARD_TIMEOUT  (60 * 10)  // terminate connections to avoid dead connections piling up
#define PUTC_LINE_BUFFER 1
#define SERIAL_ECHO 1
#define SERIAL_INPUT 1

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x)
#else
#define D(x)
#endif

#if 0
#define DV(x) do { if (C.app_verboseOutput >= vrbDebug) { x; } } while(0)
#else
#undef DV
#define DV(x)
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
#define RX_BUFSIZE 128  // power of 2
static u8 rx_buf[RX_BUFSIZE];
static u8 rx_head = 0, rx_tail = 0;
#define rxb_isEmpty() (rx_head == rx_tail)
#define  rxb_push(c) do {  rx_buf[rx_tail++] = (c);   wrap_idx(rx_tail, RX_BUFSIZE);  } while(0)
static bool  rxb_pop(u8 *res) {
  if (rxb_isEmpty())
    return false;
  *res = rx_buf[rx_head++];
  wrap_idx(rx_head, RX_BUFSIZE);
  return true;
}
#if !PUTC_LINE_BUFFER
#define TX_BUFSIZE 128 // power of 2
static u8 tx_buf[RX_BUFSIZE];
static u8 tx_head = 0, tx_tail = 0;
#define txb_isEmpty() (tx_head == tx_tail)
#define  txb_push(c) do {  tx_buf[tx_tail++] = (c);   wrap_idx(tx_tail, TX_BUFSIZE);  } while(0)
static bool  txb_pop(u8 *res) {
  if (txb_isEmpty())
    return false;
  *res = tx_buf[tx_head++];
  wrap_idx(tx_head, TX_BUFSIZE);
  return true;
}
#endif

static int tx_pending;
/////////////////////////////////////


static int 
tcpSocket_io_getc(void) {
  u8 c;

#if SERIAL_INPUT
  if (old_io_getc_fun) {

    int cc = old_io_getc_fun();
    if (cc >= 0) {
      return cc;
    }
  }
#endif


  if (rxb_pop(&c)) {

    if (nmbConnected <= 0) {
      if (rxb_isEmpty()) {
        io_getc_fun = old_io_getc_fun;
        DV(printf("reset io_getc_fun to serial UART\n"));
        rx_head = rx_tail = 0;
      }
    }
    D(printf("(%c)", c));
    return c;
  }

  return -1;
}
#if PUTC_LINE_BUFFER
#define TCPS_LINE_LEN 120
static char line_buf[TCPS_LINE_LEN];
static int line_idx;
static bool line_complete;

static int 
write_line(void) {
  u8 i;

  if (line_idx == 0)
    return -1;

  DV(printf("write line (len=%d) to tcp\n", line_idx));

  for (i = 0; i < NMB_CLIENTS; ++i) {
    if (tcpclient_espconn[i] == 0)
      continue;


    bool success = false;
    int result;
    switch (result = espconn_send(tcpclient_espconn[i], line_buf, line_idx)) {

    case ESPCONN_OK:
      ++tx_pending;
      success = true;
      DV(printf("send success. length=%d (client_idx=%d)\n", (int)line_idx, i));
      break;

    case ESPCONN_INPROGRESS:
      DV(printf("tcps: ESPCONN_INPROGRESS\n"));
     break;

    case ESPCONN_MAXNUM:
      DV(printf("tcps: ESPCONN_MAXNUM\n"));
      break;

    case ESPCONN_ABRT:
    case ESPCONN_RST:
    case ESPCONN_CLSD:
    case ESPCONN_CONN:
      DV(printf("tcps: disconnect client (idx=%d) because of failed send\n", i));
      espconn_disconnect(tcpclient_espconn[i]);
      tcpclient_espconn[i] = NULL;
      --nmbConnected;
      break;
    }

    if (!success) {
     DV(printf("##send failure. length=%d (client_idx=%d) result=%d\n", (int)line_idx, i, result));

    }
 }

  line_idx = 0;
  return 1;

}

static int 
tcpSocket_io_putc(char c) {
  u8 i;

#if SERIAL_ECHO
  if (old_io_putc_fun)
    old_io_putc_fun(c);
#endif

  if (line_idx < TCPS_LINE_LEN) {
    line_buf[line_idx++] = c;
    if (c != '\n' && (line_idx <= TCPS_LINE_LEN)) {
      return 1;
    }
  }

  if (tx_pending > 0) {
    DV(printf("tcps tx_pending %d\n", tx_pending));
    return 1;
  } else {
    return write_line();
  }
}
#else
static int 
tcpSocket_io_putc(char c) {
  u8 new_tail = tx_tail + 1;
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
#endif
///////////////////////////////////////


#if !PUTC_LINE_BUFFER
static void 
tcps_send_cb(void *arg) {
  --tx_pending;
  (printf("%s(%p), tx_pend=%d\n", __func__, arg, tx_pending));
  if (tx_pending < 0)
    tx_pending = 0;
}
#else
static void 
tcps_send_cb(void *arg) {
  --tx_pending;
  (printf("%s(%p), tx_pend=%d\n", __func__, arg, tx_pending));
  if (tx_pending < 0)
    tx_pending = 0;
}
#endif


static void 
tcps_tx_loop(void) {
#if !PUTC_LINE_BUFFER
  int i;
  static int pendingCount;

  // recover from unbalanced callbacks
  if (tx_pending && ++pendingCount < 0) {
    tx_pending = 0;
    pendingCount = 0;
    DV(printf("pending unbalanced\n"));
  }

  if (!tx_pending && tx_head != tx_tail) {
    u8 h = tx_head, t = tx_tail, l;

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
          DV(printf("send success. length=%d\n", (int)l));
        }
      }
    }
    tx_head = (l + h) % TX_BUFSIZE;
  }
#else
  if (tx_pending <= 0) {
    write_line();
  }
#endif
}

static void 
tcps_disconnect_cb(void *arg) {
  int i;
  struct espconn *pesp_conn = arg;
  bool match = false;

  DV(printf("tcp client disconnected: %d.%d.%d.%d:%d\n", pesp_conn->proto.tcp->remote_ip[0], pesp_conn->proto.tcp->remote_ip[1],
      pesp_conn->proto.tcp->remote_ip[2], pesp_conn->proto.tcp->remote_ip[3], pesp_conn->proto.tcp->remote_port));

  DV(printf("%s(%p), clients=%d\n", __func__, arg, nmbConnected));

  if (arg) {
    for (i = 0; i < NMB_CLIENTS; ++i) {
      if (tcpclient_espconn[i] == NULL) {
        continue;
      } else {
        esp_tcp *a = pesp_conn->proto.tcp, *b = tcpclient_espconn[i]->proto.tcp;

        if (a->remote_port == b->remote_port && 0 == memcmp(a->remote_ip, b->remote_ip, sizeof(a->remote_ip))) {
          tcpclient_espconn[i] = 0;
          --nmbConnected;
          DV(printf("match. connections=%d\n", (int)nmbConnected));
          match = true;
          break;
        }
      }
    }

    if (!match) {
      DV(printf("no match\n"));
      if (nmbConnected > 0)
        return;
    }
  }

  if (nmbConnected <= 0) {
    io_putc_fun = old_io_putc_fun;
    DV(printf("reset io_putc_fun to serial UART\n"));
    nmbConnected = 0;

#if !PUTC_LINE_BUFFER
    while (tx_head != tx_tail) {
      io_putc(tx_buf[tx_head++]);
      wrap_idx(tx_head, TX_BUFSIZE);
    }
    tx_head = tx_tail = 0;
#endif
    tx_pending = 0;
    os_bzero(tcpclient_espconn, sizeof tcpclient_espconn);

    if (rxb_isEmpty()) {
      io_getc_fun = old_io_getc_fun;
      DV(printf("reset io_getc_fun to serial UART\n"));
      rx_head = rx_tail = 0;
    }
  }
}

static void 
tcps_recv_cb(void *arg, char *pdata, unsigned short len) {
  tcpc_last_received = arg;

  timeout_disconnect_all = run_time(0) + TCP_HARD_TIMEOUT;

  while (len-- > 0) {
    rxb_push(*pdata++);
  }
}

static void 
tcps_connect_cb(void *arg) {
  int i;

  DV(printf("%s(%p)\n", __func__, arg));
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

  DV(printf("tcp client %d connected: %d.%d.%d.%d:%d\n", nmbConnected, pesp_conn->proto.tcp->remote_ip[0], pesp_conn->proto.tcp->remote_ip[1], pesp_conn->proto.tcp->remote_ip[2],
      pesp_conn->proto.tcp->remote_ip[3], pesp_conn->proto.tcp->remote_port));

  io_getc_fun = tcpSocket_io_getc;
  io_putc_fun = tcpSocket_io_putc;
  espconn_regist_disconcb(pesp_conn, tcps_disconnect_cb);
  espconn_regist_recvcb(pesp_conn, tcps_recv_cb);
  espconn_regist_write_finish(pesp_conn, tcps_send_cb);
  espconn_regist_sentcb(pesp_conn, tcps_send_cb);

  io_putd(nmbConnected), io_puts(" tcp client(s) connected\n");
}

#if !PUTC_LINE_BUFFER
static bool command_processing_done;
void 
tcps_command_processing_hook(bool done) {
  if (!done) {

  } else {
    command_processing_done = true;
  }
}
#endif

void 
tcps_loop(void) {
  int i;

  tcps_tx_loop();

#if !PUTC_LINE_BUFFER
  if (command_processing_done && txb_isEmpty()) {
    tcpc_last_received = 0;
    command_processing_done = false;
  }
#endif

  // cleanup
  if (nmbConnected > 0 && timeout_disconnect_all < run_time(NULL)) {
    for (i = 0; i < NMB_CLIENTS; ++i) {
      if (tcpclient_espconn[i] != NULL) {
        DV(printf("tcps: disconnect client (idx=%d) because of time out\n", i));
        espconn_disconnect(tcpclient_espconn[i]);
        tcpclient_espconn[i] = NULL;
      }
    }
    nmbConnected = 0;
    rx_head = rx_tail = 0;
#if !PUTC_LINE_BUFFER
    tx_head = tx_tail = 0;
    tx_pending = 0;
#endif


    io_getc_fun = old_io_getc_fun;
    io_putc_fun = old_io_putc_fun;
  }
}

void 
tcps_startServer(void) {
  // create tcp server
  struct espconn *pesp_conn = os_zalloc((uint32 )sizeof(struct espconn));
  int result = 0;

  old_io_getc_fun = io_getc_fun;
  old_io_putc_fun = io_putc_fun;

  if (!(tcpserver_espconn = pesp_conn)) {
    DV(printf("TCP server failure. Out of memory.\n"));
    return;
  }

  pesp_conn->type = ESPCONN_TCP;
  pesp_conn->state = ESPCONN_NONE;
  pesp_conn->proto.tcp = (esp_tcp *) os_zalloc((uint32 )sizeof(esp_tcp));
  pesp_conn->proto.tcp->local_port = 7777;      // server port
  espconn_regist_connectcb(pesp_conn, tcps_connect_cb);
  espconn_tcp_set_max_con_allow(pesp_conn, NMB_CLIENTS);

  result = espconn_accept(pesp_conn);
  DV(printf("tcps: espconn_accept: %d\n", result));
  espconn_regist_time(pesp_conn, 180, 0);

}
