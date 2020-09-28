#include <fernotron/trx/fer_trx_incoming.hh>
#include "fer_trx_incoming_event.hh"

static Fer_Trx_IncomingMsg *OurDerivedObject;

void Fer_Trx_IncomingMsg::setup(Fer_Trx_IncomingMsg *derived_object) {
  OurDerivedObject = derived_object;
}

u32 Fer_Trx_IncomingMsg::get_a() const {
  return FER_SB_GET_DEVID(&myEvt->fsb);
}

u8 Fer_Trx_IncomingMsg::get_g() const {
  if (!is_centralUnit())
    return 0;

  fer_grp grp = myEvt->fsb.sd.grp;
  return grp;
}

u8 Fer_Trx_IncomingMsg::get_m() const {
  if (!is_centralUnit())
    return 0;

  fer_memb memb = myEvt->fsb.sd.memb;
  return memb == 0 ? 0 : memb - 7;
}

fer_if_cmd Fer_Trx_IncomingMsg::get_cmd() const {
  return static_cast<fer_if_cmd>(myEvt->fsb.sd.cmd);
}

const void *Fer_Trx_IncomingMsg::get_raw() const {
  return myEvt->raw;
}

const void *Fer_Trx_IncomingMsg::get_fsb() const {
  return &myEvt->fsb;
}

bool Fer_Trx_IncomingMsg::is_centralUnit() const {
  return FER_SB_ADDR_IS_CENTRAL(&myEvt->fsb);
}

Fer_Trx_IncomingMsg::MsgKind Fer_Trx_IncomingMsg::get_msgKind() const {
  return static_cast<MsgKind>(myEvt->kind);
}

void Fer_Trx_IncomingMsg::push_event(struct Fer_Trx_IncomingEvent *evt) {
  if (!OurDerivedObject)
    return;

  Fer_Trx_IncomingMsg &that = *OurDerivedObject;
  that.myEvt = evt;

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
