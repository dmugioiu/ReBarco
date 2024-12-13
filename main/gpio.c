#include "gpio.h"

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_handler_task(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            ESP_LOGI(GPIO_TAG, "GPIO[%" PRIu32 "] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

uint8_t gpio_get_rotary(void)
{

    uint8_t result = 0;
    result |= (!gpio_get_level(GPIO_SELECTOR_IO_0) << 0);
    result |= (!gpio_get_level(GPIO_SELECTOR_IO_1) << 1);
    result |= (!gpio_get_level(GPIO_SELECTOR_IO_2) << 2);
    result |= (!gpio_get_level(GPIO_SELECTOR_IO_3) << 3);

    return result;
}

void gpio_init(void)
{

    ESP_LOGI(GPIO_TAG, "Initializing peripherals");
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};

    // Configure Rotary Selector Pins

    // disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    // bit mask of the pins
    io_conf.pin_bit_mask = GPIO_SELECTOR_PIN_SEL;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;

    // configure GPIO with the given settings
    gpio_config(&io_conf);

    // Configure User Button

    // interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    // bit mask of the pins
    io_conf.pin_bit_mask = GPIO_BTN_PIN_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;

    // configure GPIO with the given settings
    gpio_config(&io_conf);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_BTN_IO_0, gpio_isr_handler, (void *)GPIO_BTN_IO_0);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    // start gpio task
    xTaskCreate(gpio_handler_task, "gpio_handler_task", 2048, NULL, 10, NULL);

    ESP_LOGI(GPIO_TAG, "Configuration complete");
}