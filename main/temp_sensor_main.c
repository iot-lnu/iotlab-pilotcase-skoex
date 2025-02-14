#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Define sensor pins
#define DIGITAL_PIN GPIO_NUM_26   // Digital output (DO)
#define ANALOG_PIN ADC1_CHANNEL_6 // Analog output (AO) on GPIO34

void app_main(void)
{
    ESP_LOGI("HALL SENSOR", "Starting Sensor Readout...");

    // Configure Digital Input
    gpio_reset_pin(DIGITAL_PIN);
    gpio_set_direction(DIGITAL_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(DIGITAL_PIN, GPIO_PULLUP_ONLY); // Enable pull-up resistor

    // Configure Analog Input
    adc1_config_width(ADC_WIDTH_BIT_12);                    // 12-bit ADC resolution (0-4095)
    adc1_config_channel_atten(ANALOG_PIN, ADC_ATTEN_DB_11); // Full-range voltage

    while (1)
    {
        // Read values
        int digitalVal = gpio_get_level(DIGITAL_PIN);
        int analogVal = adc1_get_raw(ANALOG_PIN);

        // Print results
        ESP_LOGI("HALL SENSOR", "Digital: %d | Analog: %d", digitalVal, analogVal);

        // Small delay before next reading
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
