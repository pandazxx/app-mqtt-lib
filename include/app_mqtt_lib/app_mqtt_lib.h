
#ifndef APP_MQTT_LIB_H_
#define APP_MQTT_LIB_H_

#include "zephyr/net/mqtt.h"
#include <zephyr/kernel.h>
#include <zephyr/net/net_ip.h>

#ifdef __cplusplus
extern "C" {
#endif

int app_mqtt_lib_init(const uint8_t *client_id, const struct sockaddr_in* addr);

int app_mqtt_publish(struct mqtt_topic topic, struct mqtt_binstr payload);
int app_mqtt_lib_do_something(int value);
void wait_for_mqtt();

#ifdef __cplusplus
}
#endif

#endif
