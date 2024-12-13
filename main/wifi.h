#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define WIFI_TAG "WIFI"

#define ESP_WIFI_SSID "SSID"
#define ESP_WIFI_PASS "password"
#define ESP_MAXIMUM_RETRY 5

#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK

void wifi_init_sta(void);