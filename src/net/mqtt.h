/*
 * mqtt.h
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_MQTT_H_
#define SRC_USERIO_MQTT_H_

#include "user_config.h"
#include "userio/status_output.h"

void io_mqtt_enable(bool enable);


#ifdef USE_MQTT
void io_mqtt_publish_gmp(const so_arg_gmp_t *gmp);
void io_mqtt_publish_config(const char *json);


// low level wrapper to hide MQTT implementation
void io_mqtt_create_and_start(void);
void io_mqtt_stop_and_destroy(void);
void io_mqtt_subscribe(const char *topic, int qos);
void io_mqtt_unsubscribe(const char *topic);
void io_mqtt_publish(const char *topic, const char *data);




#else
#define io_mqtt_create_and_start()
#define io_mqtt_stop_and_destroy()
#define io_mqtt_subscribe(topic, qos)
#define io_mqtt_unsubscribe(topic)
#define io_mqtt_publish(topic, data)
#define io_mqtt_publish_gmp(gmp);
#endif



#endif /* SRC_USERIO_MQTT_H_ */
