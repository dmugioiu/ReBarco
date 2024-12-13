#include "rs485.h"

void rs485_init(void)
{

    const int uart_num = RS485_UART_PORT;

    uart_config_t uart_config = {
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Set UART log level
    esp_log_level_set(RS485_TAG, ESP_LOG_INFO);

    ESP_LOGI(RS485_TAG, "Initializing peripherals");

    // Install UART driver (we don't need an event queue here)

    ESP_ERROR_CHECK(uart_driver_install(uart_num, RS485_RX_BUF_SIZE * 2, RS485_TX_BUF_SIZE * 2, 0, NULL, 0));

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set UART pins as per KConfig settings
    ESP_ERROR_CHECK(uart_set_pin(uart_num, RS485_TXD, RS485_RXD, RS485_RTS, RS485_CTS));

    // Set RS485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_COLLISION_DETECT));

    ESP_LOGI(RS485_TAG, "Configuration complete");
}

void rs485_send(const char *str, uint8_t length)
{
    const int uart_num = RS485_UART_PORT;
    if (uart_write_bytes(uart_num, str, length) != length)
    {
        ESP_LOGE(RS485_TAG, "Send data critical failure.");
        // add your code to handle sending failure here
        abort();
    }
}