/**
 * \file   fernotron_trx/raw/fer_msg_plain_enum.h
 * \brief  Raw message for plain commands.
 * \note   Plain messages act also as a header for extended messages.
 * \author bertw
 */

#pragma once


/// \brief indices of data[5] array
enum fer_dat {
  fer_dat_ADDR_2, ///< sender or receiver address byte 2 (contains the type)
  fer_dat_ADDR_1, ///< sender or receiver address byte 1
  fer_dat_ADDR_0, ///< sender or receiver address byte 0
  fer_dat_TGL_and_MEMB, ///< key-press counter (0x1..0xf) + some ID of the sender (like Member number, Type of sender, ...)
  fer_dat_GRP_and_CMD  ///< Group-ID of sender + the command code (0...0xF)
} ;

/// \brief Command Codes
/// \note values of low nibble in data[fer_dat_GRP_and_CMD]
enum fer_cmd {
  fer_cmd_None,
  fer_cmd_x1, ///< ?
  fer_cmd_x2, ///< ?
  fer_cmd_STOP,   ///< stop motor
  fer_cmd_UP,     ///< run motor 'up'
  fer_cmd_DOWN,   ///< run motor 'down'
  fer_cmd_SunDOWN,  ///< run motor 'down' until sun position is reached (works only if sun auto is enabled)
  fer_cmd_SunUP,   ///< run motor 'up' (run motor up if both prior command was SunDOWN and motor position is above or equal sun-position)
  fer_cmd_SunINST,  ///< set current position as sun position
  fer_cmd_EndPosUP, ///< move motor 'up' until 'stop' is sent (the position at stop is saved as end position)
  fer_cmd_EndPosDOWN, ///< move motor 'down' until 'stop' is sent (the position at 'stop' is saved as end position)
  fer_cmd_ToggleRotationDirection, ///< toggles motor rotation direction (used to match up/down commands with real shutter movement)
  fer_cmd_xC, ///< ?
  fer_cmd_SET,   ///< activates set function to add or remove a controller
  fer_cmd_xE, ///< ?
  fer_cmd_Program ///< RTC/TImer data (or Sun-Test if dat_MEMB=fer_memb_SUN)
};

/// \brief Group numbers
/// \note values of high nibble in data[fer_dat_GRP_and_CMD].
typedef enum {
  fer_grp_Broadcast, ///< for all group numbers
  fer_grp_G1,  ///< group number 1
  fer_grp_G2,  ///< group number 2
  fer_grp_G3,  ///< group number 3
  fer_grp_G4,  ///< group number 4
  fer_grp_G5,  ///< group number 5
  fer_grp_G6,  ///< group number 6
  fer_grp_G7,   ///< group number 7
  fer_grp_x8,  ///< ?
  fer_grp_x9,  ///< ?
  fer_grp_xA,  ///< ?
  fer_grp_xB,  ///< ?
  fer_grp_xC,  ///< ?
  fer_grp_xD,  ///< ?
  fer_grp_xE,  ///< ?
  fer_grp_xF,  ///< ?
} fer_grp;

/// \brief Member numbers and device kind
/// \note values of low nibble in data[fer_dat_TGL_and_MEMB].
enum fer_memb{
  fer_memb_Broadcast,                ///< for all member numbers
  fer_memb_FromSunSensor,            ///< originated from a SunSensor
  fer_memb_FromPlainSender,           ///< originated from a plain sender
  fer_memb_x3, ///< ?
  fer_memb_x4, ///< ?
  fer_memb_x5, ///< ?
  fer_memb_x6, ///< ?
  fer_memb_ToReceiver, ///< send to the receiver specified by fer_dat_ADDR
  fer_memb_M1 = 8,  ///< member number 1
  fer_memb_M2,  ///< member number 2
  fer_memb_M3,  ///< member number 3
  fer_memb_M4,  ///< member number 4
  fer_memb_M5,  ///< member number 5
  fer_memb_M6,  ///< member number 6
  fer_memb_M7       ///< member number 7
};


