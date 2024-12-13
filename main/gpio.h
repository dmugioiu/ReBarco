#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define GPIO_TAG "RB_GPIO"

#define GPIO_SELECTOR_IO_0 (GPIO_NUM_0)
#define GPIO_SELECTOR_IO_1 (GPIO_NUM_1)
#define GPIO_SELECTOR_IO_2 (GPIO_NUM_2)
#define GPIO_SELECTOR_IO_3 (GPIO_NUM_3)

#define GPIO_SELECTOR_PIN_SEL ((1ULL << GPIO_SELECTOR_IO_0) | (1ULL << GPIO_SELECTOR_IO_1) | (1ULL << GPIO_SELECTOR_IO_2) | (1ULL << GPIO_SELECTOR_IO_3))

#define GPIO_BTN_IO_0 (GPIO_NUM_9)

#define GPIO_BTN_PIN_SEL (1ULL << GPIO_BTN_IO_0)

#define ESP_INTR_FLAG_DEFAULT 0

void gpio_init(void);
uint8_t gpio_get_rotary(void);