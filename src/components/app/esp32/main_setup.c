#include "main.h"

#ifdef USE_NETWORK
void lfa_gotIpAddr_cb() {
  lf_setBit(lf_gotIpAddr);
}
void lfa_lostIpAddr_cb() {
  lf_setBit(lf_lostIpAddr);
}
#endif

#ifdef USE_NTP
void lfa_ntpSync(void) {
  fau_getnextTimerEvent();
}
#endif

#ifdef FER_TRANSMITTER
void loop_setBit_txLoop() {
  lf_setBit(lf_loopFerTx);
}
void IRAM_ATTR loop_setBit_txLoop_fromISR() {
  lf_setBit_ISR(lf_loopFerTx, true);
}
#endif

#ifdef FER_RECEIVER
void loop_setBit_rxLoop() {
  lf_setBit(lf_loopFerRx);
}
void IRAM_ATTR loop_setBit_rxLoop_fromISR() {
  lf_setBit_ISR(lf_loopFerRx, true);
}
#endif

void loop_setBit_fauTimerDataHasChanged(void) {
  lf_setBit(lf_loopFauTimerDataHasChanged);
}

#ifdef USE_SEP
void loop_setPerBit_sepLoop(void) {
  lfPer_setBit(lf_loopFerSep);
}
void loop_clrPerBit_sepLoop(void) {
  lfPer_clrBit(lf_loopFerSep);
}
#endif

#ifdef USE_PAIRINGS
void loop_setPerBit_pairLoop(void) {
  lfPer_setBit(lf_checkPairingTimeout);
}
void loop_clrPerBit_pairLoop(void) {
  lfPer_clrBit(lf_checkPairingTimeout);
}
#endif

#ifdef USE_CUAS
void loop_setPerBit_cuasLoop(void) {
  lfPer_setBit(lf_checkCuasTimeout);
}
void loop_clrPerBit_cuasLoop(void) {
  lfPer_clrBit(lf_checkCuasTimeout);
}
#endif

void ferPos_loopAutoSave();
void loop_setPerBit_loopAutoSave(void) {
  lfPer_setBit(lf_loopPosAutoSave);
}
void loop_clrPerBit_loopAutoSave(void) {
  lfPer_clrBit(lf_loopPosAutoSave);
}
#define  fpos_POSITIONS_UNSAVED_cb loop_setPerBit_loopAutoSave
#define  fpos_POSTIONS_SAVED_cb loop_clrPerBit_loopAutoSave


void ferPos_loopCheckMoving();
void loop_setPerBit_loopCheckMoving(void) {
  lfPer_setBit(lf_loopPosCheckMoving);
}
void loop_clrPerBit_loopCheckMoving(void) {
  lfPer_clrBit(lf_loopPosCheckMoving);
}
#define  fpos_HAS_MOVING_cb loop_setPerBit_loopCheckMoving
#define  fpos_HAS_NO_MOVING_cb loop_clrPerBit_loopCheckMoving



void ntpApp_sync_time_cb(struct timeval *tv) {
  ets_printf("ntp synced: %ld\n", time(0));
  lf_setBit(lf_ntpSyncTime);
}

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb(ntpApp_sync_time_cb);
  ntp_setup();
}

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;
#ifdef USE_NTP
    ntpApp_setup();
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
  //lfPer_setBit(lf_loopFerTx);
#endif
  fau_getnextTimerEvent();
  lfPer_setBit(lf_loopFerTimerState);
  //lfPer_setBit(lf_checkCuasTimeout);
#ifdef USE_PAIRINGS
 // lfPer_setBit(lf_checkPairingTimeout);
#endif
//  lfPer_setBit(lf_loopFerPos);

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
