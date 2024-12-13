#pragma once
#include <stdint.h>
#include "mqtt_client.h"
#include "mdns.h"      // Include mDNS headers for mDNS support
#include "lwip/inet.h" // For inet_ntoa
#include "esp_mac.h"
#include "esp_log.h"
#include "barco.h"
#include "cJSON.h"
#include "led.h"

#define MQTT_TAG "MQTT"
#define MQTT_MDNS_NAME "homeassistant" // The mDNS hostname to resolve
#define MQTT_TOPIC_CMD "rebarco/%s/set"
#define MQTT_TOPIC_STATE "rebarco/%s/state"
#define MQTT_TOPIC_DISCOVERY_PREFIX "homeassistant/light/rebarco_rgb_light/"

#define STANDBY_GREEN_R RGB_TO_DUTY(0)
#define STANDBY_GREEN_G RGB_TO_DUTY(25)
#define STANDBY_GREEN_B RGB_TO_DUTY(0)

void mqtt_app_start(void);
static void mqtt_send_discovery(esp_mqtt_client_handle_t client);