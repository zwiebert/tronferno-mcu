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

inline bool Fer_Trx_API::send_cmd(uint32_t a, uint8_t g, uint8_t m, fer_if_cmd cmd, uint8_t repeats, uint16_t delay, uint16_t stopDelay) {
  Fer_MsgCmd msg { a, g, m, cmd, repeats, delay, stopDelay };
  return fer_trx_send_cmd(&msg);
}

inline bool Fer_Trx_API::send_rtc(uint32_t a, uint8_t g, uint8_t m, time_t rtc) {
  Fer_MsgRtc msg { a, g, m, rtc };
  return fer_trx_send_rtc(&msg);
}

inline bool Fer_Trx_API::send_timer(uint32_t a, uint8_t g, uint8_t m, time_t rtc, const Fer_TimerData &td) {
  Fer_MsgTimer msg { a, g, m, rtc, &td };
  return fer_trx_send_timer(&msg);
}

inline bool Fer_Trx_API::send_empty_timer(uint32_t a, uint8_t g, uint8_t m, time_t rtc) {
  Fer_MsgRtc msg { a, g, m, rtc };
  return fer_trx_send_empty_timer(&msg);
}


///////////////// ISR //////////////////////////////
inline void IRAM_ATTR Fer_Trx_API::isr_handle_rx(bool rx_pin_level) {
  fer_rx_tick(rx_pin_level);
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

inline uint32_t Fer_Trx_API::get_a() const {
  return myMsg.a;
}

inline uint8_t Fer_Trx_API::get_g() const {
  return myMsg.g;
}

inline uint8_t Fer_Trx_API::get_m() const {
  return myMsg.m;
}

inline fer_if_cmd Fer_Trx_API::get_cmd() const {
  return myMsg.cmd;
}

inline bool Fer_Trx_API::is_centralUnit() const {
  return FER_U32_TEST_TYPE(myMsg.a, FER_ADDR_TYPE_CentralUnit);
}
