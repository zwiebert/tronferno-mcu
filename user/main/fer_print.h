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
void frb_printPacket(const uint8_t *dg);
void fpr_printPrgPacketInfo(uint8_t d[linesPerPrg][bytesPerPrgLine], bool rtc_only);
void fer_printData(const uint8_t *cmd, uint8_t prg[linesPerPrg][bytesPerPrgLine]);


#endif /* USER_MAIN_FER_PRINT_H_ */
