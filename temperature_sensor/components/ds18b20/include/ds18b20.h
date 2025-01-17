#ifdef CONFIG_IDF_TARGET_LINUX

#define ONE_WIRE_PIN 0 

#endif

#include "driver/gpio.h" 
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"


bool one_wire_reset(gpio_num_t one_wire_pin);
void one_wire_write_bit(gpio_num_t one_wire_pin, bool bit);
bool one_wire_read_bit(gpio_num_t one_wire_pin);
void one_wire_write_byte(gpio_num_t one_wire_pin, uint8_t data);
uint8_t one_wire_read_byte(gpio_num_t one_wire_pin);