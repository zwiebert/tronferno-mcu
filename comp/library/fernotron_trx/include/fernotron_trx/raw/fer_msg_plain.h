/**
 * \file   fernotron_trx/raw/fer_msg_plain.h
 * \brief  Data structure of plain messages or header of extended messages
 */
#pragma once

#include "fer_msg_type.h"
#include "fer_msg_plain_enum.h"

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
 *  \brief     convert byte array device address to integer
 *  \param ba  address as byte array
 *  \return    address as uint32_t
 */
#define FER_RB_GET_DEVID(data) (((uint32_t)(data[fer_dat_ADDR_2]) << 16) | ((uint16_t)(data[fer_dat_ADDR_1]) << 8) | (data[fer_dat_ADDR_0]))

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
inline bool FER_CMD_ADDR_IS_CENTRAL(const Fer_Cmd *cmd) {
  return ((cmd->addr[fer_dat_ADDR_2] & 0xf0) == (uint8_t)FER_CentralUnit);
}
