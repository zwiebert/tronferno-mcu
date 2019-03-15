/*
 * fer_imp.h
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */


#ifndef USER_FERNOTRON_FER_IMP_H_
#define USER_FERNOTRON_FER_IMP_H_

#include "fer.h"

// defined in MCU specific code
bool mcu_get_rxPin();
void mcu_put_txPin(bool dat);


#endif /* USER_FERNOTRON_FER_IMP_H_ */
