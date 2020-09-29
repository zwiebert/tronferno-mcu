#include <app/config/proj_app_cfg.h>

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
