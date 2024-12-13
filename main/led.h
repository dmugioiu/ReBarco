#pragma once

#include <stdio.h>
#include <math.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define LEDC_TAG "RB_LEDC"

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE

#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY (4000)           // Frequency in Hertz. Set frequency at 4 kHz

// Define two RGB LEDs IOs and channels
#define LEDC_RED_IO (GPIO_NUM_23)
#define LEDC_GREEN_IO (GPIO_NUM_22)
#define LEDC_BLUE_IO (GPIO_NUM_21)

#define LEDC_CHANNEL_RED (LEDC_CHANNEL_0)
#define LEDC_CHANNEL_GREEN (LEDC_CHANNEL_1)
#define LEDC_CHANNEL_BLUE (LEDC_CHANNEL_2)

// Structure to store R, G, B channels
typedef struct
{
    uint32_t red_channel;
    uint32_t green_channel;
    uint32_t blue_channel;
} rgb_channel_config_t;

// Define some colors R, G, B channel PWM duty cycles
#define RGB_TO_DUTY(x) (8192 - (x * (1 << LEDC_DUTY_RES) / 255))

#define OFF_R 8192
#define OFF_G 8192
#define OFF_B 8192

// RED       - R: 255, G: 0  , B: 0
//             H: 0  , S: 100, V: 100
#define RED_R RGB_TO_DUTY(255)
#define RED_G RGB_TO_DUTY(0)
#define RED_B RGB_TO_DUTY(0)

// YELLOW    - R: 255, G: 255, B: 0
//             H: 60 , S: 100, V: 100
#define YELLOW_R RGB_TO_DUTY(255)
#define YELLOW_G RGB_TO_DUTY(255)
#define YELLOW_B RGB_TO_DUTY(0)

// GREEN    - R: 0  , G: 255, B: 0
//            H: 120, S: 100, V: 100
#define GREEN_R RGB_TO_DUTY(0)
#define GREEN_G RGB_TO_DUTY(255)
#define GREEN_B RGB_TO_DUTY(0)

// CYAN      - R: 0  , G: 255, B: 255
//             H: 180, S: 100, V: 100
#define CYAN_R RGB_TO_DUTY(0)
#define CYAN_G RGB_TO_DUTY(255)
#define CYAN_B RGB_TO_DUTY(255)

// BLUE     - R: 0  , G: 0   , B: 255
//            H: 240, S: 100, V: 100
#define BLUE_R RGB_TO_DUTY(0)
#define BLUE_G RGB_TO_DUTY(0)
#define BLUE_B RGB_TO_DUTY(255)

// MAGENTA   - R: 255, G: 0  , B: 255
//             H: 300, S: 100, V: 100
#define MAGENTA_R RGB_TO_DUTY(255)
#define MAGENTA_G RGB_TO_DUTY(0)
#define MAGENTA_B RGB_TO_DUTY(255)

// ORANGE   - R: 255, G: 128, B: 0
//            H: 30 , S: 100, V: 100
#define ORANGE_R RGB_TO_DUTY(255)
#define ORANGE_G RGB_TO_DUTY(128)
#define ORANGE_B RGB_TO_DUTY(0)

// ROSE PINK - R: 255, G: 0  , B: 128
//             H: 330, S: 100, V: 100
#define ROSE_R RGB_TO_DUTY(255)
#define ROSE_G RGB_TO_DUTY(0)
#define ROSE_B RGB_TO_DUTY(128)

// BLUEISH PURPLE - R: 178, G: 102, B: 255
//                  H: 270, S: 60, V: 100
#define BLUEISH_PURPLE_R RGB_TO_DUTY(178)
#define BLUEISH_PURPLE_G RGB_TO_DUTY(102)
#define BLUEISH_PURPLE_B RGB_TO_DUTY(255)

#if CONFIG_GAMMA_CORRECTION_WITH_LUT
// Brightness 0 - 100% gamma correction look up table (gamma = 2.6)
// Y = B ^ 2.6
// Pre-computed LUT to save some runtime computation
static const float gamma_correction_lut[101] = {
    0.000000,
    0.000006,
    0.000038,
    0.000110,
    0.000232,
    0.000414,
    0.000666,
    0.000994,
    0.001406,
    0.001910,
    0.002512,
    0.003218,
    0.004035,
    0.004969,
    0.006025,
    0.007208,
    0.008525,
    0.009981,
    0.011580,
    0.013328,
    0.015229,
    0.017289,
    0.019512,
    0.021902,
    0.024465,
    0.027205,
    0.030125,
    0.033231,
    0.036527,
    0.040016,
    0.043703,
    0.047593,
    0.051688,
    0.055993,
    0.060513,
    0.065249,
    0.070208,
    0.075392,
    0.080805,
    0.086451,
    0.092333,
    0.098455,
    0.104821,
    0.111434,
    0.118298,
    0.125416,
    0.132792,
    0.140428,
    0.148329,
    0.156498,
    0.164938,
    0.173653,
    0.182645,
    0.191919,
    0.201476,
    0.211321,
    0.221457,
    0.231886,
    0.242612,
    0.253639,
    0.264968,
    0.276603,
    0.288548,
    0.300805,
    0.313378,
    0.326268,
    0.339480,
    0.353016,
    0.366879,
    0.381073,
    0.395599,
    0.410461,
    0.425662,
    0.441204,
    0.457091,
    0.473325,
    0.489909,
    0.506846,
    0.524138,
    0.541789,
    0.559801,
    0.578177,
    0.596920,
    0.616032,
    0.635515,
    0.655374,
    0.675610,
    0.696226,
    0.717224,
    0.738608,
    0.760380,
    0.782542,
    0.805097,
    0.828048,
    0.851398,
    0.875148,
    0.899301,
    0.923861,
    0.948829,
    0.974208,
    1.000000,
};
#endif

void rgb_ledc_init(void);
void rgb_set_fade_and_start(uint32_t start_r_duty, uint32_t start_g_duty, uint32_t start_b_duty,
                            uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty,
                            uint32_t duration);