/**
 * \file  fernotron_trx/raw/fer_fsb.h
 * \brief Plain raw message
 */

#pragma once

#include <stdint.h>
#include "fer_msg_plain.h"
#include "utils_misc/int_macros.h"
#include <fernotron_trx/fer_trx_c_api.h>

/**
 * \brief Raw message w/o checksum
 */
typedef union fer_sbT {
  uint8_t data[5]; ///< byte access
  struct Fer_Cmd sd; ///< field access
} fer_sbT;

/// \brief Test if address matches a central unit
inline bool FER_SB_ADDR_IS_CENTRAL(const fer_sbT *fsb) {
  return (((fsb)->data[fer_dat_ADDR_2] & 0xf0) == FER_ADDR_TYPE_CentralUnit);
}
/// \brief Test if address matches a sun sensor
inline bool FER_SB_ADDR_IS_SUNSENS(const fer_sbT *fsb) {
  return (((fsb)->data[fer_dat_ADDR_2] & 0xf0) == FER_ADDR_TYPE_SunSensor);
}
/// \brief Test if address matches a plain sender
inline bool FER_SB_ADDR_IS_PLAIN(const fer_sbT *fsb) {
  return (((fsb)->data[fer_dat_ADDR_2] & 0xf0) == FER_ADDR_TYPE_PlainSender);
}
/**
 * \brief        Put device address into message
 * \param devID  Address as number
 */
inline void FER_SB_PUT_DEVID(fer_sbT *fsb, uint32_t devID) {
  ((fsb)->data[fer_dat_ADDR_2] = GET_BYTE_2(devID));
  ((fsb)->data[fer_dat_ADDR_1] = GET_BYTE_1(devID));
  ((fsb)->data[fer_dat_ADDR_0] = GET_BYTE_0(devID));
}
/// \brief Get device address as number from message
inline uint32_t FER_SB_GET_DEVID(const fer_sbT *fsb) {
  return (((uint32_t) (fsb)->data[fer_dat_ADDR_2] << 16) | (uint16_t) ((fsb)->data[fer_dat_ADDR_1] << 8) | ((fsb)->data[fer_dat_ADDR_0]));
}

/// \brief Get cmd from message
inline fer_cmd FER_SB_GET_CMD(const fer_sbT *fsb) {
  return (fsb)->sd.cmd;
}
/// \brief Put cmd into message
inline void FER_SB_PUT_CMD(fer_sbT *fsb, fer_cmd val) {
  ((fsb)->sd.cmd = (fer_cmd) (val));
}
/// \brief Put cmd into message (Using the API-cmd-type)
inline void FER_SB_PUT_CMD(fer_sbT *fsb, fer_if_cmd val) {
  ((fsb)->sd.cmd = (fer_cmd) (val));
}

/// \brief Get tgl field from message
inline uint8_t FER_SB_GET_TGL(const fer_sbT *fsb) {
  return ((fsb)->sd.tgl);
}
/// \brief Put tgl field into message
inline void FER_SB_PUT_TGL(fer_sbT *fsb, uint8_t val) {
  ((fsb)->sd.tgl = (val));
}
/// \brief Get grp field from message
inline fer_grp FER_SB_GET_GRP(const fer_sbT *fsb) {
  return ((fer_grp) (fsb)->sd.grp);
}
/// \brief Put grp field into message
inline void FER_SB_PUT_GRP(fer_sbT *fsb, fer_grp val) {
  ((fsb)->sd.grp = (val));
}
/// \brief Put group number into message
inline void FER_SB_PUT_NGRP(fer_sbT *fsb, uint8_t val) {
  FER_SB_PUT_GRP(fsb, static_cast<fer_grp>(val));
}

/// \brief Get memb field from message
inline fer_memb FER_SB_GET_MEMB(const fer_sbT *fsb) {
  return ((fer_memb) (fsb)->sd.memb);
}
/// \brief Put memb field into message
inline void FER_SB_PUT_MEMB(fer_sbT *fsb, fer_memb val) {
  ((fsb)->sd.memb = (val));
}
/// \brief Put member number into message
inline void FER_SB_PUT_NMEMB(fer_sbT *fsb, uint8_t m) {
  fsb->sd.memb =  static_cast<fer_memb>(m == 0 ? 0 : m + 7);
}

/// \brief Put byte data into message
inline void FER_SB_PUT_DATA(fer_sbT *fsb, int idx, uint8_t val) {
  ((fsb)->data[(idx)] = (val));
}

///  \brief  Initialize message according to device address
void fer_init_sender(fer_sbT *fsb, uint32_t devID);


/// \brief  Increment tgl nibble in message
void fer_update_tglNibble(fer_sbT *fsb);

/**
 * \brief    Construct an fer_sbT
 */
fer_sbT fer_construct_fsb(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd);



