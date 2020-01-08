/*
 * ota.h
 *
 *  Created on: 17.05.2019
 *      Author: bertw
 */

#ifndef MAIN_OTA_H_
#define MAIN_OTA_H_

#include <stdbool.h>

// downloads firmwareUpdate into OTA partion
// if success you should can call esp_restart() to run the new firwmare
bool ota_doUpdate(const char *firmwareURL);


#endif /* MAIN_OTA_H_ */
