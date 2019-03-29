/*
 * mqtt.h
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_MQTT_H_
#define SRC_USERIO_MQTT_H_

#include <stdbool.h>

void io_mqtt_enable(bool enable);

#ifdef USE_MQTT
// interface which has to be implemented by mcu specific code
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
#endif


// interface implemented in ./mqtt.c which will be called by mcu specific code
void io_mqtt_connected ();
void io_mqtt_disconnected ();
void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len);
void io_mqtt_subscribed(const char *topic, int topic_len);
void io_mqtt_unsubscribed(const char *topic, int topic_len);
void io_mqtt_published(int msg_id);

#endif /* SRC_USERIO_MQTT_H_ */
