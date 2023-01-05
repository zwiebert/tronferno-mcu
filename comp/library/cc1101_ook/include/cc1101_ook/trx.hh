/**
 * \file   cc1101_ook/trx.hh
 * \brief  CC1101 433.92 MHz transceiver hardware
 */

#pragma once
#include <stdint.h>
#include <stddef.h>

/**
 * \brief             Set CC1101's state to RX or TX
 * \param  tx         true: TX, false: RX
 * \return success
 */
bool cc1101_ook_setDirection(bool tx);

/**
 * \brief             Set GDO pin to fixed high or low potential
 * \param  gdo_num    number of the CC1101's GDO pin
 * \param  high       true: high, false: low
 * \return success
 */
bool cc1101_ook_gdo_hw(int gdo_num, bool high);

/**
 * \brief             check if GDO pin is connected to an MCU GPIO-pin
 * \param  gdo_num    number of the CC1101's GDO pin
 * \param  gpio_num   number of the MCU's GPIO pin
 * \return success    returns TRUE if pin is connected
 */
bool cc1101_ook_gdo_isConnected(int gdo_num, int gpio_num);

/**
 * \brief             get config defaults as pairs terminated by 0xff
 * \param  dst        buffer the config file will be written to
 * \param  dst_size   pointer to length of buffer. If too small, it will contain the required size after return
 * \return success    fails if buffer too small
 */
bool cc1101_ook_get_regfile(uint8_t *dst, size_t *dst_size);

/**
 * \brief             get config registers
 * \param  dst        buffer the config registers will be written to
 * \param  dst_size   length of buffer (at least 48 byte)
 * \return success
 */
bool cc1101_ook_dump_config(uint8_t *dst, size_t *dst_size);

/**
 * \brief             get status registers
 * \param  dst        buffer the status registers will be written to
 * \param  dst_size   length of buffer (at least 14 byte)
 * \return success
 */
bool cc1101_ook_dump_status(uint8_t *dst, size_t *dst_size);


/**
 * \brief        update config registers
 * \param  src   An array of pairs reg_addr,rev_value terminated by 0xff
 * \return success
 */
bool cc1101_ook_updConfig_fromPairs(const uint8_t *src);

/**
 * \brief        update config registers
 * \param  src   An ASCII string (length == 96) containing an Hex byte (ASCII) for each register or "__" to leave the register unchanged.
 * \return success
 */
bool cc1101_ook_updConfig_fromSparse(const char *src);

/**
 * \brief        Get current RSSI value in dBm.
 *
 *  Call it directly after a command was received to get its RSSI.
 */
int cc1101_ook_get_rssi();


