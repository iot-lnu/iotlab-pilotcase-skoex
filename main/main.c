#include <stdio.h>
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define REED_SWITCH_PIN 7     // GPIO pin for reed switch
#define DEBOUNCE_TIME_MS 50   // Debounce time
#define MESSAGE_DELAY_MS 5000 // Delay before sending message

void enter_deep_sleep()
{
    fflush(stdout);
    esp_sleep_enable_ext0_wakeup(REED_SWITCH_PIN, 1); // Wake up when magnet is removed (HIGH)
    esp_deep_sleep_start();
}

void app_main()
{
    // Configure reed switch pin
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << REED_SWITCH_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);

    printf("Starting reed switch monitoring...\n");

    while (1)
    {
        int sensor_state = gpio_get_level(REED_SWITCH_PIN);

        if (sensor_state == 0)
        {
            printf("Magnet detected! Going to sleep\n");
            enter_deep_sleep();
        }
        else if (sensor_state == 1)
        {
            printf("5 second delay before sending message\n");

            // Check the signal during the waiting period, whilst allowing for any interrupts from the sensor
            for (int i = 0; i < (MESSAGE_DELAY_MS / 500); i++)
            {
                vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500ms
                if (gpio_get_level(REED_SWITCH_PIN) == 0)
                {
                    printf("Signal changed back to 0, aborting message and going to sleep\n");
                    enter_deep_sleep();
                }
            }

            // If the signal stayed 1 throughout the delay, send the message
            printf("Sending message!\n");
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500ms
    }
}
