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
  fer_grp grp = myEvt->fsb.sd.grp;
  return grp;
}
u8 Fer_Trx_IncomingMsg::get_m() const {
  fer_memb memb = myEvt->fsb.sd.memb;
  return memb == 0 ? 0 : memb - 7;
}

#if 0
virtual void Fer_Trx_IncomingMsg::event_plain_message_was_received() {
}
virtual void Fer_Trx_IncomingMsg::event_plain_double_message_was_received() {
}
virtual void Fer_Trx_IncomingMsg::event_rtc_message_was_received() {
}
virtual void Fer_Trx_IncomingMsg::event_timer_message_was_received() {
}
virtual void Fer_Trx_IncomingMsg::event_any_message_was_received() {
}
#endif

void Fer_Trx_IncomingMsg::push_event(struct Fer_Trx_IncomingEvent *evt) {
  if (!OurDerivedObject)
    return;

  Fer_Trx_IncomingMsg &that = *OurDerivedObject;

  that.myEvt = evt;
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

  that.myEvt = 0;
}
