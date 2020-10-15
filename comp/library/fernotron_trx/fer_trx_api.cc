#include <fernotron_trx/fer_trx_api.hh>
#include "fer_trx_incoming_event.hh"
#include "fer_trx_api_private.hh"



static Fer_Trx_API *OurDerivedObject;

void Fer_Trx_API::setup(Fer_Trx_API *derived_object) {
  OurDerivedObject = derived_object;
}

void Fer_Trx_API::setup_astro(const struct cfg_astro *cfg_astro) {
  fer_astro_init_and_reinit(cfg_astro);
}

const void *Fer_Trx_API::get_raw() const {
  return myEvt->raw;
}

const void *Fer_Trx_API::get_fsb() const {
  return &myEvt->fsb;
}

Fer_Trx_API::MsgKind Fer_Trx_API::get_msgKind() const {
  return static_cast<MsgKind>(myEvt->kind);
}

//////////////// helper /////////////////////
static Fer_MsgPlainCmd make_plainMsg(const fer_sbT &fsb) {
  Fer_MsgPlainCmd pc { .a = FER_SB_GET_DEVID(&fsb) };
  if (FER_U32_TEST_TYPE(pc.a, FER_ADDR_TYPE_CentralUnit)) {
    pc.g = fsb.sd.grp;
    pc.m = fsb.sd.memb ? fsb.sd.memb - 7 : 0;
    pc.cmd = static_cast<fer_if_cmd>(fsb.sd.cmd);
  }
  return pc;
}



/////////////////////// push functions ////////////////////////////
void fer_trx_api_pushEvent_readyToTransmit() {
  if (!OurDerivedObject)
    return;
  OurDerivedObject->event_ready_to_transmit();
}
void Fer_Trx_API::push_event(struct Fer_Trx_IncomingEvent *evt) {
  if (!OurDerivedObject)
    return;




  Fer_Trx_API &that = *OurDerivedObject;
  that.myEvt = evt;
  that.myMsg = make_plainMsg(evt->fsb);

  if (evt->tx) {
    if (evt->first)
      that.event_first_message_will_be_sent();
    else
      that.event_any_message_will_be_sent();

  } else { //rx
    switch (evt->kind) {
    case MSG_TYPE_PLAIN_DOUBLE:
      that.event_any_message_was_received();
      that.event_plain_double_message_was_received();
      break;

    case MSG_TYPE_PLAIN:
      that.event_any_message_was_received();
      that.event_plain_message_was_received();
      break;

    case MSG_TYPE_RTC:
      that.event_any_message_was_received();
      that.event_rtc_message_was_received();
      break;
    case MSG_TYPE_TIMER:
      that.event_any_message_was_received();
      that.event_timer_message_was_received();
      break;

    case MSG_TYPE_NONE:
    default:
      break;
    }
  }

  that.myEvt = 0;
}








