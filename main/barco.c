#include "barco.h"
#include "rs485.h"

const unsigned char serial_sync_pattern[] = {0x0f, 0xf1, 0x03, 0xf2};

int barco_sendframe(unsigned char *data)
{

    int txBytes = uart_write_bytes(RS485_UART_PORT, serial_sync_pattern, sizeof(serial_sync_pattern));
    txBytes += uart_write_bytes(RS485_UART_PORT, data, BARCO_DATA_PAYLOAD_SIZE);
    ESP_LOGI(BARCO_TAG, "Wrote %d bytes", txBytes);
    return txBytes;
}

void barco_sendtestpattern(void)
{

    unsigned char serial_test_pattern[BARCO_DATA_PAYLOAD_SIZE] = {0};

    for (int i = 0; i < BARCO_LEDS_PER_STRIP; i++)
    {
        for (int c = 0; c < BARCO_COL_NUM; c++)
        {
            serial_test_pattern[i * BARCO_COL_NUM + c] = i * 2;
        }
    }

    barco_sendframe(serial_test_pattern);
}

void barco_sendoff(void)
{

    unsigned char off_pattern[BARCO_DATA_PAYLOAD_SIZE] = {0};

    barco_sendframe(off_pattern);
}

void barco_sendcolor(uint8_t red, uint8_t green, uint8_t blue)
{

    unsigned char color_pattern[BARCO_DATA_PAYLOAD_SIZE] = {0};

    for (int i = 0; i < BARCO_DATA_PAYLOAD_SIZE; i = i + 3)
    {
        color_pattern[i] = red;
        color_pattern[i + 1] = green;
        color_pattern[i + 2] = blue;
    }

    barco_sendframe(color_pattern);
}
