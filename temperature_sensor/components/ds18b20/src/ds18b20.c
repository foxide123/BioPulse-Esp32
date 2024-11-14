#ifdef CONFIG_IDF_TARGET_LINUX

#else

#include "ds18b20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

bool one_wire_reset(void){
    bool isPresent = false;

    gpio_set_direction(ONE_WIRE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(ONE_WIRE_PIN, 0);
    esp_rom_delay_us(480);

    gpio_set_direction(ONE_WIRE_PIN, GPIO_MODE_INPUT);
    esp_rom_delay_us(70);

    isPresent = !gpio_get_level(ONE_WIRE_PIN);
    esp_rom_delay_us(410);

    return isPresent;
}

void one_wire_write_bit(bool bit){
    gpio_set_direction(ONE_WIRE_PIN, GPIO_MODE_OUTPUT);

    if(bit){
        gpio_set_level(ONE_WIRE_PIN, 0);
        esp_rom_delay_us(10);
        gpio_set_level(ONE_WIRE_PIN, 1);
        esp_rom_delay_us(55);
    } else {
        gpio_set_level(ONE_WIRE_PIN, 0);
        esp_rom_delay_us(65);
        gpio_set_level(ONE_WIRE_PIN, 1);
    }

    esp_rom_delay_us(5);
}

bool one_wire_read_bit(void)
{
    bool bit;

    gpio_set_direction(ONE_WIRE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(ONE_WIRE_PIN, 0);
    esp_rom_delay_us(3);

    gpio_set_direction(ONE_WIRE_PIN, GPIO_MODE_INPUT);
    esp_rom_delay_us(10);
    
    bit = gpio_get_level(ONE_WIRE_PIN);
    esp_rom_delay_us(50);

    return bit;
}

void one_wire_write_byte(uint8_t data){
    for(int i=0; i<8; i++){
        one_wire_write_bit(data & 0x01);
        data >>= 1;
    }
}

uint8_t one_wire_read_byte(void){
    uint8_t data = 0;

    for (int i=0; i<8; i++){
        data >>= 1;
        if(one_wire_read_bit()){
            data |= 0x80;
        }
    }
    return data;
}
#endif