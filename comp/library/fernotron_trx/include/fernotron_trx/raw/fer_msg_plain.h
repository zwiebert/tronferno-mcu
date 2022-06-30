/**
 * \file   fernotron_trx/raw/fer_msg_plain.h
 * \brief  Data structure of plain messages or header of extended messages
 */
#pragma once

#include "fer_msg_type.h"
#include "fer_msg_plain_enum.h"
#include <fernotron_trx/fer_trx_c_api.h>
#include "utils_misc/int_macros.h"

#define FER_CMD_BYTE_CT    6   ///<  Message byte count (with checksum)
#define FER_CMD_ADDR_CT    3   ///<  Device address byte count


/**
 * \brief   Plain message or header for extended messages (w/o checksum)
 */
struct __attribute__ ((packed)) Fer_Cmd  {
  uint8_t addr[FER_CMD_ADDR_CT];  ///< Device address
  fer_memb memb :4;  ///< /link fer_memb /endlink
  uint8_t tgl :4;  ///< kind of a message ID. Incremented for each unique message sent
  fer_cmd cmd :4;  ///< /link fer_cmd /endlink
  fer_grp grp :4;  ///< /link fer_grp /endlink
};


/**
 * \brief  Plain message or header for extended messages (with checksum)
 */
union fer_cmd_row {
  uint8_t bd[FER_CMD_BYTE_CT];  ///< Message object as byte array

  struct __attribute__((__packed__)) {
    struct Fer_Cmd cmd;         ///< Message object w/o checksum
    uint8_t checkSum;           ///< sum of all previous bytes in message
  } sd;                         ///< Structured message object

};

#ifdef __cplusplus
static_assert(sizeof(Fer_Cmd) == FER_CMD_BYTE_CT - 1, "wrong msg size");
static_assert(sizeof(fer_cmd_row) == FER_CMD_BYTE_CT, "wrong msg size");
#endif

//
/// \brief device type (\link fer_dat_ADDR_2 \endlink)
enum Fer_DevType : uint8_t {
  FER_PlainSender = 0x10,   ///< type byte of plain sender addresses
  FER_SunSensor = 0x20,   ///< type byte of  sun-sensor addresses
  FER_CentralUnit = 0x80,   ///< type byte of  central-unit addresses
  FER_Receiver = 0x90,   ///< type byte of receiver/motor addresses (written on motor label w/o the leading 9)
};

/**
 * \brief       Match type of device address
 * \param a     device address
 * \param t     type
 * \return      true for match
 */
inline bool FER_U32_TEST_TYPE(uint32_t a, Fer_DevType t) {
  return (a >> 16) == t;
}

/// \brief Test if address matches a central unit
inline bool FER_CMD_ADDR_IS_CENTRAL(const struct Fer_Cmd *cmd) {
  return ((cmd->addr[fer_dat_ADDR_2] & 0xf0) == (uint8_t)FER_CentralUnit);
}



/////////////////////


/// \brief Test if address matches a central unit
inline bool FER_RB_ADDR_IS_CENTRAL(const struct Fer_Cmd *cmd) {
  return ((cmd->addr[fer_dat_ADDR_2] & 0xf0) == FER_ADDR_TYPE_CentralUnit);
}
/// \brief Test if address matches a sun sensor
inline bool FER_RB_ADDR_IS_SUNSENS(const struct Fer_Cmd *cmd) {
  return ((cmd->addr[fer_dat_ADDR_2] & 0xf0) == FER_ADDR_TYPE_SunSensor);
}
/// \brief Test if address matches a plain sender
inline bool FER_RB_ADDR_IS_PLAIN(const struct Fer_Cmd *cmd) {
  return ((cmd->addr[fer_dat_ADDR_2] & 0xf0) == FER_ADDR_TYPE_PlainSender);
}
/**
 * \brief        Put device address into message
 * \param devID  Address as number
 */
inline void FER_RB_PUT_DEVID(struct Fer_Cmd *cmd, uint32_t devID) {
  (cmd->addr[fer_dat_ADDR_2] = GET_BYTE_2(devID));
  (cmd->addr[fer_dat_ADDR_1] = GET_BYTE_1(devID));
  (cmd->addr[fer_dat_ADDR_0] = GET_BYTE_0(devID));
}
/// \brief Get device address as number from message
inline uint32_t FER_RB_GET_DEVID(const struct Fer_Cmd *cmd) {
  return (((uint32_t) cmd->addr[fer_dat_ADDR_2] << 16) | (uint16_t) (cmd->addr[fer_dat_ADDR_1] << 8) | (cmd->addr[fer_dat_ADDR_0]));
}

/// \brief Get cmd from message
inline fer_cmd FER_RB_GET_CMD(const struct Fer_Cmd *cmd) {
  return cmd->cmd;
}
/// \brief Put cmd into message
inline void FER_RB_PUT_CMD(struct Fer_Cmd *cmd, fer_cmd val) {
  (cmd->cmd = (fer_cmd) (val));
}

/// \brief Get tgl field from message
inline uint8_t FER_RB_GET_TGL(const struct Fer_Cmd *cmd) {
  return (cmd->tgl);
}
/// \brief Put tgl field into message
inline void FER_RB_PUT_TGL(struct Fer_Cmd *cmd, uint8_t val) {
  (cmd->tgl = (val));
}
/// \brief Get grp field from message
inline fer_grp FER_RB_GET_GRP(const struct Fer_Cmd *cmd) {
  return ((fer_grp) cmd->grp);
}
/// \brief Put grp field into message
inline void FER_RB_PUT_GRP(struct Fer_Cmd *cmd, fer_grp val) {
  (cmd->grp = (val));
}
/// \brief Put group number into message
inline void FER_RB_PUT_NGRP(struct Fer_Cmd *cmd, uint8_t val) {
  FER_RB_PUT_GRP(cmd, static_cast<fer_grp>(val));
}

/// \brief Get memb field from message
inline fer_memb FER_RB_GET_MEMB(const struct Fer_Cmd *cmd) {
  return ((fer_memb) cmd->memb);
}
/// \brief Get memb field from message
inline uint8_t FER_RB_GET_NMEMB(const struct Fer_Cmd *cmd) {
  uint8_t memb = cmd->memb;
  if (memb > 0)
    memb -= 7;
  return memb;
}
/// \brief Put memb field into message
inline void FER_RB_PUT_MEMB(struct Fer_Cmd *cmd, fer_memb val) {
  (cmd->memb = (val));
}
/// \brief Put member number into message
inline void FER_RB_PUT_NMEMB(struct Fer_Cmd *cmd, uint8_t m) {
  cmd->memb =  static_cast<fer_memb>(m == 0 ? 0 : m + 7);
}




