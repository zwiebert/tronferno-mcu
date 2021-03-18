/*
 * trx.hh
 *
 *  Created on: Feb 25, 2021
 *      Author: bertw
 */

#pragma once

bool cc1101_ook_setDirection(bool tx);

bool cc1101_ook_gdo_invert(int numb, bool inverted_output);
bool cc1101_ook_gdo_hw(int gdo_num, bool high);
bool cc1101_ook_gdo_isConnected(int gdo_num, int gpio_num);
