#include <stdio.h>
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DIGITAL_PIN 25 // KY-025 D0 connected here

void app_main()
{
    // Configure digital pin
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << DIGITAL_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);

    int digital_value = gpio_get_level(DIGITAL_PIN);

    if (digital_value == 1)
    {
        ESP_LOGI("KY-025", "D0 = 1, entering deep sleep...");
        esp_sleep_enable_ext0_wakeup(DIGITAL_PIN, 0); // Wake up when D0 goes LOW (magnet detected)
        esp_deep_sleep_start();                       // Enter deep sleep
    }

    // If woken up, wait 5 seconds before printing the message
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Check if the signal is STILL low after the delay
    digital_value = gpio_get_level(DIGITAL_PIN);
    if (digital_value == 0)
    {
        ESP_LOGI("KY-025", "Magnet detected! Sending message...");
    }
    else
    {
        ESP_LOGI("KY-025", "D0 changed to 1 before delay, skipping message.");
    }

    // Go back to deep sleep
    ESP_LOGI("KY-025", "Going back to deep sleep...");
    esp_sleep_enable_ext0_wakeup(DIGITAL_PIN, 0);
    esp_deep_sleep_start();
}
