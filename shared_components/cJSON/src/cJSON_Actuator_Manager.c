#include "cJSON_Actuator_Manager.h"

char* create_actuator_request_json(jsonActuatorRequest dataStruct)
{   
    // Create a new JSON object
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        printf("Failed to create JSON object\n");
        return NULL;
    }

    // "actuator_id" field
    if(!cJSON_AddStringToObject(root, "actuator_id", dataStruct.actuator_id)) {
        ESP_LOGE(CJSON_TAG, "Failed to add 'id' to JSON");
        cJSON_Delete(root);
        return NULL;
    }

    // "action_type" field
    if(!cJSON_AddStringToObject(root, "action_type", action_types[dataStruct.action])) {
        ESP_LOGE(CJSON_TAG, "Failed to add 'Temp' to JSON");
        cJSON_Delete(root);
        return NULL;
    }

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