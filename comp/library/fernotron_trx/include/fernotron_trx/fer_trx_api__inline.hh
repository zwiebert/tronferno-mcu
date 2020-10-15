/**
 * \file fernotron_trx/fer_trx_api__inline.hh
 * \brief Private implementation
 */
#include <app_config/proj_app_cfg.h>

////////////////////// loop /////////////////////////
inline void Fer_Trx_API::loop_rx() {
  fer_rx_loop();
}

inline void Fer_Trx_API::loop_tx() {
  fer_tx_loop();
}

inline void Fer_Trx_API::register_callback_msgReceived_ISR(CallBackFnType cb) {
  fer_rx_MSG_RECEIVED_ISR_cb = cb;
}

inline void Fer_Trx_API::register_callback_msgTransmitted_ISR(CallBackFnType cb) {
  fer_tx_MSG_TRANSMITTED_ISR_cb = cb;
}

///////////////// send ////////////////////////////
inline bool Fer_Trx_API::send_cmd(const Fer_MsgCmd &msg) {
  return fer_trx_send_cmd(&msg);
}

inline bool Fer_Trx_API::send_rtc(const Fer_MsgRtc &msg) {
  return fer_trx_send_rtc(&msg);
}

inline bool Fer_Trx_API::send_timer(const Fer_MsgTimer &msg) {
  return fer_trx_send_timer(&msg);
}

inline bool Fer_Trx_API::send_empty_timer(const Fer_MsgRtc &msg) {
  return fer_trx_send_empty_timer(&msg);
}

inline bool Fer_Trx_API::send_cmd(u32 a, u8 g, u8 m, fer_if_cmd cmd, i8 repeats, u16 delay, u16 stopDelay) {
  Fer_MsgCmd msg { a, g, m, cmd, repeats, delay, stopDelay };
  return fer_trx_send_cmd(&msg);
}

inline bool Fer_Trx_API::send_rtc(u32 a, u8 g, u8 m, time_t rtc) {
  Fer_MsgRtc msg { a, g, m, rtc };
  return fer_trx_send_rtc(&msg);
}

inline bool Fer_Trx_API::send_timer(u32 a, u8 g, u8 m, time_t rtc, const Fer_TimerData &td) {
  Fer_MsgTimer msg { a, g, m, rtc, &td };
  return fer_trx_send_timer(&msg);
}

inline bool Fer_Trx_API::send_empty_timer(u32 a, u8 g, u8 m, time_t rtc) {
  Fer_MsgRtc msg { a, g, m, rtc };
  return fer_trx_send_empty_timer(&msg);
}


///////////////// ISR //////////////////////////////
inline void IRAM_ATTR Fer_Trx_API::isr_sample_rx_pin(bool level) {
  fer_rx_sampleInput(level);
}
inline void IRAM_ATTR Fer_Trx_API::isr_handle_rx() {
  fer_rx_tick();
}

inline bool IRAM_ATTR Fer_Trx_API::isr_get_tx_level() {
  return fer_tx_setOutput();
}
inline void IRAM_ATTR Fer_Trx_API::isr_handle_tx() {
  fer_tx_dck();
}


//////////////////// getter //////////////////////////
inline Fer_MsgPlainCmd Fer_Trx_API::get_msg() const {
  return myMsg;
}

inline u32 Fer_Trx_API::get_a() const {
  return myMsg.a;
}

inline u8 Fer_Trx_API::get_g() const {
  return myMsg.g;
}

inline u8 Fer_Trx_API::get_m() const {
  return myMsg.m;
}

inline fer_if_cmd Fer_Trx_API::get_cmd() const {
  return myMsg.cmd;
}

inline bool Fer_Trx_API::is_centralUnit() const {
  return FER_U32_TEST_TYPE(myMsg.a, FER_ADDR_TYPE_CentralUnit);
}
