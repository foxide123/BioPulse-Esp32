#ifdef CONFIG_IDF_TARGET_LINUX

#else

#include "ds18b20.h"

bool one_wire_reset(gpio_num_t one_wire_pin){
    bool isPresent = false;

    gpio_set_direction(one_wire_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(one_wire_pin, 0);
    esp_rom_delay_us(480);

    gpio_set_direction(one_wire_pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(70);

    isPresent = !gpio_get_level(one_wire_pin);
    esp_rom_delay_us(410); // wait for remaining 410 (410 + 70 = 480 for RX)

    return isPresent;
}

void one_wire_write_bit(gpio_num_t one_wire_pin, bool bit){
    gpio_set_direction(one_wire_pin, GPIO_MODE_OUTPUT);

    if(bit){
        gpio_set_level(one_wire_pin, 0);
        esp_rom_delay_us(10);
        gpio_set_level(one_wire_pin, 1);
        esp_rom_delay_us(55);
    } else {
        gpio_set_level(one_wire_pin, 0);
        esp_rom_delay_us(65);
        gpio_set_level(one_wire_pin, 1);
    }

    esp_rom_delay_us(5);
}

bool one_wire_read_bit(gpio_num_t one_wire_pin)
{
    bool bit;

    gpio_set_direction(one_wire_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(one_wire_pin, 0);
    esp_rom_delay_us(3);

    gpio_set_direction(one_wire_pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(10);
    
    bit = gpio_get_level(one_wire_pin);
    esp_rom_delay_us(50);

    return bit;
}

void one_wire_write_byte(gpio_num_t one_wire_pin, uint8_t data){
    for(int i=0; i<8; i++){
        one_wire_write_bit(one_wire_pin, data & 0x01);
        data >>= 1;
    }
}

uint8_t one_wire_read_byte(gpio_num_t one_wire_pin){
    uint8_t data = 0;

    for (int i=0; i<8; i++){
        data >>= 1;
        if(one_wire_read_bit(one_wire_pin)){
            data |= 0x80;
        }
    }
    return data;
}
#endif