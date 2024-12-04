#ifndef CONFIG_IDF_TARGET_LINUX

#include "unity.h"
#include "i2c_slave.h"
#include "esp_log.h"
#include "mocks/i2c_mock.h"

DEFINE_FFF_GLOBALS;

void setUp(void) {
    FFF_RESET_HISTORY();
    RESET_FAKE(i2c_slave_transmit);
    RESET_FAKE(i2c_new_slave_device);
}

TEST_CASE("i2c slave should write with success", "[i2c_unit_tests]") {

    // Arrange
    i2c_new_slave_device_fake.return_val = ESP_OK;
    i2c_slave_transmit_fake.return_val = ESP_OK;

    // Act
    i2c_slave_write_task(NULL);

    // Assert
    TEST_ASSERT_EQUAL(1, i2c_new_slave_device_fake.call_count);
    TEST_ASSERT_EQUAL(1, i2c_slave_transmit_fake.call_count);
    TEST_ASSERT_EQUAL(TEST_I2C_PORT, i2c_slave_transmit_fake.arg0_val);
    TEST_ASSERT_EQUAL(DATA_LENGTH, i2c_slave_transmit_fake.arg2_val);
   // TEST_ASSERT_EQUAL(1, vTaskDelay_fake.call_count);
}
/*
TEST_CASE("i2c slave should write with success", "[i2c_unit_tests]") {
    // Arrange
    i2c_new_slave_device_fake.return_val = ESP_OK;
    i2c_slave_transmit_fake.return_val = ESP_OK;

    // Act
    i2c_slave_write_task(NULL);

    // Assert
    TEST_ASSERT_EQUAL(1, i2c_new_slave_device_fake.call_count);
    TEST_ASSERT_EQUAL(1, i2c_slave_transmit_fake.call_count);
    TEST_ASSERT_EQUAL(TEST_I2C_PORT, i2c_slave_transmit_fake.arg0_val);
    TEST_ASSERT_EQUAL(DATA_LENGTH, i2c_slave_transmit_fake.arg2_val);
    TEST_ASSERT_EQUAL(1, vTaskDelay_fake.call_count);
}
*/

#endif


