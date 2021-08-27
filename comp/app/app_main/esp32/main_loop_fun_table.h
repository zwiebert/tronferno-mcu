#pragma once

#include "app_config/proj_app_cfg.h"

enum loop_flagbits {
#ifdef USE_NETWORK
  lf_gotIpAddr, lf_lostIpAddr,
#endif
#ifdef USE_NTP
  lf_ntpSyncTime,
#endif
#if defined USE_AP_FALLBACK || defined USE_WLAN_AP
  lf_createWifiAp,
#endif
  lf_loopCli,
#ifdef FER_TRANSMITTER
  lf_loopFerTx,
#endif
#ifdef FER_RECEIVER
  lf_loopFerRx,
#endif
#ifdef USE_SEP
  lf_loopFerSep,
#endif
  lf_loopFerPos,
  lf_loopFerTimerState,
  lf_loopFauTimerDataHasChanged,
#ifdef USE_CUAS
  lf_checkCuasTimeout,
#endif
#ifdef USE_PAIRINGS
  lf_checkPairingTimeout,
#endif
  lf_loopPosAutoSave, lf_loopPosCheckMoving,
  lf_gpio_input_intr,
  lf_mcuRestart,

  //-------------
  lf_configGpio,
  lf_configCc1101,
  lf_configAstro,
#ifdef USE_LAN
    lf_configEthernet,
#endif
#ifdef USE_MQTT
    lf_configMqttAppClient,
#endif
#ifdef USE_HTTP
    lf_configHttpServer,
#endif
    lf_configTxtio,

  //-------------
  lf_Len
};

void loop_fun_table_call(loop_flagbits fb);
void loop_fun_table_calls(uint32_t bitmask);




