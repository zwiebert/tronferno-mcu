//-*- C -*-
#ifndef _fer_code
#define _fer_code

#include "common.h"
#include "boolean.h"

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
	fer_cmd_SunUP,   // run motor 'up' (should work only after SunDOWN (or if not below sun pos?))
	fer_cmd_SunINST,  // set current position als sun position
	fer_cmd_EndPosUP, // move motor 'up' until 'stop' is sent (the position at stop is saved as end position)
	fer_cmd_EndPosDOWN, // move motor 'down' until 'stop' is sent (the position at 'stop' is saved as end position)
	fer_cmd_ChangeRotationDirection, // toggles motor rotation direction (used to match up/down commands with real shutter movement)
	fer_cmd_0xc,
	fer_cmd_SET,   // atcivates set function to add or remove a controller
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

// high nibble of data[fer_dat_ADDR_2]
////// device type
#define FER_ADDR_TYPE_PlainSender  0x10
#define FER_ADDR_TYPE_SunSensor     0x20
#define FER_ADDR_TYPE_CentralUnit   0x80
#define FER_ADDR_TYPE_Receiver      0x90 // 0x9xxxxx (code written on motor label)

#define FRB_GET_DEVID(data) (((uint32_t)(data[fer_dat_ADDR_2]) << 16) | ((uint16_t)(data[fer_dat_ADDR_1]) << 8) | (data[fer_dat_ADDR_0]))

typedef struct {
	uint8_t data[5];
	int8_t repeats;
} fer_sender_basic;

#define FSB_ADDR_IS_CENTRAL(fsb)  (((fsb)->data[fer_dat_ADDR_2] & 0xf0)  == FER_ADDR_TYPE_CentralUnit)
#define FSB_ADDR_IS_SUNSENS(fsb)  (((fsb)->data[fer_dat_ADDR_2] & 0xf0)  == FER_ADDR_TYPE_SunSensor)
#define FSB_ADDR_IS_PLAIN(fsb) (((fsb)->data[fer_dat_ADDR_2] & 0xf0) == FER_ADDR_TYPE_PlainSender)
#define FSB_IS_BUTTON_HOLD(fsb)    ((fsb)->repeats > 0) 

#define FSB_PUT_ADDR(fsb, a2, a1, a0) (((fsb)->data[fer_dat_ADDR_2] = (a2)), ((fsb)->data[fer_dat_ADDR_1] = (a1)), ((fsb)->data[fer_dat_ADDR_0] = (a0)))
#define FSB_PUT_DEVID(fsb, devID) (((fsb)->data[fer_dat_ADDR_2] = GET_BYTE_2(devID)), ((fsb)->data[fer_dat_ADDR_1] = GET_BYTE_1(devID)), ((fsb)->data[fer_dat_ADDR_0] = GET_BYTE_0(devID)))
#define FSB_GET_DEVID(fsb) (((uint32_t)(fsb)->data[fer_dat_ADDR_2] << 16) | (uint16_t)((fsb)->data[fer_dat_ADDR_1] << 8) | ((fsb)->data[fer_dat_ADDR_0]))



#define FSB_GET_CMD(fsb)          (GET_LOW_NIBBLE((fsb)->data[fer_dat_GRP_and_CMD]))
#define FSB_PUT_CMD(fsb,val)      (PUT_LOW_NIBBLE((fsb)->data[fer_dat_GRP_and_CMD], val))

#define FSB_GET_TGL(fsb)          (GET_HIGH_NIBBLE((fsb)->data[fer_dat_TGL_and_MEMB]))
#define FSB_PUT_TGL(fsb,val)      (PUT_HIGH_NIBBLE((fsb)->data[fer_dat_TGL_and_MEMB], val))

#define FSB_GET_GRP(fsb)          (GET_HIGH_NIBBLE((fsb)->data[fer_dat_GRP_and_CMD]))
#define FSB_PUT_GRP(fsb,val)      (PUT_HIGH_NIBBLE((fsb)->data[fer_dat_GRP_and_CMD], val))

#define FSB_GET_MEMB(fsb)         (GET_LOW_NIBBLE((fsb)->data[fer_dat_TGL_and_MEMB]))
#define FSB_PUT_MEMB(fsb,val)     (PUT_LOW_NIBBLE((fsb)->data[fer_dat_TGL_and_MEMB], val))

#define FSB_PUT_DATA(fsb,idx,val)     ((fsb)->data[(idx)] = (val))

#define FSB_TOGGLE(fsb) fer_update_tglNibble(fsb)

////////////// programming frame /////////////////////
//18 rows each 9 columns

/// entire frame
enum fpr0 {
	fpr0_RTC_secs, fpr0_RTC_mint, fpr0_RTC_hour,  // RTC time
  fpr0_RTC_wdayMask, // Sun...Sat = 0x01 ... 0x40 (each bit represents a weekday)
	fpr0_RTC_days,  // day of month 1..31
	fpr0_RTC_mont,  // month of year 1..12
	fpr0_RTC_wday, // low nibble: Mon...Sun = 1...7.  high nibble is 0x0 (full program frame) or 0x8 (RTC only short program frame)
	fpr0_FlagBits, // bits to enable SunAutomatic, Random, DST
	fpr0_checksum
};

// values of fpr0_FlagBits.
enum fpr0_FlagBitsValues { // FIXME: there may be more flag bits in use. try to find out
	flag_Random, // shutter opens/closes at random times (theft protection during longer absence). (FIXME: ignored ???)
	flag_1,  // ???
	flag_DST,  // daylight saving time. if set, the shutter will add 1 hour to its saved astro time table. (never ignored)
	flag_3,  //
	flag_4,  // ???
	flag_5,  // ???
	flag_6,  // ???
	flag_SunAuto  // automatic sunsensor. if off the sun-down command does not work. (ignored at rtc-only frames)
};

// frames 1 - 4: timer  
enum fpr1 {
	fpr1_T_sun_up_mint,
	fpr1_T_sun_up_hour,
	fpr1_T_sun_down_mint,
	fpr1_T_sun_down_hour,
	fpr1_T_mon_up_mint,
	fpr1_T_mon_up_hour,
	fpr1_T_mon_down_mint,
	fpr1_T_mon_down_hour,
	fpr1_checksum
};

enum fpr2 {
	fpr2_T_tue_up_mint,
	fpr2_T_tue_up_hour,
	fpr2_T_tue_down_mint,
	fpr2_T_tue_down_hour,
	fpr2_T_wed_up_mint,
	fpr2_T_wed_up_hour,
	fpr2_T_wed_down_mint,
	fpr2_T_wed_down_hour,
	fpr2_checksum
};

enum fpr3 {
	fpr3_T_thu_up_mint,
	fpr3_T_thu_up_hour,
	fpr3_T_thu_down_mint,
	fpr3_T_thu_down_hour,
	fpr3_T_fri_up_mint,
	fpr3_T_fri_up_hour,
	fpr3_T_fri_down_mint,
	fpr3_T_fri_down_hour,
	fpr3_checksum
};

enum fpr4 {
	fpr4_T_sat_up_mint,
	fpr4_T_sat_up_hour,
	fpr4_T_sat_down_mint,
	fpr4_T_sat_down_hour,
	fpr4_T_daily_up_mint,
	fpr4_T_daily_up_hour,
	fpr4_T_daily_down_mint,
	fpr4_T_daily_down_hour,
	fpr4_checksum
};

//////////////////////////////////////////////////////////////////////////

// frames 5 - 16: astro

//////////////////////////////////////////////////////////////////////////

// frame 17

enum fpr17 {
	fpr17_0,
	fpr17_Addr_2,
	fpr17_Addr_1,
	fpr17_Addr_0,
	fpr17_4,
	fpr17_5,
	fpr17_6,
	fpr17_7,
	fpr17_8
};

//////////////////////////////////////////////////////////////////////////

// sub frames

// timer sub frame. There are 8 timer stamps (Up/Down).  7 are for weekly timer (one timer for each weekday) and 1 is for daily timer (for every day).
enum fpr_tim {
	fpr_tim_sun,
	fpr_tim_mon,
	fpr_tim_tue,
	fpr_tim_wed,
	fpr_tim_thu,
	fpr_tim_fri,
	fpr_tim_sat,
	fpr_tim_daily
};

// values of fpr0_RTC_wdayMask
enum wdays {
	wd_SUN, wd_MON, wd_TUE, wd_WED, wd_THU, wd_FRI, wd_SAT
};
// weekday by bitmask in prg[0][fpr0_RTC_wdayMask]
// values of low nibble of fpr0_RTC_wday
enum wday2 {
	wd2_MON = 1, wd2_TUE, wd2_WED, wd2_THU, wd2_FRI, wd2_SAT, wd2_SUN,
};
// weekday by number in prg[0][fpr0_RTC_wday]
// values of high nibble of fpr0_RTC_wday (bit flags?)
#define FLAG_TIMER_DATA 0x8

extern uint8_t bcd2dec(uint8_t bcd);

#define FPR_RTC_START_ROW  0
#define FPR_RTC_START_COL  0
#define FPR_RTC_WIDTH      8 // nmb of bytes used for RTC data
#define FPR_RTC_HEIGHT     1 // nmb of rows used for RTC data
#define FPR_RTC_BYTE_CT    3 // nmb of bytes in data (6 for SS:MM:HH)

#define FPR_ASTRO_START_ROW   5 // astro data start at this row in frame
#define FPR_ASTRO_WIDTH       8 // nmb of bytes used for astro data each row (8 or 9???)
#define FPR_ASTRO_HEIGHT     12 // nmb of rows used for astro data

#define FPR_TIMER_START_ROW     1 // timer data start at this row in frame
#define FPR_TIMER_WIDTH         8 // nmb of bytes used for timer data each row (8 or 9???)
#define FPR_TIMER_STAMP_WIDTH   4 // nmb of bytes used for a time stamp (4 bytes for up MM:HH down MM:HH)
#define FPR_TIMER_HEIGHT        4 // nmb of rows used for timer data

#define FPR_WEEKLY_START_ROW  1
#define FPR_WEEKLY_START_COL  0
#define FPR_WEEKLY_TIMER_STAMP_CT 7 // nmb of time stamps in data

#define FPR_DAILY_START_ROW  4
#define FPR_DAILY_START_COL  4
#define FPR_DAILY_TIMER_STAMP_CT 1 // nmb of time stamps in data

#define FPR_LAST_START_ROW 17
#define FPR_LAST_HEIGHT 1

#define FPR_DATA_HEIGHT 18
#define FPR_DATA_WIDTH 8
#define FPR_CS_WIDTH 1

#define fpr2dec(fpr_nmb) (bcd2dec(0xff - (fpr_nmb)))

#define FPR_GET_LOW_NIBBLE(d,row,col)  (0xF & GET_LOW_NIBBLE( (d)[(row)][(col)]))
#define FPR_GET_HIGH_NIBBLE(d,row,col) (0xF & GET_HIGH_NIBBLE((d)[(row)][(col)]))

#define FPR_RTC_GET_SECS_L(d) (FPR_GET_LOW_NIBBLE( d, 0, fprt0_RTC_secs))
#define FPR_RTC_GET_SECS_H(d) (FPR_GET_HIGH_NIBBLE(d, 0, fprt0_RTC_secs))
#define FPR_RTC_GET_SECS(d)   (FPR_RTC_GET_SECS_H(d)*10 + FPR_RTC_GET_SECS_L(d))

#define FPR_RTC_GET_MINT_L(d) (0xf - GET_LOW_NIBBLE( (d)[0][fprt0_RTC_mint]))
#define FPR_RTC_GET_MINT_H(d) (0xf - GET_HIGH_NIBBLE((d)[0][fprt0_RTC_mint]))
#define FPR_RTC_GET_MINT(d) (FPR_RTC_GET_MINT_H(d)*10 + FPR_RTC_GET_MINT_L(d))

#define FPR_RTC_GET_HOUR_L(d) (0xf - GET_LOW_NIBBLE( (d)[0][fprt0_RTC_hour]))
#define FPR_RTC_GET_HOUR_H(d) (0xf - GET_HIGH_NIBBLE((d)[0][fprt0_RTC_hour]))
#define FPR_RTC_GET_HOUR(d) (FPR_RTC_GET_HOUR_H(d)*10 + FPR_RTC_GET_HOUR_L(d))

void fer_init_sender(fer_sender_basic *fsb, uint32_t devID);
void fer_init_plain(fer_sender_basic *fsb, uint8_t a2, uint8_t a1, uint8_t a0);
void fer_init_sunSensor(fer_sender_basic *fsb, uint8_t a2, uint8_t a1,
		uint8_t a0);
void fer_update_tglNibble(fer_sender_basic *fsb);

// like macros above but extract from raw send-data array (2 words per byte with parity)
#define FRW_GET_CMD(data)          (GET_LOW_NIBBLE((data)[fer_dat_GRP_and_CMD * 2]))
#define FRW_GET_MEMB(data)         (GET_LOW_NIBBLE((data)[fer_dat_TGL_and_MEMB * 2]))
#define FRW_GET_GRP(data)          (GET_HIGH_NIBBLE((data)[fer_dat_GRP_and_CMD * 2]))
#define FRW_MODEL_IS_CENTRAL(data)  (((data)[fer_dat_ADDR_2 * 2] & 0xff)  == FER_ADDR_TYPE_CentralUnit)
#define FRW_GET_FPR0_IS_RTC_ONLY(data) ((GET_HIGH_NIBBLE((data)[fpr0_RTC_wday * 2]) & 0x8) != 0)

#define FRB_GET_CMD(data)          (GET_LOW_NIBBLE((data)[fer_dat_GRP_and_CMD]))
#define FRB_GET_MEMB(data)         (GET_LOW_NIBBLE((data)[fer_dat_TGL_and_MEMB]))
#define FRB_GET_GRP(data)          (GET_HIGH_NIBBLE((data)[fer_dat_GRP_and_CMD]))
#define FRB_ADDR_IS_CENTRAL(data)  (((data)[fer_dat_ADDR_2] & 0xf0)  == FER_ADDR_TYPE_CentralUnit)
#define FRB_GET_FPR0_IS_RTC_ONLY(data) (((data)[fpr0_RTC_wday] & 0x80) != 0)

#endif //_fer_code
