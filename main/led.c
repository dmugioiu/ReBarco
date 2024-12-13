#include "led.h"

static rgb_channel_config_t rgb_led_1_channels = {
    .red_channel = LEDC_CHANNEL_RED,
    .green_channel = LEDC_CHANNEL_GREEN,
    .blue_channel = LEDC_CHANNEL_BLUE,
};

#if CONFIG_GAMMA_CORRECTION_WITH_LUT

static uint32_t gamma_correction_calculator(uint32_t duty)
{
    return gamma_correction_lut[duty * 100 / (1 << LEDC_DUTY_RES)] * (1 << LEDC_DUTY_RES);
}
#else // !CONFIG_GAMMA_CORRECTION_WITH_LUT
#define GAMMA_FACTOR (2.8)

static uint32_t gamma_correction_calculator(uint32_t duty)
{
    return pow((double)duty / (1 << LEDC_DUTY_RES), GAMMA_FACTOR) * (1 << LEDC_DUTY_RES);
}
#endif // CONFIG_GAMMA_CORRECTION_WITH_LUT

static void rgb_set_duty_and_update(rgb_channel_config_t rgb_channels,
                                    uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty)
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, rgb_channels.red_channel, target_r_duty));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, rgb_channels.green_channel, target_g_duty));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, rgb_channels.blue_channel, target_b_duty));

    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, rgb_channels.red_channel));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, rgb_channels.green_channel));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, rgb_channels.blue_channel));
}

static void rgb_set_linear_fade(rgb_channel_config_t rgb_channels,
                                uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty,
                                uint32_t duration)
{
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, rgb_channels.red_channel, target_r_duty, duration));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, rgb_channels.green_channel, target_g_duty, duration));
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, rgb_channels.blue_channel, target_b_duty, duration));
}

static void rgb_set_gamma_curve_fade(rgb_channel_config_t rgb_channels,
                                     uint32_t start_r_duty, uint32_t start_g_duty, uint32_t start_b_duty,
                                     uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty,
                                     uint32_t duration)
{
    const uint32_t linear_fade_segments = 12;
    uint32_t actual_fade_ranges;
    ledc_fade_param_config_t fade_params_list[SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX] = {};

    ESP_ERROR_CHECK(ledc_fill_multi_fade_param_list(LEDC_MODE, rgb_channels.red_channel,
                                                    start_r_duty, target_r_duty,
                                                    linear_fade_segments, duration,
                                                    gamma_correction_calculator,
                                                    SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX,
                                                    fade_params_list, &actual_fade_ranges));
    ESP_ERROR_CHECK(ledc_set_multi_fade(LEDC_MODE, rgb_channels.red_channel, gamma_correction_calculator(start_r_duty), fade_params_list, actual_fade_ranges));

    ESP_ERROR_CHECK(ledc_fill_multi_fade_param_list(LEDC_MODE, rgb_channels.green_channel,
                                                    start_g_duty, target_g_duty,
                                                    linear_fade_segments, duration,
                                                    gamma_correction_calculator,
                                                    SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX,
                                                    fade_params_list, &actual_fade_ranges));
    ESP_ERROR_CHECK(ledc_set_multi_fade(LEDC_MODE, rgb_channels.green_channel, gamma_correction_calculator(start_g_duty), fade_params_list, actual_fade_ranges));

    ESP_ERROR_CHECK(ledc_fill_multi_fade_param_list(LEDC_MODE, rgb_channels.blue_channel,
                                                    start_b_duty, target_b_duty,
                                                    linear_fade_segments, duration,
                                                    gamma_correction_calculator,
                                                    SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX,
                                                    fade_params_list, &actual_fade_ranges));
    ESP_ERROR_CHECK(ledc_set_multi_fade(LEDC_MODE, rgb_channels.blue_channel, gamma_correction_calculator(start_b_duty), fade_params_list, actual_fade_ranges));
}

static void rgb_fade_start(rgb_channel_config_t rgb_channels)
{
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, rgb_channels.red_channel, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, rgb_channels.green_channel, LEDC_FADE_NO_WAIT));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, rgb_channels.blue_channel, LEDC_FADE_NO_WAIT));
}

void rgb_ledc_init(void)
{
    ESP_LOGI(LEDC_TAG, "Initializing peripherals");

    ledc_fade_func_install(0);
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY, // Set output frequency at 4 kHz
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM configuration to the six channels
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_MODE,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 8192, // Set initial duty to 0%
        .hpoint = 0};
    ledc_channel.channel = LEDC_CHANNEL_RED;
    ledc_channel.gpio_num = LEDC_RED_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.channel = LEDC_CHANNEL_GREEN;
    ledc_channel.gpio_num = LEDC_GREEN_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_channel.channel = LEDC_CHANNEL_BLUE;
    ledc_channel.gpio_num = LEDC_BLUE_IO;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(LEDC_TAG, "Configuration complete");
}

void rgb_set_fade_and_start(uint32_t start_r_duty, uint32_t start_g_duty, uint32_t start_b_duty,
                            uint32_t target_r_duty, uint32_t target_g_duty, uint32_t target_b_duty,
                            uint32_t duration)
{

    rgb_set_gamma_curve_fade(rgb_led_1_channels, start_r_duty, start_g_duty, start_b_duty, target_r_duty, target_g_duty, target_b_duty, duration);
    rgb_fade_start(rgb_led_1_channels);
}