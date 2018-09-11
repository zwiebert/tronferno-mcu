#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "string.h"

#include "time.h"
#include "main/rtc.h"
#include "main/inout.h"

#define TCP_HARD_TIMEOUT  (60 * 10)  // terminate connections to avoid dead connections piling up
#define SERIAL_ECHO 0

#define printf ets_printf
#ifndef DISTRIBUTION
#define D(x)
#else
#define D(x)
#endif

static int (*old_io_putc_fun)(char c);
static int (*old_io_getc_fun)(void);

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
//#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>

#define MY_PORT   7777
#define MAX_BUF   1024

static int sockfd;
static struct sockaddr_in self;
static char buffer[MAX_BUF];
static int clientfd = -1;

int tcps_create_server()
{

  /** Create streaming socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Socket");
    exit(errno);
  }

  /** Initialize address/port structure */
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(MY_PORT);
  self.sin_addr.s_addr = INADDR_ANY;

  /** Assign a port number to the socket */
    if (bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0)
  {
    perror("socket:bind()");
    exit(errno);
  }

  /** Make it a "listening socket". Limit to 20 connections */
  if (listen(sockfd, 20) != 0)
  {
    perror("socket:listen()");
    exit(errno);
  }


  return 0;
}


static int tcp_io_putc(char c) {
  if (1 == send(clientfd, &c, 1, 0))
    return 1;

  return -1;
}

static int  tcp_io_getc(void) {
  char buf[1];
  if (1 == recv(clientfd, buf, 1, 0))
    return buf[0];

  if (errno == EAGAIN || errno == EWOULDBLOCK) {
    return -1;
  }
  close(clientfd);
  clientfd = -1;

  return -1;

}

void
tcps_loop(void) {
  /** Server run continuously */

  if (clientfd < 0) {
    struct sockaddr_in client_addr;
    size_t addrlen=sizeof(client_addr);

    /** accept an incomming connection  */
    clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
    if (clientfd < 0)
      return;

    printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
  }

    io_getc_fun = tcp_io_getc;
    io_putc_fun = tcp_io_putc;

    //close(clientfd);
}


void
setup_tcp_server(void) {
  // create tcp server
  if (tcps_create_server() == 0) {
    printf("tcp server created\n");
    old_io_getc_fun = io_getc_fun;
    old_io_putc_fun = io_putc_fun;
  }


 // old_io_getc_fun = io_getc_fun;
 // old_io_putc_fun = io_putc_fun;


}
