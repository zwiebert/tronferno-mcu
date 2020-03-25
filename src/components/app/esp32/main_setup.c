#include "main.h"

void lfa_gotIpAddr_cb() {
  lf_setBit(lf_gotIpAddr);
}
void lfa_lostIpAddr_cb() {
  lf_setBit(lf_lostIpAddr);
}

void IRAM_ATTR lfa_frxMsgReceived_cb(void) {
  lf_setBit(lf_loopFerRx);
}

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;
#ifdef USE_NTP
    ntp_setup();
#endif
#ifdef USE_MQTT
    io_mqttApp_setup();
#endif
#ifdef USE_TCPS
  tcps_startServer();
#endif
  }
}



void mcu_init() {
  uart_mutex = xSemaphoreCreateRecursiveMutex();
#ifdef USE_EG
  loop_eventBits_setup();
#endif

  kvs_setup();
  txtio_setup();
  config_setup();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  lfPer_setBit(lf_loopCli);
#ifdef USE_TCPS
  lfPer_setBit(lf_loopTcpServer);
#endif
  frx_cbRegister_msgReceived(lfa_frxMsgReceived_cb);
  lfPer_setBit(lf_loopFerRx);
#if ENABLE_SET_ENDPOS
  lfPer_setBit(lf_loopFerSep);
#endif
  lfPer_setBit(lf_loopFerTimerState);
  lfPer_setBit(lf_checkCuasTimeout);
#ifdef USE_PAIRINGS
  lfPer_setBit(lf_checkPairingTimeout);
#endif
  lfPer_setBit(lf_loopFerPos);

  ipnet_cbRegister_gotIpAddr(lfa_gotIpAddr_cb);
  ipnet_cbRegister_lostIpAddr(lfa_lostIpAddr_cb);
#ifdef USE_NETWORK
  switch (C.network) {
#ifdef USE_WLAN
  case nwWlanSta:
    esp_netif_init();
    wifistation_setup();
    break;
#endif
#ifdef USE_WLAN_AP
  case nwWlanAp:
    esp_netif_init();
    wifiAp_setup("tronferno", "tronferno");
    break;
#endif
#ifdef USE_LAN
  case nwLan:
    esp_netif_init();
    ethernet_setup(C.lan_phy, C.lan_pwr_gpio);
#endif
    break;
  default:
    break;
  }
#else
  wifistation_setup();
#endif
  setup_pin();

#ifdef USE_AP_FALLBACK
  tmr_checkNetwork_start();
#endif
#ifdef USE_MUTEX
  void mutex_setup(void);
  mutex_setup();
#endif
  //setup_udp();
  intTimer_setup();
  stor_setup();
  main_setup();
}
