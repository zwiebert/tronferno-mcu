#include "user_config.h"
#include "fernotron/fer_code.h"

void ICACHE_FLASH_ATTR
fer_init_sender(fer_sender_basic *fsb, uint32_t devID) {

  fer_init_plain(fsb, GET_BYTE_2(devID), GET_BYTE_1(devID), GET_BYTE_0(devID));

  switch (GET_BYTE_2(devID)) {
    case FER_ADDR_TYPE_PlainSender:
        FSB_PUT_MEMB(fsb, fer_memb_FromPlainSender);
    break;

    case FER_ADDR_TYPE_SunSensor:
    FSB_PUT_MEMB(fsb, fer_memb_FromSunSensor);
    break;

    case FER_ADDR_TYPE_CentralUnit:
    break;
    
    case FER_ADDR_TYPE_Receiver:
    	FSB_PUT_MEMB(fsb, fer_memb_ToReceiver);
    break;

    default:
    break;
  }
}

void ICACHE_FLASH_ATTR
fer_init_plain(fer_sender_basic *fsb, uint8_t a2, uint8_t a1, uint8_t a0) {
  FSB_PUT_ADDR(fsb, a2, a1, a0);
  FSB_PUT_TGL(fsb, 1);
  FSB_PUT_MEMB(fsb, fer_memb_FromPlainSender);
  FSB_PUT_GRP(fsb, fer_grp_Broadcast);
  FSB_PUT_CMD(fsb, fer_cmd_None);
  fsb->repeats = -1;
}

void ICACHE_FLASH_ATTR
fer_init_sunSensor(fer_sender_basic *fsb, uint8_t a2, uint8_t a1, uint8_t a0) {
  fer_init_plain(fsb, a2, a1, a0);
  FSB_PUT_MEMB(fsb, fer_memb_FromSunSensor);
}


/* count down TOGGLE_NIBBLE by STEP. Skip zero.

Stop key hold down on central unit (-1): (...,1,2,3,4,5,6,7,8,9,a,b,c,d,e,1,2,...)
Up/Down key hold down on central unit (unchanged) (...,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,...)
*/
uint8_t ICACHE_FLASH_ATTR
fer_tglNibble_ctUp(uint8_t toggle_nibble, int step) {
  uint8_t result = (toggle_nibble) & 0x0f;
  while(step-- > 0) {
    result = (result + 1) & 0x0f;
    if (result == 0)
    ++result;
  }

  return result;
}


void ICACHE_FLASH_ATTR
fer_update_tglNibble(fer_sender_basic *fsb) {
#if 0
  int step = 0;

  step = !FSB_ADDR_IS_CENTRAL(fsb) ? 1
  : (FSB_IS_BUTTON_HOLD(fsb) ?  (FSB_GET_CMD(fsb) == fer_cmd_STOP ? 1 : 0)
  : 1);
  if (step > 0) {
    FSB_PUT_TGL(fsb, fer_tglNibble_ctUp(FSB_GET_TGL(fsb), step));
  }
  #else
    FSB_PUT_TGL(fsb, fer_tglNibble_ctUp(FSB_GET_TGL(fsb), 1));
#endif

}

