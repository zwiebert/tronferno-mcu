/*
 * fer_print.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_PRINT_H_
#define USER_MAIN_FER_PRINT_H_

void fmsg_print(const fer_msg *msg, fmsg_type t);
void fmsg_print_as_cmdline(const fer_msg *msg, fmsg_type t);
// diagnostic output
void frb_printPacket(const u8 *dg);
void fpr_printPrgPacketInfo(u8 d[linesPerPrg][bytesPerPrgLine], bool rtc_only);
void fer_printData(const u8 *cmd, u8 prg[linesPerPrg][bytesPerPrgLine]);


#endif /* USER_MAIN_FER_PRINT_H_ */
