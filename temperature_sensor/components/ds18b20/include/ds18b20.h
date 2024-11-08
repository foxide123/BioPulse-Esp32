#define ONE_WIRE_PIN GPIO_NUM_16

bool one_wire_reset(void);
void one_wire_write_bit(bool bit);
bool one_wire_read_bit(void);
void one_wire_write_byte(uint8_t data);
uint8_t one_wire_read_byte(void);