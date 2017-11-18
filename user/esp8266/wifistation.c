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

// TCP Server //////////////////////////////////
static struct espconn *pTcpServer;
#define NMB_CLIENTS 6
static struct espconn *tcpClients[NMB_CLIENTS];
static int nmbConnected;

// disconnect callback arg points to server, not client. Should be wrong, but there is no documentation?
// use a table of callback functions as workaround
static void trans_tcpclient_discon_cb(void *arg);
static void ICACHE_FLASH_ATTR bug_discon_cb_00(void *arg) { trans_tcpclient_discon_cb(tcpClients[0]); }
static void ICACHE_FLASH_ATTR bug_discon_cb_01(void *arg) { trans_tcpclient_discon_cb(tcpClients[1]); }
static void ICACHE_FLASH_ATTR bug_discon_cb_02(void *arg) { trans_tcpclient_discon_cb(tcpClients[2]); }
static void ICACHE_FLASH_ATTR bug_discon_cb_03(void *arg) { trans_tcpclient_discon_cb(tcpClients[3]); }
static void ICACHE_FLASH_ATTR bug_discon_cb_04(void *arg) { trans_tcpclient_discon_cb(tcpClients[4]); }
static void ICACHE_FLASH_ATTR bug_discon_cb_05(void *arg) { trans_tcpclient_discon_cb(tcpClients[5]); }
static const espconn_connect_callback bug_discon_cb[NMB_CLIENTS] = { bug_discon_cb_00, bug_discon_cb_01, bug_discon_cb_02, bug_discon_cb_03, bug_discon_cb_04, bug_discon_cb_05, };

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
	tx_buf[tx_tail++] = c;
	tx_tail %= TX_BUFSIZE;

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

	if (!tx_pending && tx_head != tx_tail) {
		uint8_t h = tx_head, t = tx_tail, l;

		l = (h <= t) ? t - h : TX_BUFSIZE - h;

		for (i=0; i < NMB_CLIENTS; ++i) {
			if (tcpClients[i] == 0)
				continue;
			if (0 == espconn_send(tcpClients[i], tx_buf + h, l)) {
				++tx_pending;
			} else {
				trans_tcpclient_discon_cb(tcpClients[i]);  // FIXME: in case the callback was not sent for disconnecting
			}
		}
		tx_head = (l + h) % TX_BUFSIZE;
	}

}

static void ICACHE_FLASH_ATTR
trans_tcpclient_discon_cb(void *arg) {
	int i;
	D(printf("trans_tcpclient_discon_cb(%p), clients=%d\n", arg, nmbConnected));


	for (i=0; i < NMB_CLIENTS; ++i) {
		if (tcpClients[i] == (struct espconn*)arg) {
			tcpClients[i] = 0;
			--nmbConnected;
			break;
		}
	}
	if (i == NMB_CLIENTS)
		return;

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
tcp_server_listen(void *arg) {
	int i;

	D(printf("callback tcp_server_listen(%p)\n", arg));
	struct espconn *pespconn = (struct espconn *) arg;

	if (pespconn == NULL) {
		return;
	}

	if (nmbConnected >= NMB_CLIENTS) {
		return;
	}

	for (i=0; i < NMB_CLIENTS; ++i) {
		if (tcpClients[i] == 0) {
			tcpClients[i] = pespconn;
			++nmbConnected;
			break;
		}
	}
	espconn_regist_disconcb(pespconn, bug_discon_cb[i]);

	espconn_regist_recvcb(pespconn, trans_tcpclient_recv);

	io_getc_fun = tcpSocket_io_getc;
	io_putc_fun = tcpSocket_io_putc;

	espconn_regist_write_finish(pespconn, trans_tcpclient_write_cb);
	espconn_regist_sentcb(pespconn, trans_tcpclient_write_cb);
}

static void ICACHE_FLASH_ATTR
setup_tcp(void) {
	// create tcp server
	pTcpServer = (struct espconn *) os_zalloc((uint32 )sizeof(struct espconn));

	if (pTcpServer == NULL) {
		printf("TcpServer Failure\r\n");
		return;
	}
	D(printf("tcp_server %p\n", (void*)pTcpServer));
	pTcpServer->type = ESPCONN_TCP;
	pTcpServer->state = ESPCONN_NONE;
	pTcpServer->proto.tcp = (esp_tcp *) os_zalloc((uint32 )sizeof(esp_tcp));
	pTcpServer->proto.tcp->local_port = 7777;      // server port
	espconn_regist_connectcb(pTcpServer, tcp_server_listen);
	espconn_accept(pTcpServer);
	espconn_regist_time(pTcpServer, 180, 0);
	espconn_tcp_set_max_con_allow(pTcpServer, NMB_CLIENTS);

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

/*
 enum{
 STATION_IDLE = 0,
 STATION_CONNECTING,
 STATION_WRONG_PASSWORD,
 STATION_NO_AP_FOUND,
 STATION_CONNECT_FAIL,
 STATION_GOT_IP
 };
 */

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
