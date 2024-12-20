#include "unity.h"
#include "Mockgpio.h"
#include "one_wire.h"

void setUp(void)
{
    Mockgpio_Init();
}

void tearDown(void)
{
    Mockgpio_Verify();
    Mockgpio_Destroy();
}

// Stub for esp_rom_delay_us
void esp_rom_delay_us(uint32_t us)
{
}

bool test_one_wire_reset_device(bool isPresent)
{
    #ifndef ONE_WIRE_PIN
    #define ONE_WIRE_PIN GPIO_NUM_4
    #endif

    uint8_t gpioLevel;
    gpioLevel = isPresent ? 1 : 0;

    gpio_set_direction_ExpectAndReturn(ONE_WIRE_PIN, GPIO_MODE_OUTPUT, ESP_OK);
    gpio_set_level_ExpectAndReturn(ONE_WIRE_PIN, 0, ESP_OK);
    esp_rom_delay_us_Expect(480);

    gpio_set_direction_ExpectAndReturn(ONE_WIRE_PIN, GPIO_MODE_INPUT, ESP_OK);
    esp_rom_delay_us_Expect(70);

    gpio_get_level_ExpectAndReturn(ONE_WIRE_PIN, gpioLevel);
    esp_rom_delay_us_Expect(410);

    return one_wire_reset();
}

void expect_one_wire_write_bit(bool bit)
{
    #ifndef ONE_WIRE_PIN
    #define ONE_WIRE_PIN GPIO_NUM_4
    #endif

    gpio_set_direction_ExpectAndReturn(ONE_WIRE_PIN, GPIO_MODE_OUTPUT, ESP_OK);
    gpio_set_level_ExpectAndReturn(ONE_WIRE_PIN, 0, ESP_OK);

    if(bit)
    {
        esp_rom_delay_us_Expect(10);
        gpio_set_level_ExpectAndReturn(ONE_WIRE_PIN, 1, ESP_OK);
        esp_rom_delay_us_Expect(55);
    }else
    {
        esp_rom_delay_us_Expect(65);
        gpio_set_level_ExpectAndReturn(ONE_WIRE_PIN, 1, ESP_OK);
    }

    esp_rom_delay_us_Expect(5);
}

void test_one_wire_write_bit_0(void)
{
    test_one_wire_write_bit(false);
    one_wire_write_bit(false);
}

void test_one_wire_write_bit_1(void)
{
    test_one_wire_write_bit(true);
    one_wire_write_bit(true);
}


void one_wire_read_bit(void)
{
    #ifndef ONE_WIRE_PIN
    #define ONE_WIRE_PIN GPIO_NUM_4
    #endif

    gpio_set_direction_ExpectAndReturn(ONE_WIRE_PIN, GPIO_MODE_OUTPUT, ESP_OK);
    gpio_set_direction_ExpectAndReturn(ONE_WIRE_PIN, 0, ESP_OK);
    gpio_set_level_ExpectAndReturn(ONE_WIRE_PIN, 0, ESP_OK);
    esp_rom_delay_us_Expect(3);

    gpio_set_direction_ExpectAndReturn(ONE_WIRE_PIN, GPIO_MODE_INPUT);
    esp_rom_delay_us_Expect(10);

    gpio_get_level_ExpectAndReturn(0);
    esp_rom_delay_us_Expect(50);

    bool bit = one_wire_read_bit();
    TEST_ASSERT_TRUE(bit);
}



void test_one_wire_write_byte(void)
{
    uint8_t data = 0xAA; //10101010

    for(int i = 0; i < 8; i++)
    {
        bool bit = data & 0x01;
        expect_one_wire_write_bit(bit);
        data >>= 1;
    }

    one_wire_write_byte(0xAA);
}

void expect_one_wire_read_bit_sequence(bool bits[8])
{
    for(int i=0; i<8; i++)
    {
        one_wire_read_bit_ExpectAndReturn(bits[i]);
    }
}

void test_one_wire_read_byte_returns_corect_byte(void)
{
    // ARRANGE
    bool bits_sequence_AA[8] = {false, true, false, true, false, true, false, true}; //from LSB to MSB
    uint8_t expected_byte_AA = 0xAA; // 10101010
    
    // ACT
    expect_one_wire_read_bit_sequence(bits_sequence_AA);
    uint8_t result_AA = one_wire_read_byte();

    // ASSERT
    TEST_ASSERT_EQUAL_UINT8(expected_byte_AA, result_AA);
}


void test_one_wire_reset_device_present(void)
{
    bool isPresent = false;

    isPresent = test_one_wire_reset_device(true);

    TEST_ASSERT_TRUE(isPresent);
}

void test_one_wire_reset_device_absent(void)
{

    bool isPresent = true;

    isPresent = test_one_wire_reset_device(false);

    TEST_ASSERT_FALSE(isPresent);
}

TEST_CASE("Should_Return_True_When_reset_device_called_with_true", "[temperature_tests]")
{
    test_one_wire_reset_device_present();
}

TEST_CASE("Should_Return_True_When_reset_device_called_with_false", "[temperature_tests]")
{
    test_one_wire_reset_device_absent();
}


