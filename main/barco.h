#pragma once
#include <stdint.h>

#define BARCO_TAG "BARCO"
#define BARCO_LEDS_PER_STRIP (112)
#define BARCO_COL_NUM (3)
#define BARCO_DATA_PAYLOAD_SIZE BARCO_COL_NUM *BARCO_LEDS_PER_STRIP

void barco_sendtestpattern(void);
void barco_sendoff(void);
int barco_sendframe(unsigned char *data);
void barco_sendcolor(uint8_t red, uint8_t green, uint8_t blue);