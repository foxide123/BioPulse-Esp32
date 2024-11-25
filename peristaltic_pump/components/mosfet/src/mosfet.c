#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Define the GPIO pin connected to the MOSFET gate
const int mosfetPin = 21; 

void mosfet_setup() {
    // Initialize the GPIO pin as an output
    gpio_set_direction(mosfetPin, GPIO_MODE_OUTPUT);
    // Ensure the MOSFET is turned off initially
    gpio_set_level(mosfetPin, 0);
}

void mosfet_turn_on(){
    gpio_set_level(mosfetPin, 1);
    ESP_LOGI("MOSFET", "MOSFET turned ON");
}

void mosfet_turn_off(){
    gpio_set_level(mosfetPin, 0);
    ESP_LOGI("MOSFET", "MOSFET turned OFF");
}