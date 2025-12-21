
#ifndef APP_MQTT_LIB_H_
#define APP_MQTT_LIB_H_

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

int app_mqtt_lib_init(void);
int app_mqtt_lib_do_something(int value);

#ifdef __cplusplus
}
#endif

#endif
