/*
 * mqtt.h
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_MQTT_H_
#define SRC_USERIO_MQTT_H_

#include "app_config/proj_app_cfg.h"
#include "stdbool.h"

void io_mqtt_enable(bool enable);


#ifdef USE_MQTT
// low level wrapper to hide MQTT implementation
void io_mqtt_create_and_start(void);
void io_mqtt_stop_and_destroy(void);
void io_mqtt_subscribe(const char *topic, int qos);
void io_mqtt_unsubscribe(const char *topic);
void io_mqtt_publish(const char *topic, const char *data);
void io_mqtt_setup(const char *client_id);

#else

#define io_mqtt_create_and_start()
#define io_mqtt_stop_and_destroy()
#define io_mqtt_subscribe(topic, qos)
#define io_mqtt_unsubscribe(topic)
#define io_mqtt_publish(topic, data)
#define setup_mqtt()
#endif


void io_mqttApp_enable(bool enable);
void io_mqttApp_disconnected();
void io_mqttApp_subscribed(const char *topic, int topic_len);
void io_mqttApp_unsubscribed(const char *topic, int topic_len);
void io_mqttApp_published(int msg_id);




// interface implemented in ./mqtt.c called by MQTT implementation (mcu specific)

void io_mqtt_disconnected ();
void io_mqtt_subscribed(const char *topic, int topic_len);
void io_mqtt_unsubscribed(const char *topic, int topic_len);
void io_mqtt_published(int msg_id);
// ...app-specific:
void io_mqtt_received(const char *topic, int topic_len, const char *data, int data_len);
void io_mqtt_connected ();

// helper functions
bool topic_startsWith(const char *topic, int topic_len, const char *s);
bool topic_endsWith(const char *topic, int topic_len, const char *s);



#endif /* SRC_USERIO_MQTT_H_ */
