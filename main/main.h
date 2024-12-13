#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "led.h"
#include "rs485.h"
#include "gpio.h"
#include "barco.h"
#include "wifi.h"
#include "mqtt.h"

#define MAIN_TAG "RB_APP"

static void app_task_0(void *arg);
static void app_task_1(void *arg);
static void app_task_led_error(void *arg);