#include <app_mqtt_lib/app_mqtt_lib.h>
#include <zephyr/kernel.h>

int main(void) {
  app_mqtt_lib_init();

  int result = app_mqtt_lib_do_something(21);
  printk("Result = %d\n", result);

  while (1) {
    k_sleep(K_SECONDS(1));
  }
}
