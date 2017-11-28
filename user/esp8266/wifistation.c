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
#define D(x) x
#else
#define D(x)
#endif

int (*old_io_putc_fun)(char c);
int (*old_io_getc_fun)(void);

static void tcps_disconnect_cb(void *arg);

// TCP Server //////////////////////////////////
static struct espconn *tcpserver_espconn, *tcpc_last_received;
#define NMB_CLIENTS 6
static struct espconn *tcpclient_espconn[NMB_CLIENTS];
static int nmbConnected;

// circular buffers
#define RX_BUFSIZE 256
static uint8_t rx_buf[RX_BUFSIZE];
static uint8_t rx_head = 0, rx_tail = 0;
#define rxb_isEmpty() (rx_head == rx_tail)
#define  rxb_push(c) do {  rx_buf[rx_tail++] = (c);   rx_tail %= RX_BUFSIZE;  } while(0)

#define TX_BUFSIZE 256
static uint8_t tx_buf[RX_BUFSIZE];
static uint8_t tx_head = 0, tx_tail = 0;
#define txb_isEmpty() (tx_head == tx_tail)
#define  txb_push(c) do {  tx_buf[tx_tail++] = (c);   tx_tail %= TX_BUFSIZE;  } while(0)

#ifdef USE_UDP
#define URX_BUFSIZE 256
static uint8_t urx_buf[URX_BUFSIZE];
static uint8_t urx_head = 0, urx_tail = 0;
#define urxb_isEmpty() (urx_head == urx_tail)
#define urxb_push(c) do { urx_buf[urx_tail++] = (c); urx_tail %= URX_BUFSIZE;  } while(0)
#endif

static int ICACHE_FLASH_ATTR
tcpSocket_io_getc(void) {
	int result = -1;

	if (rx_head != rx_tail) {
		result = rx_buf[rx_head++] & 0xFF;
		rx_head %= RX_BUFSIZE;
	}

	if (nmbConnected <= 0) {
		if (rxb_isEmpty()) {
			io_getc_fun = old_io_getc_fun;
			D(printf("reset io_getc_fun to serial UART\n"));
			rx_head = rx_tail = 0;
		}
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
tcps_send_cb(void *arg) {
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

		if (tcpc_last_received)
	    	espconn_send(tcpc_last_received, tx_buf + h, l);


		for (i = 0; i < NMB_CLIENTS; ++i) {
			int result;
			if (tcpclient_espconn[i] == 0)
				continue;
			if (tcpclient_espconn[i] == tcpc_last_received) {
				continue;
				++tx_pending;
			}
			if (ESPCONN_OK == (result = espconn_send(tcpclient_espconn[i], tx_buf + h, l))) {
				++tx_pending;
				D(printf("send success. length=%d\n", (int)l));
			} else {
				D(printf("send failed. length=%d result=%d\n", (int)l, result));
				if (result == ESPCONN_ARG) { // FIXME: kludge
					if (nmbConnected > 1) {
						tcpclient_espconn[i] = 0;
						--nmbConnected;
					} else {
						tcps_disconnect_cb(tcpclient_espconn[i]);
					}
				}
			}
		}

		tx_head = (l + h) % TX_BUFSIZE;
	} else {
	}

}

static void ICACHE_FLASH_ATTR
tcps_disconnect_cb(void *arg) {
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
			io_putc_fun = old_io_putc_fun;
			D(printf("reset io_putc_fun to serial UART\n"));
			nmbConnected = 0;

			while (tx_head != tx_tail) {
				io_putc(tx_buf[tx_head++]);
				tx_head %= TX_BUFSIZE;
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
	while (len-- > 0) {
		rxb_push(*pdata++);
	}
}

static void ICACHE_FLASH_ATTR
tcps_connect_cb(void *arg) {
	int i;

	D(printf("callback connect (%p)\n", arg));
	struct espconn *pesp_conn = (struct espconn *) arg;

	if (nmbConnected >= NMB_CLIENTS)
		mcu_restart(); // FIXME:

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
	if (i >= NMB_CLIENTS)
		mcu_restart(); // FIXME:

	printf("tcp client connected: %d.%d.%d.%d:%d\n", pesp_conn->proto.tcp->remote_ip[0], pesp_conn->proto.tcp->remote_ip[1], pesp_conn->proto.tcp->remote_ip[2],
			pesp_conn->proto.tcp->remote_ip[3], pesp_conn->proto.tcp->remote_port);

	espconn_regist_disconcb(pesp_conn, tcps_disconnect_cb);

	espconn_regist_recvcb(pesp_conn, tcps_recv_cb);

	io_getc_fun = tcpSocket_io_getc;
	io_putc_fun = tcpSocket_io_putc;

	espconn_regist_write_finish(pesp_conn, tcps_send_cb);
	espconn_regist_sentcb(pesp_conn, tcps_send_cb);
	io_putc('0' + nmbConnected), io_puts(" tcp client(s) connected\n");

}

static void ICACHE_FLASH_ATTR
setup_tcp(void) {
	// create tcp server
	struct espconn *pesp_conn = os_zalloc((uint32 )sizeof(struct espconn));

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

#ifdef USE_UDP
//////////// UDP ///////////////////////////////
int udp_available;



int ICACHE_FLASH_ATTR udp_getc(void) {
	int result = -1;

	if (! urxb_isEmpty()) {
		result = urx_buf[urx_head++] & 0xFF;
		urx_head %= URX_BUFSIZE;
		--udp_available;
	}

	return result;
}


static void ICACHE_FLASH_ATTR
udp_sendcb(void *arg) {
	struct espconn *pesp_conn = arg;
}



static void ICACHE_FLASH_ATTR
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


static bool ICACHE_FLASH_ATTR
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
			&&	ESPCONN_OK == espconn_regist_recvcb(pesp_conn, udp_recvb),
			//&&  ESPCONN_OK == espconn_regist_sentcb(pesp_conn, udp_sendcb),
			1;
}
#endif ///// end UDP /////////////

// WIFI Station ////////////////////////////////////////

void ICACHE_FLASH_ATTR
user_set_station_config(void) {
	struct station_config stationConf;
	stationConf.bssid_set = 0; //need not check MAC address of AP
#ifdef USE_WLAN
	os_memcpy(&stationConf.ssid, C.wifi_SSID, 32);
	os_memcpy(&stationConf.password, C.wifi_password, 64);
#endif
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
#ifdef USE_UDP
	setup_udp();
#endif

}
