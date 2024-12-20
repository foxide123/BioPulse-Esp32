
#ifndef CJSON_SENSOR_MANAGER_H
#define CJSON_SENSOR_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON_Sensor_Model.h"
#include "esp_log.h"

#define CJSON_TAG "CJSON_Sensor_Manager"

char* create_sensor_response_json(jsonSensorResponse dataStruct);

#endif