#include "main.h"

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(MAIN_TAG, "Initializing Drivers");

    // Set the LEDC peripheral configuration
    gpio_init();
    rgb_ledc_init();
    rs485_init();

    // Clear LED Registers
    barco_sendoff();

    // Get Mode Switch Position
    uint8_t mode = gpio_get_rotary();
    ESP_LOGI(MAIN_TAG, "Rotary Switch Position [0x%01X]", mode);

    // Run Task for selected mode
    switch (mode)
    {

    // Mode 0: Show Test Pattern on LEDs
    case 0x0:
        rgb_set_fade_and_start(OFF_R, OFF_G, OFF_B, GREEN_R, GREEN_G, GREEN_B, 500);
        xTaskCreate(app_task_0, "task_main_mode_0", 2048 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
        break;

    // Mode 1: Connect to WiFi and connect to MQTT Broker
    case 0x1:
        rgb_set_fade_and_start(OFF_R, OFF_G, OFF_B, ORANGE_R, ORANGE_G, ORANGE_B, 500);
        xTaskCreate(app_task_1, "task_main_mode_1", 4096 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
        break;

    default:
        ESP_LOGE(MAIN_TAG, "Undefined Mode");
        xTaskCreate(app_task_led_error, "task_led_error", 1024 * 2, NULL, configMAX_PRIORITIES - 2, NULL);
        break;
    }
}

static void app_task_0(void *arg)
{
    ESP_LOGI(MAIN_TAG, "Streaming Data to Barco");
    while (1)
    {
        barco_sendtestpattern();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void app_task_1(void *arg)
{
    ESP_LOGI(MAIN_TAG, "Streaming Data to Barco");
    wifi_init_sta();
    mqtt_app_start();

    while (1)
    {

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void app_task_led_error(void *arg)
{

    while (1)
    {
        rgb_set_fade_and_start(OFF_R, OFF_G, OFF_B, RED_R, RED_G, RED_B, 500);
        vTaskDelay(pdMS_TO_TICKS(5));
        rgb_set_fade_and_start(RED_R, RED_G, RED_B, OFF_R, OFF_G, OFF_B, 500);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}