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

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x)
#else
#define D(x)
#endif


int (*old_io_putc_fun)(char c);
int (*old_io_getc_fun)(void);


static void hook_tcpclient_disconnect(void *arg);

// TCP Server //////////////////////////////////
static struct espconn *tcpserver_espconn;
#define NMB_CLIENTS 6
static struct espconn *tcpclient_espconn[NMB_CLIENTS];
static int nmbConnected;

// circular buffers
#define RX_BUFSIZE 256
static uint8_t buf[RX_BUFSIZE];
static uint8_t rx_head = 0, rx_tail = 0;

#define TX_BUFSIZE 256
static uint8_t tx_buf[RX_BUFSIZE];
static uint8_t tx_head = 0, tx_tail = 0;


static void ICACHE_FLASH_ATTR
rx_copy(uint8_t *start, uint8_t *end) {
	while (start < end) {
		buf[rx_tail++] = *start++;
		rx_tail %= RX_BUFSIZE;
	}
}

static int ICACHE_FLASH_ATTR
tcpSocket_io_getc(void) {
	int result = -1;

	if (rx_head != rx_tail) {
		result = buf[rx_head++] & 0xFF;
		rx_head %= RX_BUFSIZE;
	}

	return result;
}

static int ICACHE_FLASH_ATTR
tcpSocket_io_putc(char c) {
	uint8_t new_tail = tx_tail + 1;
	if (new_tail == TX_BUFSIZE)
		new_tail = 0;

	if (new_tail == tx_head)
		return -1;   // the only thing we can do, as long txTask is not done by interrupt (if this is possible)

	tx_buf[tx_tail] = c;
	tx_tail = new_tail;

	return 0xff & c;
}

static int tx_pending;

static void ICACHE_FLASH_ATTR
trans_tcpclient_write_cb(void *arg) {
	--tx_pending;
	D(printf("trans_tcpclient_write_cb(%p), tx_pend=%d\n", arg, tx_pending));
  if (tx_pending < 0)
	  tx_pending = 0;
}

void ICACHE_FLASH_ATTR
tcpSocket_txTask(void) {
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

		for (i=0; i < NMB_CLIENTS; ++i) {
			if (tcpclient_espconn[i] == 0)
				continue;
			if (0 == espconn_send(tcpclient_espconn[i], tx_buf + h, l)) {
				++tx_pending;
				D(printf("send success. length=%d\n", (int)l));
			} else {
				D(printf("send failed. length=%d\n", (int)l));
				//hook_tcpclient_disconnect(tcpclient_espconn[i]);  // FIXME: in case the callback was not sent for disconnecting
			}
		}
		tx_head = (l + h) % TX_BUFSIZE;
	} else {
		if (tx_pending)
		D(printf(" #%d# ", (int)tx_pending));
	}

}

static void ICACHE_FLASH_ATTR
hook_tcpclient_disconnect(void *arg) {
	int i;
	struct espconn *pesp_conn = arg;

	printf("tcp client disconnected: %d.%d.%d.%d:%d\n", pesp_conn->proto.tcp->remote_ip[0], pesp_conn->proto.tcp->remote_ip[1],
			pesp_conn->proto.tcp->remote_ip[2], pesp_conn->proto.tcp->remote_ip[3], pesp_conn->proto.tcp->remote_port);

	D(printf("trans_tcpclient_discon_cb(%p), clients=%d\n", arg, nmbConnected));

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
		io_getc_fun = old_io_getc_fun;
		io_putc_fun = old_io_putc_fun;
		D(printf("reset io functions to serial UART\n"));
		nmbConnected = 0;
		rx_head = rx_tail = 0;

		while (tx_head != tx_tail) {
			io_putc(tx_buf[tx_head++]);
			tx_head %= TX_BUFSIZE;
		}
		tx_head = tx_tail = 0;
	}
}


static void ICACHE_FLASH_ATTR
trans_tcpclient_recv(void *arg, char *pdata, unsigned short len) {
	rx_copy(pdata, pdata + len);
}

static void ICACHE_FLASH_ATTR
hook_tcpclient_connect(void *arg) {
	int i;

	D(printf("callback connect (%p)\n", arg));
	struct espconn *pesp_conn = (struct espconn *) arg;

	if (nmbConnected >= NMB_CLIENTS) mcu_restart(); // FIXME:

	if (pesp_conn == NULL) {
		return;
	}



	if (nmbConnected >= NMB_CLIENTS) {
		return;
	}

	for (i=0; i < NMB_CLIENTS; ++i) {
		if (tcpclient_espconn[i] == 0) {
			tcpclient_espconn[i] = pesp_conn;
			++nmbConnected;
			break;
		}
	}
	if (i >= NMB_CLIENTS) mcu_restart(); // FIXME:

	printf("tcp client connected: %d.%d.%d.%d:%d\n", pesp_conn->proto.tcp->remote_ip[0], pesp_conn->proto.tcp->remote_ip[1],
			pesp_conn->proto.tcp->remote_ip[2], pesp_conn->proto.tcp->remote_ip[3], pesp_conn->proto.tcp->remote_port);


	espconn_regist_disconcb(pesp_conn, hook_tcpclient_disconnect);

	espconn_regist_recvcb(pesp_conn, trans_tcpclient_recv);

	io_getc_fun = tcpSocket_io_getc;
	io_putc_fun = tcpSocket_io_putc;

	espconn_regist_write_finish(pesp_conn, trans_tcpclient_write_cb);
	espconn_regist_sentcb(pesp_conn, trans_tcpclient_write_cb);
	io_putc('0'+nmbConnected), io_puts(" tcp client(s) connected\n");

}

static void ICACHE_FLASH_ATTR
setup_tcp(void) {
	// create tcp server
	tcpserver_espconn = (struct espconn *) os_zalloc((uint32 )sizeof(struct espconn));

	if (tcpserver_espconn == NULL) {
		printf("TCP server failure. Out of memory.\n");
		return;
	}
	D(printf("tcp_server %p\n", (void*)tcpserver_espconn));
	tcpserver_espconn->type = ESPCONN_TCP;
	tcpserver_espconn->state = ESPCONN_NONE;
	tcpserver_espconn->proto.tcp = (esp_tcp *) os_zalloc((uint32 )sizeof(esp_tcp));
	tcpserver_espconn->proto.tcp->local_port = 7777;      // server port
	espconn_regist_connectcb(tcpserver_espconn, hook_tcpclient_connect);
	espconn_tcp_set_max_con_allow(tcpserver_espconn, NMB_CLIENTS);

	espconn_accept(tcpserver_espconn);
	//espconn_regist_time(tcpserver_espconn, 180, 0);

}

// WIFI Station ////////////////////////////////////////

void ICACHE_FLASH_ATTR
user_set_station_config(void) {
	struct station_config stationConf;
	stationConf.bssid_set = 0; //need not check MAC address of AP
	os_memcpy(&stationConf.ssid, C.wifi_SSID, 32);
	os_memcpy(&stationConf.password, C.wifi_password, 64);
	wifi_station_set_config(&stationConf);
}


void wst_reconnect(void) {
	uint8_t status = wifi_station_get_connect_status();
	io_printf_fun("wifi state: %d\n", (int) 0xff & status);

	//wifi_station_connect();
}

void ICACHE_FLASH_ATTR
wifiStation_loop(void) {
	tcpSocket_txTask();
}

void ICACHE_FLASH_ATTR
setup_wifistation(void) {
	wifi_set_opmode(STATION_MODE);
	user_set_station_config();
	old_io_getc_fun = io_getc_fun;
	old_io_putc_fun = io_putc_fun;
	setup_tcp();

}
