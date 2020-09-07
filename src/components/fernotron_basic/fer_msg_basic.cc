#include "fer_app_cfg.h"
#include "fernotron/fer_msg_plain.h"
#include "fernotron/fsb.h"

void 
fer_init_sender(fsbT *fsb, u32 devID) {

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

void 
fer_init_plain(fsbT *fsb, u8 a2, u8 a1, u8 a0) {
  FSB_PUT_ADDR(fsb, a2, a1, a0);
  FSB_PUT_TGL(fsb, 1);
  FSB_PUT_MEMB(fsb, fer_memb_FromPlainSender);
  FSB_PUT_GRP(fsb, fer_grp_Broadcast);
  FSB_PUT_CMD(fsb, fer_cmd_None);
}

void 
fer_init_sunSensor(fsbT *fsb, u8 a2, u8 a1, u8 a0) {
  fer_init_plain(fsb, a2, a1, a0);
  FSB_PUT_MEMB(fsb, fer_memb_FromSunSensor);
}


/* count down TOGGLE_NIBBLE by STEP. Skip zero.

Stop key hold down on central unit (-1): (...,1,2,3,4,5,6,7,8,9,a,b,c,d,e,1,2,...)
Up/Down key hold down on central unit (unchanged) (...,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,...)
*/
u8 
fer_tglNibble_ctUp(u8 toggle_nibble, int step) {
  u8 result = (toggle_nibble) & 0x0f;
  while(step-- > 0) {
    result = (result + 1) & 0x0f;
    if (result == 0)
    ++result;
  }

  return result;
}


void 
fer_update_tglNibble(fsbT *fsb) {
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


