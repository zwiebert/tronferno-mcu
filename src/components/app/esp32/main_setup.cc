#include "main.h"
#include "misc/time/run_time.h"
#include "key_value_store/kvs_wrapper.h"
#include "cli/mutex.hh"
#include "net/http/server/content/setup.h"
#include <cli/cli.h>

void loop_setBit_mcuRestart() {
  lf_setBit(lf_mcuRestart);
}
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
  tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, (void*) 0, tmr_setBit_txLoop_cb);
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

void loop_setBit_pinNotifyInputChange_fromISR() {
  lf_setBit_ISR(lf_gpio_input_intr, true);
}

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
  config_setup_ntpClient();
}

extern "C" void main_setup_ip_dependent() { //XXX called from library
  static int once;
  so_output_message(SO_INET_PRINT_ADDRESS, 0);
  if (!once) {
    once = 1;
#ifdef USE_NTP
    ntpApp_setup();
#endif
#ifdef USE_MQTT
    config_setup_mqttAppClient();
#endif
#if defined USE_TCPS || defined USE_TCPS_TASK
    config_setup_cliTcpServer();
#endif
#ifdef USE_HTTP
  config_setup_httpServer();
#endif
  }
}



void mcu_init() {
#ifdef USE_EG
  loop_eventBits_setup();
#endif
  kvs_setup();
  config_setup_txtio();

  config_setup_global();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());
#ifdef USE_CLI_TASK
  cli_setup_task(true);
#else
  lfPer_setBit(lf_loopCli);
#endif
#ifdef USE_TCPS
  lfPer_setBit(lf_loopTcpServer);
#endif
#ifdef USE_TCPS_TASK
#endif
  fam_updateTimerEvent();
  lfPer_setBit(lf_loopFerTimerState);


#ifdef USE_NETWORK
  enum nwConnection network = config_read_network_connection();
#ifdef USE_AP_FALLBACK
  esp_netif_init();
#else
  if (network != nwNone)
    esp_netif_init();
#endif

  ipnet_cbRegister_gotIpAddr(lfa_gotIpAddr_cb);
  ipnet_cbRegister_lostIpAddr(lfa_lostIpAddr_cb);
  switch (network) {
#ifdef USE_WLAN
  case nwWlanSta:
    config_setup_wifiStation();
    break;
#endif
#ifdef USE_WLAN_AP
  case nwWlanAp:
    lfa_createWifiAp(); // XXX: Create the fall-back AP. Should we have a regular configured AP also?
    break;
#endif
#ifdef USE_LAN
  case nwLan:
    config_setup_ethernet();
#endif
    break;
  default:
    break;
  }
#ifdef USE_HTTP
    hts_setup_content();
#endif
#endif

  config_setup_gpio();

#ifdef USE_AP_FALLBACK
  if (network != nwWlanAp)
    tmr_checkNetwork_start();
#endif
  intTimer_setup();
  stor_setup();
  main_setup();

  kvshT h;
  if ((h = kvs_open("misc", kvs_READ_WRITE))) {
    boot_counter = kvs_get_i32(h, "boot_ct", 0, 0) + 1;
    kvs_set_i32(h, "boot_ct", boot_counter);
    kvs_close(h);
  }
}
