//-*- C -*-
#pragma once

#include "fer_msg_type.h"
#include "fer_msg_plain_enum.h"

#define FER_CMD_BYTE_CT    6
#define FER_CMD_ADDR_CT    3

#define ENUMBF(type) uint8_t

struct fer_cmd {
  uint8_t addr[FER_CMD_ADDR_CT];

  ENUMBF(fer_memb) memb :4;
  ENUMBF(uint8_t) tgl :4;
  ENUMBF(fer_cmd) cmd :4;
  ENUMBF(fer_grp) grp :4;
};

union fer_cmd_row {
  uint8_t bd[FER_CMD_BYTE_CT];

  struct {
    struct fer_cmd cmd;
    uint8_t checkSum;
  } __attribute__((__packed__)) sd;

};

// high nibble of data[fer_dat_ADDR_2]
////// device type
#define FER_ADDR_TYPE_PlainSender  0x10
#define FER_ADDR_TYPE_SunSensor     0x20
#define FER_ADDR_TYPE_CentralUnit   0x80
#define FER_ADDR_TYPE_Receiver      0x90 // 0x9xxxxx (code written on motor label)

#define FRB_GET_DEVID(data) (((uint32_t)(data[fer_dat_ADDR_2]) << 16) | ((uint16_t)(data[fer_dat_ADDR_1]) << 8) | (data[fer_dat_ADDR_0]))

