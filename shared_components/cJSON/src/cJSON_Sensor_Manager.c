#include "cJSON_Sensor_Manager.h"

char* create_sensor_response_json(jsonSensorResponse dataStruct)
{   
    /*
    jsonDataStruct data_array[DATA_TYPE_COUNT] = {
        {25.5, TEMPERATURE},
        {300.0, LIGHT},
        {1.0, PUMP}
    };
    */

    // Create a new JSON object
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        printf("Failed to create JSON object\n");
        return NULL;
    }

    // "id" field
    if(!cJSON_AddStringToObject(root, "id", dataStruct.id)) {
        ESP_LOGE(CJSON_TAG, "Failed to add 'id' to JSON");
        cJSON_Delete(root);
        return NULL;
    }

    // "sensor_name" field
    if(!cJSON_AddStringToObject(root, "sensor_name", dataStruct.sensor_name)) {
        ESP_LOGE(CJSON_TAG, "Failed to add 'sensor_name' to JSON");
        cJSON_Delete(root);
        return NULL;
    }

    // "type" field
    if(!cJSON_AddStringToObject(root, "type", data_types[dataStruct.type])) {
        ESP_LOGE(CJSON_TAG, "Failed to add 'type' to JSON");
        cJSON_Delete(root);
        return NULL;
    }

    // "temp" field
    if(!cJSON_AddNumberToObject(root, data_types[TEMPERATURE], dataStruct.value)) {
        ESP_LOGE(CJSON_TAG, "Failed to add 'Temp' to JSON");
        cJSON_Delete(root);
        return NULL;
    }

   // cJSON_AddNumberToObject(root, data_types[dataStruct.type], dataStruct.value);
    

    // JSON Object to string
    char *json_str = cJSON_Print(root);
    if (json_str == NULL) {
        printf("Failed to print JSON object\n");
        cJSON_Delete(root);
        return NULL;
    }

    printf("%s\n", json_str);

    cJSON_Delete(root);
    
    return json_str;
}