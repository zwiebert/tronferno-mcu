//-*- C -*-
#ifndef _fer_code
#define _fer_code

#include "fer_msg.h"

// indexes of data[5]
typedef enum {
	fer_dat_ADDR_2, fer_dat_ADDR_1, fer_dat_ADDR_0, // sender or receiver address
	fer_dat_TGL_and_MEMB, // key-press counter (0x1..0xf) + some ID of the sender (like Member number, Type of sender, ...)
	fer_dat_GRP_and_CMD  // Group-ID of sender + the command code (0...0xF)
} fer_dat;

// values of low nibble in data[fer_dat_GRP_and_CMD]. 
/////// Command Codes
typedef enum {
	fer_cmd_None,
	fer_cmd_1,
	fer_cmd_2,
	fer_cmd_STOP,   // stop motor
	fer_cmd_UP,     // run motor 'up'
	fer_cmd_DOWN,   // run motor 'down'
	fer_cmd_SunDOWN,  // run motor 'down' until sun position is reached (works only if sun auto is enabled)
	fer_cmd_SunUP,   // run motor 'up' (run motor up if both prior command was SunDOWN and motor position is above or equal sun-position)
	fer_cmd_SunINST,  // set current position as sun position
	fer_cmd_EndPosUP, // move motor 'up' until 'stop' is sent (the position at stop is saved as end position)
	fer_cmd_EndPosDOWN, // move motor 'down' until 'stop' is sent (the position at 'stop' is saved as end position)
	fer_cmd_ChangeRotationDirection, // toggles motor rotation direction (used to match up/down commands with real shutter movement)
	fer_cmd_0xc,
	fer_cmd_SET,   // activates set function to add or remove a controller
	fer_cmd_0xe,
	fer_cmd_Program // RTC/TImer data (or Sun-Test if dat_MEMB=fer_memb_SUN)
} fer_cmd;

// values of high nibble in data[fer_dat_GRP_and_CMD].
/////// Sender IDs
typedef enum {
	fer_grp_Broadcast, // for all group numbers
	fer_grp_G1,  // group number 1
	fer_grp_G2,
	fer_grp_G3,
	fer_grp_G4,
	fer_grp_G5,
	fer_grp_G6,
	fer_grp_G7   // group number 7
/* FIXME: only 3 bits used so far. Is the highest bit used for anything? */

} fer_grp;

// values of low nibble in data[fer_dat_TGL_and_MEMB].
/////// Sender IDs
typedef enum {
	fer_memb_Broadcast,                // for all member numbers
	fer_memb_FromSunSensor,            // originated from a SunSensor
	fer_memb_FromPlainSender,           // originated from a plain sender
	fer_memb_P3,
	fer_memb_P4,
	fer_memb_P5,
	fer_memb_P6,
	fer_memb_ToReceiver, // send to the receiver specified by fer_dat_ADDR
	fer_memb_M1 = 8,  // member number 1
	fer_memb_M2,
	fer_memb_M3,
	fer_memb_M4,
	fer_memb_M5,
	fer_memb_M6,
	fer_memb_M7       // member number 7
} fer_memb;

#ifdef XX__GNUC__
#define ENUMBF(type)
__extension__ type
#else
#define ENUMBF(type) uint8_t
#endif

struct fer_cmd {
  uint8_t addr[3];

  ENUMBF(fer_memb) memb :4;
  uint8_t tgl :4;

  ENUMBF(fer_cmd) cmd :4;
  ENUMBF(fer_grp) grp :4;
};

union fer_cmd_row {
  uint8_t bd[6];

  struct {
    struct fer_cmd  cmd;
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




#endif //_fer_code
