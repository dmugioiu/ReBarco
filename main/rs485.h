#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define RS485_TAG "RB_RS485"

#define RS485_UART_PORT (UART_NUM_1)

// Note: Some pins on target chip cannot be assigned for UART communication.
// Please refer to documentation for selected board and target to configure pins using Kconfig.
#define RS485_TXD (GPIO_NUM_18)
#define RS485_RXD (GPIO_NUM_19)

// RTS for RS485 Half-Duplex Mode manages DE/~RE
#define RS485_RTS (GPIO_NUM_20)

// CTS is not used in RS485 Half-Duplex Mode
#define RS485_CTS (UART_PIN_NO_CHANGE)

#define RS485_TX_BUF_SIZE (1024 * 2)
#define RS485_RX_BUF_SIZE (1024 * 2)
#define RS485_BAUD_RATE (500000)

void rs485_init(void);
void rs485_send(const char *str, uint8_t length);