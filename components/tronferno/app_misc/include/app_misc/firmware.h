/**
 * \file      app_misc/firmware.h
 * \brief     Firmware OTA update
 */

#pragma once

#include <stdint.h>

#ifdef CONFIG_APP_USE_OTA
#include "firmware_update/ota.h"

extern const char ca_cert_pem[]; ///< embedded HTTPS certificate to download firmware

/**
 * \brief      doing a firmware update from URL
 * \param url  HTTPS firmware URL
 * \param cert HTTPS certificate for URL (defaults to embedded ca_cert_pem)
 */
#ifndef CONFIG_APP_OTA_USE_CERT_BUNDLE
inline bool app_doFirmwareUpdate(const char *url, const char *cert = ca_cert_pem) { return ota_doUpdate(url, cert); }
#else
inline bool app_doFirmwareUpdate(const char *url) { return ota_doUpdate(url); }
#endif

#endif
/**
 * \brief       delay program or suspend task for a given time
 * \param us    duration in microseconds
 */
void mcu_delay_us(uint16_t us);

/// \brief      doing a clean MCU restart
void mcu_restart(void);



