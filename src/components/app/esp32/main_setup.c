#include "main.h"
#include "misc/time/run_time.h"

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
  fam_updateTimerEvent();
}
#endif

#ifdef FER_TRANSMITTER
static void tmr_setBit_txLoop_cb(TimerHandle_t xTimer) {
  lf_setBit(lf_loopFerTx);
}

void tmr_setBit_txLoop_start(u32 interval_ms) {
  TimerHandle_t tmr;
  int interval = pdMS_TO_TICKS(interval_ms);
  tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, (void*) lf_createWifiAp, tmr_setBit_txLoop_cb);
  if (xTimerStart(tmr, 10 ) != pdPASS) {
    printf("CheckNetworkTimer start error");
  }
}

void loop_setBit_txLoop(u32 time_ts) {
  u32 now_ts = get_now_time_ts();
  if (now_ts >= time_ts) {
    lf_setBit(lf_loopFerTx);
    return;
  }
  u32 delay_ms = (time_ts - now_ts) * 100;
  tmr_setBit_txLoop_start(delay_ms);
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

void loop_setPerBit_loopAutoSave(void) {
  lfPer_setBit(lf_loopPosAutoSave);
}
void loop_clrPerBit_loopAutoSave(void) {
  lfPer_clrBit(lf_loopPosAutoSave);
}

void loop_setPerBit_loopCheckMoving(void) {
  lfPer_setBit(lf_loopPosCheckMoving);
}
void loop_clrPerBit_loopCheckMoving(void) {
  lfPer_clrBit(lf_loopPosCheckMoving);
}


void ntpApp_sync_time_cb(struct timeval *tv) {
  ets_printf("ntp synced: %ld\n", time(0));
  lf_setBit(lf_ntpSyncTime);
}

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb(ntpApp_sync_time_cb);
  ntp_setup(cfg_getNtpClient());
}

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;
#ifdef USE_NTP
    ntpApp_setup();
#endif
#ifdef USE_MQTT
    io_mqttApp_setup(cfg_getMqttClient());
#endif
#ifdef USE_TCPS
  tcpCli_setup(cfg_getTcpsServer());
#endif
#ifdef USE_TCPS_TASK
  tcpCli_setup_task(cfg_getTcpsServer());
#endif
#ifdef USE_HTTP
  hts_setup(cfg_getHttpServer());
#endif
  }
}



void mcu_init() {
  uart_mutex = xSemaphoreCreateRecursiveMutex();
#ifdef USE_EG
  loop_eventBits_setup();
#endif

  txtio_setup(cfg_getTxtio());
  kvs_setup();
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
  fam_updateTimerEvent();
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
    wifistation_setup(cfg_getWlan());
    break;
#endif
#ifdef USE_WLAN_AP
  case nwWlanAp:
    esp_netif_init();
    wifiAp_setup(WIFI_AP_SSID, WIFI_AP_PASSWD);
    break;
#endif
#ifdef USE_LAN
  case nwLan:
    esp_netif_init();
    ethernet_setup(cfg_getLan());
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
