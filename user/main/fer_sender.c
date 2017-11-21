#include "common.h"
#include "fer.h"
#include "counter.h"
#include "inout.h"
#include "utils.h"
#include "config.h"

volatile bool is_sendMsgPending;
volatile uint16_t wordsToSend;


uint8_t ICACHE_FLASH_ATTR fer_make_cmdPacket(const uint8_t *src, uint8_t *dst) {
  int i;
  uint8_t check_sum = 0;

  for (i=0; i < (bytesPerCmdPacket - 1); ++i) {
    dst[i] = src[i];
    check_sum += src[i];
  }

  return dst[i] = check_sum;
}


bool ICACHE_FLASH_ATTR fer_send_cmd(fer_sender_basic *fsb) {
	if (is_sendMsgPending)
		return false;
	if (recv_lockBuffer(true)) {
	fer_make_cmdPacket(fsb->data, txmsg->cmd);
	if (C.app_verboseOutput >= vrb1)
		io_puts("S:"), frb_printPacket(txmsg->cmd);
	wordsToSend = 2 * BYTES_MSG_PLAIN;

	is_sendMsgPending = true;

	recv_lockBuffer(false);
	return true;
	} else {
		return false;
	}
}


bool ICACHE_FLASH_ATTR fer_send_prg(fer_sender_basic *fsb) {
	if (is_sendMsgPending)
		return false;

	uint8_t cmd_checksum = fer_make_cmdPacket(fsb->data, txmsg->cmd);
	fer_prg_create_checksums(getMsgData(txmsg), cmd_checksum);

if (C.app_verboseOutput >= vrb1)
	io_puts("S:"), fer_printData(txmsg->cmd, C.app_verboseOutput >= vrb2 ? getMsgData(txmsg) : 0);

	wordsToSend = 2 * (FRB_GET_FPR0_IS_RTC_ONLY(txmsg->rtc) ? BYTES_MSG_RTC : BYTES_MSG_TIMER);
	is_sendMsgPending = true;
	return true;
}


