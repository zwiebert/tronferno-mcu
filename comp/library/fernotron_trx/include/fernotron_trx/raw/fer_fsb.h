#pragma once

#include <stdint.h>
#include "fer_msg_plain.h"
#include "utils_misc/int_macros.h"

typedef union fer_sbT {
  uint8_t data[5];
  struct Fer_Cmd sd;
} fer_sbT;

#define FER_SB_ADDR_IS_CENTRAL(fsb)  (((fsb)->data[fer_dat_ADDR_2] & 0xf0)  == FER_ADDR_TYPE_CentralUnit)
#define FER_SB_ADDR_IS_SUNSENS(fsb)  (((fsb)->data[fer_dat_ADDR_2] & 0xf0)  == FER_ADDR_TYPE_SunSensor)
#define FER_SB_ADDR_IS_PLAIN(fsb) (((fsb)->data[fer_dat_ADDR_2] & 0xf0) == FER_ADDR_TYPE_PlainSender)
#define FER_SB_IS_BUTTON_HOLD(fsb)    ((fsb)->repeats > 0)

#define FER_SB_PUT_ADDR(fsb, a2, a1, a0) (((fsb)->data[fer_dat_ADDR_2] = (a2)), ((fsb)->data[fer_dat_ADDR_1] = (a1)), ((fsb)->data[fer_dat_ADDR_0] = (a0)))
#define FER_SB_PUT_DEVID(fsb, devID) (((fsb)->data[fer_dat_ADDR_2] = GET_BYTE_2(devID)), ((fsb)->data[fer_dat_ADDR_1] = GET_BYTE_1(devID)), ((fsb)->data[fer_dat_ADDR_0] = GET_BYTE_0(devID)))
#define FER_SB_GET_DEVID(fsb) (((uint32_t)(fsb)->data[fer_dat_ADDR_2] << 16) | (uint16_t)((fsb)->data[fer_dat_ADDR_1] << 8) | ((fsb)->data[fer_dat_ADDR_0]))

#define FER_SB_GET_CMD(fsb)          ((fer_cmd)(fsb)->sd.cmd)
#define FER_SB_PUT_CMD(fsb,val)      ((fsb)->sd.cmd = (fer_cmd)(val))

#define FER_SB_GET_TGL(fsb)          ((fsb)->sd.tgl)
#define FER_SB_PUT_TGL(fsb,val)      ((fsb)->sd.tgl = (val))

#define FER_SB_GET_GRP(fsb)          ((fer_grp)(fsb)->sd.grp)
#define FER_SB_PUT_GRP(fsb,val)      ((fsb)->sd.grp = (val))
inline fer_grp fer_ngrp2fer_grp(int g) { return static_cast<fer_grp>(g); }
#define FER_SB_PUT_NGRP(fsb,val) FER_SB_PUT_GRP(fsb, fer_ngrp2fer_grp(val))

#define FER_SB_GET_MEMB(fsb)         ((fer_memb)(fsb)->sd.memb)
#define FER_SB_PUT_MEMB(fsb,val)     ((fsb)->sd.memb = (val))

inline fer_memb fer_nmb2fer_memb(int m) { return static_cast<fer_memb>(m == 0 ? 0 : m + 7); }
#define FER_SB_PUT_NMEMB(fsb,val) FER_SB_PUT_MEMB(fsb, fer_nmb2fer_memb(val))


#define FER_SB_PUT_DATA(fsb,idx,val) ((fsb)->data[(idx)] = (val))

#define FER_SB_TOGGLE(fsb) fer_update_tglNibble(fsb)

void fer_init_sender(fer_sbT *fsb, uint32_t devID);
void fer_init_plain(fer_sbT *fsb, uint8_t a2, uint8_t a1, uint8_t a0);
void fer_init_sunSensor(fer_sbT *fsb, uint8_t a2, uint8_t a1, uint8_t a0);
void fer_update_tglNibble(fer_sbT *fsb);

// like FER_SB_ but works on array instead of struct
#define FER_RB_GET_CMD(data)          (GET_LOW_NIBBLE((data)[fer_dat_GRP_and_CMD]))
#define FER_RB_GET_MEMB(data)         (GET_LOW_NIBBLE((data)[fer_dat_TGL_and_MEMB]))
#define FER_RB_GET_GRP(data)          (GET_HIGH_NIBBLE((data)[fer_dat_GRP_and_CMD]))
#define FER_RB_ADDR_IS_CENTRAL(data)  (((data)[fer_dat_ADDR_2] & 0xf0)  == FER_ADDR_TYPE_CentralUnit)

#define FER_RB_GET_FPR0_IS_RTC_ONLY(data) (((data)[fpr0_RTC_wday] & 0x80) != 0) //TODO:move

