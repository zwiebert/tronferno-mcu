#ifndef COMPONENTS_FERNOTRON_PINS_H_
#define COMPONENTS_FERNOTRON_PINS_H_

#include <stdint.h>

uint8_t mcu_get_rxPin();  //IMPLEMENT_ME (called from interrupt!)
void mcu_put_txPin(uint8_t on); //IMPLEMENT_ME (called from interrupt!)
void mcu_delay_us(uint16_t us); //IMPLEMENT_ME

#endif /* COMPONENTS_FERNOTRON_PINS_H_ */
