/**
 * \file      app/firmware.h
 * \brief     Firmware OTA update
 */

#pragma once
#include <app/config/proj_app_cfg.h>
#ifdef USE_OTA
#include "firmware_update/ota.h"

#ifdef USE_LAN
#define OTA_FWURL_MASTER "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/master/firmware/esp32/tronferno-mcu-lan.bin"
#define OTA_FWURL_BETA "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/beta/firmware/esp32/tronferno-mcu-lan.bin"
#else
#define OTA_FWURL_MASTER "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/master/firmware/esp32/tronferno-mcu.bin"
#define OTA_FWURL_BETA "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/beta/firmware/esp32/tronferno-mcu.bin"
#endif

#define OTA_FWURL_TAG_HEAD "https://raw.githubusercontent.com/zwiebert/tronferno-mcu-bin/"
#define OTA_FWURL_TAG_TAIL "/firmware/esp32/tronferno-mcu.bin"
#define OTA_FWURL_TAG_COOKIE "tag:"


extern const char ca_cert_pem[]; ///< embedded HTTPS certificate to download firmware

/**
 * \brief      doing a firmware update from URL
 * \param url  HTTPS firmware URL
 * \param cert HTTPS certificate for URL (defaults to embedded ca_cert_pem)
 */
inline bool app_doFirmwareUpdate(const char *url, const char *cert = ca_cert_pem) { return ota_doUpdate(url, cert); }

#endif

/**
 * \brief       delay program or suspend task for a given time
 * \param us    duration in microseconds
 */
void mcu_delay_us(u16 us);

/// \brief      doing a clean MCU restart
void mcu_restart(void);



