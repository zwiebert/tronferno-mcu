#include "main.h"

#ifdef USE_NETWORK
void lfa_gotIpAddr_cb() {
  lf_setBit(lf_gotIpAddr);
}
void lfa_lostIpAddr_cb() {
  lf_setBit(lf_lostIpAddr);
}
#endif

#ifdef FER_TRANSMITTER
void loop_setBit_txLoop() {
  lf_setBit(lf_loopFerTx);
}
void IRAM_ATTR loop_setBit_txLoop_fromISR(bool yield) {
  lf_setBit_ISR(lf_loopFerTx, yield);
}
#endif

#ifdef FER_RECEIVER
void loop_setBit_rxLoop() {
  lf_setBit(lf_loopFerRx);
}
void IRAM_ATTR loop_setBit_rxLoop_fromISR(bool yield) {
  lf_setBit_ISR(lf_loopFerRx, yield);
}
#endif

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
#ifdef USE_TCPS_TASK
#endif
#ifdef FER_TRANSMITTER
  lfPer_setBit(lf_loopFerTx);
#endif
#if ENABLE_SET_ENDPOS
  lfPer_setBit(lf_loopFerSep);
#endif
  lfPer_setBit(lf_loopFerTimerState);
  lfPer_setBit(lf_checkCuasTimeout);
#ifdef USE_PAIRINGS
  lfPer_setBit(lf_checkPairingTimeout);
#endif
  lfPer_setBit(lf_loopFerPos);

#ifdef USE_NETWORK
  ipnet_cbRegister_gotIpAddr(lfa_gotIpAddr_cb);
  ipnet_cbRegister_lostIpAddr(lfa_lostIpAddr_cb);
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
#endif

  setup_pin();

#ifdef USE_AP_FALLBACK
  tmr_checkNetwork_start();
#endif
#ifdef USE_CLI_MUTEX
  void mutex_setup(void);
  mutex_setup();
#endif
  //setup_udp();
  intTimer_setup();
  stor_setup();
  main_setup();
}
