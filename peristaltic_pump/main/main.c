#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mosfet.h"

void app_main(void)
{
    mosfet_setup();

    while(1){
        mosfet_turn_on();
        vTaskDelay(pdMS_TO_TICKS(1000));

        mosfet_turn_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
