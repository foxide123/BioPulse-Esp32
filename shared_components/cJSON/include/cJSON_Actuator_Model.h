#ifndef CJSON_ACTUATOR_MODEL_H
#define CJSON_ACTUATOR_MODEL_H

typedef enum{
    TURN_ON = 0,
    TURN_OFF,
    DATA_TYPE_COUNT
} actionType;

static const char * const action_types[DATA_TYPE_COUNT] = {
    [TURN_ON] = "TURN_ON",
    [TURN_OFF] = "TURN_OFF"
};

typedef struct{
  char* actuator_id;
  actionType action;
} jsonActuatorRequest;

#endif