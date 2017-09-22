#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <espconn.h>
#include <mem.h>

#include "driver/uart.h"
#include "config.h"
#include "inout.h"

#define printf io_printf_fun

// TCP Server //////////////////////////////////
static struct espconn *pTcpServer;
int (*old_io_putc_fun)(char c);
int (*old_io_getc_fun)(void);
int (*old_io_printf_fun)(const char *fmt, ...);

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

static bool tx_pending = false;

static void ICACHE_FLASH_ATTR
trans_tcpclient_write_cb(void *arg) {
	tx_pending = false;
}

void ICACHE_FLASH_ATTR
tcpSocket_txTask(void) {

	if (!tx_pending && tx_head != tx_tail) {
		//uint8_t len = ((tx_tail + TX_BUFSIZE) - tx_head) % TX_BUFSIZE;
		uint8_t h = tx_head, t = tx_tail, l;

		l = (h <= t) ? t - h : TX_BUFSIZE - h;

		if (0 == espconn_send(pTcpServer, tx_buf + h, l)) {
			tx_pending = true;
			tx_head = (l + h) % TX_BUFSIZE;
		}
	}

}

static void ICACHE_FLASH_ATTR
trans_tcpclient_discon_cb(void *arg) {
	io_getc_fun = old_io_getc_fun;
	rx_head = rx_tail = 0;

	if ((io_putc_fun = old_io_putc_fun) != NULL) {
		while (tx_head != tx_tail) {
			io_putc_fun(tx_buf[tx_head++]);
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
	printf("callback tcp_server_listen\n");
	struct espconn *pespconn = (struct espconn *) arg;

	if (pespconn == NULL) {
		return;
	}

	espconn_regist_disconcb(pespconn, trans_tcpclient_discon_cb);

	espconn_regist_recvcb(pespconn, trans_tcpclient_recv);
	old_io_getc_fun = io_getc_fun;
	io_getc_fun = tcpSocket_io_getc;
	old_io_putc_fun = io_putc_fun;
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

	pTcpServer->type = ESPCONN_TCP;
	pTcpServer->state = ESPCONN_NONE;
	pTcpServer->proto.tcp = (esp_tcp *) os_zalloc((uint32 )sizeof(esp_tcp));
	pTcpServer->proto.tcp->local_port = 7777;      // server port
	espconn_regist_connectcb(pTcpServer, tcp_server_listen);
	espconn_accept(pTcpServer);
	espconn_regist_time(pTcpServer, 180, 0);

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

	setup_tcp();

}
