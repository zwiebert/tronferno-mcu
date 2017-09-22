#ifndef CODEC_H_
#define CODEC_H_

uint8_t fer_make_cmdPacket (const uint8_t *src, uint8_t *dst);
void fer_make_prgPacket (const uint8_t* src, uint8_t* dst);
uint16_t make_Word(uint8_t val, uint8_t wordCounter);

typedef enum { fer_OK, fer_PAIR_NOT_EQUAL, fer_BAD_WORD_PARITY, fer_BAD_CHECKSUM } fer_errors;
fer_errors fer_verify_cmdPacket (const uint16_t* dg);
fer_errors fer_verify_cmd(const uint8_t *dg);
fer_errors fer_verify_prgPacket (const uint16_t* dg);

void fer_extract_packet (const uint16_t* src, uint8_t* dst);
void fer_extract_prgPacket (const uint16_t* src, uint8_t* dst);
void fer_extract_prgPacket2(const uint16_t *src, uint8_t dst[9]);
fer_errors fer_extract_Byte(const uint16_t *src, uint8_t *dst);

#endif