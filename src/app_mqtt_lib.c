
#include "zephyr/net/net_ip.h"
#include <app_mqtt_lib/app_mqtt_lib.h>
#include <app_poll_lib/app_poll_lib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/mqtt.h>

LOG_MODULE_REGISTER(app_mqtt_lib, LOG_LEVEL_INF);

static struct mqtt_client client_ctx;
static struct sockaddr_in mqtt_addr;
static uint8_t mqtt_client_id[256];
static uint8_t rx_buffer[256];
static uint8_t tx_buffer[256];

static poll_timeout_handler prev_poll_timeout_handler; // = NULL;
//
static K_SEM_DEFINE(mqtt_connected, 0, 1);
static void mqtt_evt_handler(struct mqtt_client *client,
                             const struct mqtt_evt *evt) {
  LOG_INF("mqtt_evt_handler with event:%d", evt->type);

  switch (evt->type) {
  case MQTT_EVT_PUBLISH:
    // TODO: on publish
    break;
  case MQTT_EVT_CONNACK:
    k_sem_give(&mqtt_connected);
    // app_mqtt_on_connect(client, evt);
    break;
  case MQTT_EVT_PINGRESP:
    LOG_INF("MQTT ping respond");
    break;
  case MQTT_EVT_DISCONNECT:
    // app_mqtt_on_evt_disconnect(client, evt);
    // TODO: on disconnect
    break;

  case MQTT_EVT_PUBACK:
    // app_mqtt_on_evt_puback(client, evt);
    break;

  case MQTT_EVT_PUBREC:
    // app_mqtt_on_evt_pubrec(client, evt);

    break;
  case MQTT_EVT_PUBCOMP:
    // app_mqtt_on_evt_pubcomp(client, evt);
    break;

  default:
    break;
  }
}

static void mqtt_init_context(const struct sockaddr_in *addr) {
  // __mqtt_session.client_ctx.broker = &__mqtt_session.broker;
  // __mqtt_session.client_ctx.evt_cb = app_mqtt_evt_handler;
  // __mqtt_session.client_ctx.client_id.utf8 =
  //     (uint8_t *)"zephyr_mqtt_client"; // TODO: Use device id
  // __mqtt_session.client_ctx.client_id.size = sizeof("zephyr_mqtt_client") -
  // 1;
  // __mqtt_session.client_ctx.password = NULL;
  // __mqtt_session.client_ctx.user_name = NULL;
  // __mqtt_session.client_ctx.protocol_version = MQTT_VERSION_3_1_1;
  // __mqtt_session.client_ctx.transport.type = MQTT_TRANSPORT_NON_SECURE;
  //
  // /* MQTT buffers configuration */
  // __mqtt_session.client_ctx.rx_buf = __mqtt_session.rx_buffer;
  // __mqtt_session.client_ctx.rx_buf_size = sizeof(__mqtt_session.rx_buffer);
  // __mqtt_session.client_ctx.tx_buf = __mqtt_session.tx_buffer;
  // __mqtt_session.client_ctx.tx_buf_size = sizeof(__mqtt_session.tx_buffer);
  // mqtt_init_context(&mqtt_addr);
  client_ctx.broker = addr;
  client_ctx.evt_cb = mqtt_evt_handler;
  client_ctx.client_id.utf8 = mqtt_client_id;
  client_ctx.client_id.size = strlen(mqtt_client_id);
  client_ctx.protocol_version = MQTT_VERSION_3_1_1;
  client_ctx.transport.type = MQTT_TRANSPORT_NON_SECURE;

  client_ctx.password = NULL;
  client_ctx.user_name = NULL;
  // Buffer configuration
  client_ctx.rx_buf = rx_buffer;
  client_ctx.rx_buf_size = sizeof(rx_buffer);
  client_ctx.tx_buf = tx_buffer;
  client_ctx.tx_buf_size = sizeof(tx_buffer);
}

static int app_mqtt_publish_impl(struct mqtt_client *client,
                                 struct mqtt_topic topic,
                                 struct mqtt_binstr payload) {

  static uint16_t msg_id = 1;
  int rc;
  struct mqtt_publish_param param;

  param.message.topic = topic;
  param.message.payload = payload;
  param.message_id = msg_id++;
  param.dup_flag = 0;
  param.retain_flag = 0;

  rc = mqtt_publish(client, &param);
  if (rc != 0) {
    LOG_ERR("MQTT Publish failed [%d]", rc);
  }

  LOG_INF("Published to topic '%s', QoS %d", param.message.topic.topic.utf8,
          param.message.topic.qos);

  return rc;
}

int app_mqtt_publish(struct mqtt_topic topic, struct mqtt_binstr payload) {
  return app_mqtt_publish_impl(&client_ctx, topic, payload);
}

static void app_mqtt_poll_handler(int fd, short revents, void *ud,
                                  struct PollLoop *L) {
  LOG_INF("poll handler");
  if (revents & POLLIN) {
    mqtt_input(&client_ctx);
  }
  // if (prev_poll_timeout_handler != NULL) {
  //   prev_poll_timeout_handler();
  // }
}

static void mqtt_poll_timeout_handler() {
  LOG_INF("LOOP timeout, sending the ping");
  mqtt_ping(&client_ctx);
}
int app_mqtt_lib_init(const uint8_t *client_id,
                      const struct sockaddr_in *addr) {
  mqtt_client_init(&client_ctx);
  strncpy(mqtt_client_id, client_id, sizeof(mqtt_client_id));
  mqtt_addr.sin_addr.s_addr = addr->sin_addr.s_addr;
  mqtt_addr.sin_family = AF_INET;
  mqtt_addr.sin_port = addr->sin_port;

  // broker_addr->sin_addr.s_addr = addr->sin_addr.s_addr;
  // broker_addr->sin_family = AF_INET;
  // broker_addr->sin_port = addr->sin_port;
  // __mqtt_session.client_ctx.broker = &__mqtt_session.broker;
  // __mqtt_session.client_ctx.evt_cb = app_mqtt_evt_handler;
  // __mqtt_session.client_ctx.client_id.utf8 =
  //     (uint8_t *)"zephyr_mqtt_client"; // TODO: Use device id
  // __mqtt_session.client_ctx.client_id.size = sizeof("zephyr_mqtt_client") -
  // 1;
  // __mqtt_session.client_ctx.password = NULL;
  // __mqtt_session.client_ctx.user_name = NULL;
  // __mqtt_session.client_ctx.protocol_version = MQTT_VERSION_3_1_1;
  // __mqtt_session.client_ctx.transport.type = MQTT_TRANSPORT_NON_SECURE;
  //
  // /* MQTT buffers configuration */
  // __mqtt_session.client_ctx.rx_buf = __mqtt_session.rx_buffer;
  // __mqtt_session.client_ctx.rx_buf_size = sizeof(__mqtt_session.rx_buffer);
  // __mqtt_session.client_ctx.tx_buf = __mqtt_session.tx_buffer;
  // __mqtt_session.client_ctx.tx_buf_size = sizeof(__mqtt_session.tx_buffer);
  // mqtt_init_context(&mqtt_addr);
  mqtt_init_context(addr);

  int rc = mqtt_connect(&client_ctx);
  if (rc != 0) {
    LOG_ERR("Error in connecting MQTT: rc=%d", rc);
    return rc;
  }

  prev_poll_timeout_handler =
      app_register_poll_timeout_handler(mqtt_poll_timeout_handler);

  int fd_idx = register_poll_fd(client_ctx.transport.tcp.sock,
                                app_mqtt_poll_handler, ZSOCK_POLLIN, NULL);

  LOG_INF("app_mqtt_lib init");
  return 0;
}

int app_mqtt_lib_do_something(int value) {
  LOG_INF("Doing something with %d", value);
  return value * 2;
}

void wait_for_mqtt() { k_sem_take(&mqtt_connected, K_FOREVER); }
