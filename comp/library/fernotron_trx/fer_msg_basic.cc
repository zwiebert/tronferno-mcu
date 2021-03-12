#include "fer_app_cfg.h"
#include "fernotron_trx/fer_trx_c_api.h"
#include "fernotron_trx/raw/fer_msg_plain.h"
#include "fernotron_trx/raw/fer_fsb.h"
#include <fernotron_trx/raw/fer_msg_tx.h>

static void
fer_init_plain(fer_sbT *fsb, u32 a) {
  FER_SB_PUT_DEVID(fsb, a);
  fsb->sd.memb = fer_memb_FromPlainSender;
  fsb->sd.grp = fer_grp_Broadcast;
  fsb->sd.cmd = fer_cmd_None;
}

void 
fer_init_sender(fer_sbT *fsb, u32 devID) {

  fer_init_plain(fsb, devID);

  switch (GET_BYTE_2(devID)) {
    case FER_PlainSender:
        FER_SB_PUT_MEMB(fsb, fer_memb_FromPlainSender);
    break;

    case FER_SunSensor:
    FER_SB_PUT_MEMB(fsb, fer_memb_FromSunSensor);
    break;

    case FER_CentralUnit:
    break;
    
    case FER_Receiver:
    	FER_SB_PUT_MEMB(fsb, fer_memb_ToReceiver);
    break;

    default:
    break;
  }
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
fer_update_tglNibble(fer_sbT *fsb) {
    FER_SB_PUT_TGL(fsb, fer_tglNibble_ctUp(FER_SB_GET_TGL(fsb), 1));
}


