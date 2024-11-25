#include "cJSON.h"
#include "cJSON_Model.h"

char* create_json(jsonDataStruct dataStruct)
{   
    jsonDataStruct data_array[DATA_TYPE_COUNT] = {
        {25.5, TEMPERATURE},
        {300.0, LIGHT},
        {1.0, PUMP}
    };

    // Create a JSON object
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        printf("Failed to create JSON object\n");
        return;
    }

    cJSON_AddNumberToObject(root, dataStruct.type, dataStruct.value);
    

    // JSON Object to string
    char *json_str = cJSON_Print(root);
    if (json_str == NULL) {
        printf("Failed to print JSON object\n");
        cJSON_Delete(root);
        return;
    }

    printf("Generated JSON: %s\n", json_str);

    cJSON_Delete(root);
    free(json_str);
}