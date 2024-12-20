#ifndef CJSON_ACTUATOR_MANAGER_H
#define CJSON_ACTUATOR_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON_Actuator_Model.h"
#include "esp_log.h"

#define CJSON_TAG "CJSON_Manager"

char* create_actuator_request_json(jsonActuatorRequest dataStruct);

#endif