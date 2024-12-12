#ifndef CJSON_MODEL_H
#define CJSON_MODEL_H

typedef enum{
    TEMPERATURE = 0,
    LIGHT,
    PUMP,
    DATA_TYPE_COUNT
} dataType;

static const char * const data_types[DATA_TYPE_COUNT] = {
    [TEMPERATURE] = "Temperature",
    [LIGHT] = "Light",
    [PUMP] = "Pump",
};


typedef struct{
  double value;
  dataType type;
} jsonDataStruct;

#endif