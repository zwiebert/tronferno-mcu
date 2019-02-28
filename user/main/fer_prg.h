/*
 * fer_prg.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_PRG_H_
#define USER_MAIN_FER_PRG_H_

void fmsg_create_checksums(struct fer_msg *m, fmsg_type t);
bool fmsg_verify_checksums(const struct fer_msg *m, fmsg_type t);
void fmsg_init_data(fer_msg *msg);
void fmsg_write_rtc(fer_msg *msg, time_t rtc, bool rtc_only);
void fmsg_write_flags(fer_msg *msg, uint8_t flags, uint8_t mask);
void fmsg_write_wtimer(fer_msg *msg, const uint8_t *wtimer_data);
void fmsg_write_dtimer(fer_msg *msg, const uint8_t *dtimer_data);
void fmsg_write_astro(fer_msg *msg, int mint_offset);
void fmsg_write_lastline(fer_msg *msg, fer_sender_basic *fsb);

#endif /* USER_MAIN_FER_PRG_H_ */
