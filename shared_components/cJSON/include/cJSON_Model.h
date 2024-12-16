#ifndef CJSON_MODEL_H
#define CJSON_MODEL_H

typedef enum{
    TEMPERATURE = 0,
    LIGHT,
    PUMP,
    PH,
    EC,
    WATERLEVEL,
    DATA_TYPE_COUNT
} dataType;

static const char * const data_types[DATA_TYPE_COUNT] = {
    [TEMPERATURE] = "Temp",
    [LIGHT] = "Light",
    [PUMP] = "Pump",
    [PH] = "pH",
    [EC] = "EC",
    [WATERLEVEL] = "WaterLevel"
};


typedef struct{
  char* id;
  dataType type;
  char* sensor_name;
  double value;

} jsonDataStruct;

#endif