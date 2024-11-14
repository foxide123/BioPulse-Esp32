#ifdef CONFIG_IDF_TARGET_LINUX

#define ONE_WIRE_PIN 0 

#else

#include "driver/gpio.h" 
#define ONE_WIRE_PIN GPIO_NUM_16

#endif

#include <stdint.h>
#include <stdbool.h>

bool one_wire_reset(void);
void one_wire_write_bit(bool bit);
bool one_wire_read_bit(void);
void one_wire_write_byte(uint8_t data);
uint8_t one_wire_read_byte(void);
