//-*- C -*-
#pragma once

#include "fer_msg_type.h"
#include "fer_msg_plain_enum.h"

#define FER_CMD_BYTE_CT    6
#define FER_CMD_ADDR_CT    3

#ifdef __cplusplus
#define ENUMBF(type) type
#else
#define ENUMBF(type) uint16_t
#endif
struct __attribute__ ((packed)) Fer_Cmd  {
  uint8_t addr[FER_CMD_ADDR_CT];

  ENUMBF(fer_memb) memb :4;
  ENUMBF(uint8_t) tgl :4;
  ENUMBF(fer_cmd) cmd :4;
  ENUMBF(fer_grp) grp :4;
};



union fer_cmd_row {
  uint8_t bd[FER_CMD_BYTE_CT];

  struct __attribute__((__packed__)) {
    struct Fer_Cmd cmd;
    uint8_t checkSum;
  } sd;

};

#ifdef __cplusplus
static_assert(sizeof(Fer_Cmd) == FER_CMD_BYTE_CT - 1, "wrong msg size");
static_assert(sizeof(fer_cmd_row) == FER_CMD_BYTE_CT, "wrong msg size");
#endif

// high nibble of data[fer_dat_ADDR_2]
////// device type
#define FER_ADDR_TYPE_PlainSender  0x10
#define FER_ADDR_TYPE_SunSensor     0x20
#define FER_ADDR_TYPE_CentralUnit   0x80
#define FER_ADDR_TYPE_Receiver      0x90 // 0x9xxxxx (code written on motor label)

#define FER_RB_GET_DEVID(data) (((uint32_t)(data[fer_dat_ADDR_2]) << 16) | ((uint16_t)(data[fer_dat_ADDR_1]) << 8) | (data[fer_dat_ADDR_0]))

#define FER_U32_TEST_TYPE(u32_a, t) (((u32_a) >> 16) == (t))

