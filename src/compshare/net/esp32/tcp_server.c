#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "time.h"
#include "app/rtc.h"
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#include "esp_wifi.h"
#include "esp_event.h"

#include "nvs_flash.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include <errno.h>
#include "txtio/inout.h"

#include "tcp_server.h"
#include "misc/int_types.h"

#define TCP_HARD_TIMEOUT  (60 * 10)  // terminate connections to avoid dead connections piling up
#define SERIAL_ECHO 1
#define SERIAL_INPUT 1
#define PUTC_LINE_BUFFER 1

#define printf ets_printf
#ifndef DISTRIBUTION
#define DP(x) printf("db:tcps: %s\n", (x))
#define D(x) x
#else
#define D(x)
#endif

static int (*old_io_putc_fun)(char c);
static int (*old_io_getc_fun)(void);
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
//#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#endif

#define MY_PORT   7777
#define MAX_BUF   1024
#define tcp_io_getc_buf tcp_io_getc

static void modify_io_fun(bool add_connection);

static int sockfd = -1;
static struct sockaddr_in self;
static int cconn_count;


#define TCPS_CCONN_MAX 16

typedef struct {
  int fd;
} tcps_cconn;

static tcps_cconn cconn_table[TCPS_CCONN_MAX];
static u8 cconn_idx;
#define cconn_is_used(idx) (cconn_table[(idx)].fd >= 0)
#define cconn_free_count (TCPS_CCONN_MAX - cconn_count)
#define cconn_curr (cconn_table[cconn_idx])
#define cconn_incr() (cconn_idx = (cconn_idx+1) & (TCPS_CCONN_MAX - 1))

static tcps_cconn *get_next_ccon(bool in_use) {
  u8 i;
  for (i=0; i < TCPS_CCONN_MAX; ++i) {
    cconn_incr();

    if (in_use == cconn_is_used(cconn_idx)) {
      return &cconn_table[cconn_idx];
    }
  }
  return 0;
}

static tcps_cconn *tcps_add_cconn(int fd) {
  if (cconn_free_count == 0)
    return 0;
  tcps_cconn *cc = get_next_ccon(false);
  ++cconn_count;
  cc->fd = fd;
  modify_io_fun(true);
  return cc;
}

static void tcps_close_cconn(int idx) {
  if (close(cconn_table[idx].fd) < 0) perror("close");
  cconn_table[idx].fd = -1;
  --cconn_count;
  printf("tcps: disconnected. %d client(s) still connected\n", cconn_count);
  modify_io_fun(false);
}


static void tcps_close_current_cconn() {
  tcps_close_cconn(cconn_idx);
}

int tcps_create_server() {

  /** Create streaming socket */
    if ((sockfd = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Socket");
    return(errno);
  }
    if (lwip_fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
    {
      perror("fcntl");
      return(errno);
    }
  /** Initialize address/port structure */
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(MY_PORT);
  self.sin_addr.s_addr = INADDR_ANY;

  /** Assign a port number to the socket */
    if (lwip_bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0)
  {
    perror("socket:bind()");
    return(errno);
  }

  /** Make it a "listening socket". Limit to 16 connections */
  if (lwip_listen(sockfd, TCPS_CCONN_MAX) != 0)
  {
    perror("socket:listen()");
    return(errno);
  }


  return 0;
}

#if PUTC_LINE_BUFFER
#define TCPS_LINE_LEN 120
static char line_buf[TCPS_LINE_LEN];
static int line_idx;

static int tcp_io_putc(char c) {
  u8 i;

#if SERIAL_ECHO
  if(old_io_putc_fun)
    old_io_putc_fun(c);
#endif

  if (line_idx < TCPS_LINE_LEN) {
    line_buf[line_idx++] = c;
    if (c != '\n' && (line_idx <= TCPS_LINE_LEN)) {
      return 1;
    }
  }

  //DP("write line to tcp");

  for (i = 0; i < TCPS_CCONN_MAX; ++i) {
    int idx = (i + 1) & (TCPS_CCONN_MAX - 1);
    if (cconn_is_used(idx)) {
      if (lwip_send(cconn_table[idx].fd, line_buf, line_idx, 0) < 0) {  // FIXME: handle partially sent data
        tcps_close_cconn(idx);
      }
    }
  }

  line_idx = 0;
  return 1;
}

#else
static int tcp_io_putc(char c) {
  u8 i;

  if(old_io_putc_fun)
    old_io_putc_fun(c);

  for (i = 0; i < TCPS_CCONN_MAX; ++i) {
    int idx = (i + 1) & (TCPS_CCONN_MAX - 1);
    if (cconn_is_used(idx)) {
      if (send(cconn_table[idx].fd, &c, 1, 0) < 0) {
        tcps_close_cconn(idx);
      }

    }

  }
  return 1;
}
#endif

int  tcps_io_read(char *buf, size_t buf_size, bool incomplete) {
  int result;

  if (!cconn_is_used(cconn_idx)) {
    if (incomplete)
      return -2;
    if (get_next_ccon(true) == 0)
      return -1;
  }


  if ((result = lwip_recv(cconn_curr.fd, buf, buf_size, 0)) > 0)
    return result;


  result = errno;

  if (result == EAGAIN || result == EWOULDBLOCK) {
    get_next_ccon(true);
    return result;
  }
  perror("tcp_io_read");
  tcps_close_current_cconn();

  return result;
}


static int  tcp_io_getc_buf(void) {
  #define BUF_SIZE 80
  static char buf[BUF_SIZE];
  static u8 idx, used;
  int c, res;

  if (old_io_getc_fun && (c = old_io_getc_fun()) >= 0) return c;

  if (used) {
    if (idx < used) {
      return buf[idx++];
    } else {
      used = idx = 0;
    }
  }
  
  if (cconn_count == 0)
    return -1;
#if 1
  if (!cconn_is_used(cconn_idx)  && get_next_ccon(true) == 0)
    return -1;
#endif

  //DP("getc start");
  if ((res = lwip_recv(cconn_curr.fd, buf, BUF_SIZE, MSG_DONTWAIT)) > 0) {
    used = res;
    return buf[idx++];
  }
  //DP("getc end");



  if (errno == EAGAIN || errno == EWOULDBLOCK) {
    get_next_ccon(true);
    return -1;
  }

  perror("tcp_io_getc");
  tcps_close_current_cconn();

  return -1;
}

 
void
tcps_loop(void) {
  /** Server run continuously */
  if (sockfd < 0)
    return;

  if (cconn_free_count > 0) {
    int fd;
    struct sockaddr_in client_addr;
    size_t addrlen=sizeof(client_addr);

    /** accept an incomming connection  */
    fd = lwip_accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
    if (fd >= 0) {
      tcps_add_cconn(fd);
      printf("%s:%d connected (%d clients)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), cconn_count);
    } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("tcps: accept");

    }
  }

}

static void modify_io_fun(bool add_connection) {
  if (add_connection && cconn_count == 1) {
    // fist connection opened
    printf("modify io to tcp\n");
    old_io_getc_fun = io_getc_fun;
    old_io_putc_fun = io_putc_fun;
    io_getc_fun = tcp_io_getc;
    io_putc_fun = tcp_io_putc;
  } else if (cconn_count == 0) {
    // last connection closed
    printf("modify io to serial\n");
    io_getc_fun = old_io_getc_fun;
    io_putc_fun = old_io_putc_fun;
  }
}

void
tcps_startServer(void) {
  u8 i;

  for (i=0; i < TCPS_CCONN_MAX; ++i) {
    cconn_table[i].fd = -1;
  }

  // create tcp server
  if (tcps_create_server() == 0) {
    printf("tcp server created\n");
  }


}
