#include "common.h"
#include "codec.h"
#include "utils.h"
#include "fer.h"

//static uint16_t fer_add_word_parity (uint8_t data_byte,int pos);
//static bool fer_word_parity_p (uint16_t word,int pos);


/* "calculate 2bit parity value for DATA_BYTE according to POS" */
static uint8_t
fer_get_word_parity (uint8_t data_byte, uint8_t pos) {
  uint8_t result;

  result = ((pos & 1) != 0) ?
  (byte_even_p (data_byte) ? 3 : 1)
  : (byte_even_p (data_byte) ? 0 : 2);

  return result;
}

/* "extend DATA_BYTE with parity according to POS" */
static uint16_t
fer_add_word_parity (uint8_t data_byte, int pos) {
  uint16_t result= (data_byte | (((uint16_t)fer_get_word_parity(data_byte, pos)) << 8));
  return result;
}

/* "return t if parity is even and postiion matches parity bit \ 1/3
on even positions and 0,2 on odd positions)" */
static bool
fer_word_parity_p (uint16_t word, uint8_t pos) {
  bool result = fer_add_word_parity((word & 0xff), pos) == word;
  return result;
}


uint8_t
fer_make_cmdPacket(const uint8_t *src, uint8_t *dst) {
  int i;
  uint8_t check_sum = 0;

  for (i=0; i < (bytesPerCmdPacket - 1); ++i) {
    dst[i] = src[i];
    check_sum += src[i];
  }

  return dst[i] = check_sum;
}




uint16_t
make_Word(uint8_t val, uint8_t wordCounter)
{
  uint16_t result = fer_add_word_parity(val, wordCounter);
  return result;
}

void
fer_extract_packet(const uint16_t *src, uint8_t *dst) {
  int i;

  for (i=0; i < FER_CMD_WORD_CT; i += 2) {
    uint8_t data_byte = (src[i] & 0xff);
    *dst++ = data_byte;
  }
}

fer_errors
fer_verify_cmdPacket(const uint16_t *dg) {
  int i;
  uint8_t check_sum = 0;

  for (i=0; i < FER_CMD_WORD_CT;) {
    uint8_t data_byte = (dg[i] & 0xff);
    if (data_byte != (dg[i+1]&0xff))
    return fer_PAIR_NOT_EQUAL;
    if (!fer_word_parity_p(dg[i], 0))
    return fer_BAD_WORD_PARITY;
    if (!fer_word_parity_p(dg[i+1], 1))
    return fer_BAD_WORD_PARITY;


    i += 2;
    if (i < FER_CMD_WORD_CT)
    check_sum += data_byte;
    else if (check_sum != data_byte)
    return fer_BAD_CHECKSUM;
  }

  return fer_OK;
}


fer_errors
fer_verify_cmd(const uint8_t *dg) {
  int i;
  uint8_t checksum = 0;

  for (i=0; i < bytesPerCmdPacket - 1; ++i) {
    checksum += dg[i];
  }
  
 return (checksum == dg[i] ? fer_OK : fer_BAD_CHECKSUM);
}


void
fer_extract_prgPacket(const uint16_t *src, uint8_t *dst) {
  int i;

  for (i=0; i < FER_PRG_WORD_CT; i += 2) {
    uint8_t data_byte = (src[i] & 0xff);
    *dst++ = data_byte;
  }
}


void
fer_extract_prgPacket2(const uint16_t *src, uint8_t dst[FER_PRG_BYTE_CT]) {
  int i;

  for (i=0; i < FER_PRG_BYTE_CT; ++i) {
    uint8_t data_byte = (src[i*2] & 0xff);
    dst[i] =  data_byte;
  }
}

fer_errors
fer_extract_Byte(const uint16_t *src, uint8_t *dst)
 {
	int i;
  
  for (i=0; i < 2; ++i) {
    if (fer_word_parity_p(src[i], i)) {
      *dst = src[i];
      return fer_OK;
    }
  }
  *dst = 0;
  return fer_BAD_WORD_PARITY;
  
}




fer_errors
fer_verify_prgPacket(const uint16_t *dg) {
  int i;
  

  for (i=0; i < FER_PRG_WORD_CT; i+=2) {
    uint8_t data_byte = (dg[i] & 0xff);
    if (data_byte != (dg[i+1]&0xff))
    return fer_PAIR_NOT_EQUAL;
    if (!fer_word_parity_p(dg[i], 0))
    return fer_BAD_WORD_PARITY;
    if (!fer_word_parity_p(dg[i+1], 1))
    return fer_BAD_WORD_PARITY;
  }

  return fer_OK;
}
