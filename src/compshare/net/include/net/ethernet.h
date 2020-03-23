/*
 * ethernet.h
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_NET_INCLUDE_NET_ETHERNET_H_
#define COMPONENTS_NET_INCLUDE_NET_ETHERNET_H_


enum lanPhy {
  lanPhyNone, lanPhyLAN8270, lanPhyRTL8201, lanPhyIP101, lanPhyLEN,
};


void ethernet_configure(enum lanPhy lan_phy, int lan_pwr_gpio);
void ethernet_setup(enum lanPhy lan_phy, int lan_pwr_gpio);
void ethernet_loop(void);


#endif /* COMPONENTS_NET_INCLUDE_NET_ETHERNET_H_ */
