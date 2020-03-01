/*
 * mqtt_imp.h
 *
 *  Created on: 31.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_MQTT_IMP_H_
#define SRC_USERIO_MQTT_IMP_H_


// interface implemented in ./mqtt.c called by MQTT implementation (mcu specific)
void io_mqtt_connected ();
void io_mqtt_disconnected ();
void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len);
void io_mqtt_subscribed(const char *topic, int topic_len);
void io_mqtt_unsubscribed(const char *topic, int topic_len);
void io_mqtt_published(int msg_id);

#endif /* SRC_USERIO_MQTT_IMP_H_ */
