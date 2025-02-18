#include <stdio.h>
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DIGITAL_PIN 25             // KY-025 D0 connected here
static const char *TAG = "KY-025"; // ESP_LOG tag for better debugging

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

    ESP_LOGI(TAG, "Actual digit: %d", digital_value); // Log the initial value

    // Go to deep sleep if NO magnet is detected (D0 = 0)
    if (digital_value == 0)
    {
        ESP_LOGI(TAG, "No magnet detected (D0 = 0), entering deep sleep...");
        esp_sleep_enable_ext0_wakeup(DIGITAL_PIN, 1); // Wake up when D0 goes HIGH (magnet appears)
        esp_deep_sleep_start();                       // Enter deep sleep
    }

    // If woken up, wait 5 seconds before checking again
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Re-check the sensor value
    digital_value = gpio_get_level(DIGITAL_PIN);
    ESP_LOGI(TAG, "Actual digit after delay: %d", digital_value); // Log updated value

    if (digital_value == 1)
    {
        ESP_LOGI(TAG, "Magnet detected! Sending message...");
    }
    else
    {
        ESP_LOGI(TAG, "D0 changed to 0 before delay, skipping message.");
    }

    // Go back to deep sleep if magnet is gone
    ESP_LOGI(TAG, "Rechecking... Going back to deep sleep if no magnet...");
    if (digital_value == 0)
    {
        esp_sleep_enable_ext0_wakeup(DIGITAL_PIN, 1);
        esp_deep_sleep_start();
    }
}
