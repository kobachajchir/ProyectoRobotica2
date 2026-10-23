#ifndef TCRT5000_DATA_H
#define TCRT5000_DATA_H
#include "am_port.h"
#define TCRT5000_SENSOR_COUNT     8u
#define TCRT5000_ADC_MAX_VALUE    4095u
#define TCRT5000_LINE_SENSOR_SPACING_MM 16.0f
#define TCRT5000_LINE_SENSOR_SPAN_MM    32.0f
#define TCRT5000_LINE_BLACK_WIDTH_DEFAULT_MM 18u
#define TCRT5000_LINE_PATTERN_LEFT      0x04u
#define TCRT5000_LINE_PATTERN_CENTER    0x02u
#define TCRT5000_LINE_PATTERN_RIGHT     0x01u
#define TCRT5000_LINE_PATTERN_CENTERED  TCRT5000_LINE_PATTERN_CENTER
typedef enum {
    TCRT5000_POLARITY_PULL_DOWN = 0u,
    TCRT5000_POLARITY_PULL_UP,
} TCRT5000_Polarity_t;

typedef enum {
    TCRT5000_SENSOR_PA1 = 0u,
    TCRT5000_SENSOR_PA2 = 1u,
    TCRT5000_SENSOR_PB1 = 2u,
    TCRT5000_SENSOR_PA0 = 3u,
    TCRT5000_SENSOR_PA4 = 4u,
    TCRT5000_SENSOR_PA5 = 5u,
    TCRT5000_SENSOR_PA6 = 6u,
    TCRT5000_SENSOR_PA7 = 7u,
} TCRT5000_SensorId_t;

typedef enum {
    TCRT5000_LINE_LEFT = 0u,
    TCRT5000_LINE_CENTER,
    TCRT5000_LINE_RIGHT,
    TCRT5000_LINE_COUNT,
} TCRT5000_LineSensor_t;

typedef enum {
    TCRT5000_OBJECT_LEFT_45 = 0u,
    TCRT5000_OBJECT_LEFT_CENTER,
    TCRT5000_OBJECT_CENTER,
    TCRT5000_OBJECT_RIGHT_CENTER,
    TCRT5000_OBJECT_RIGHT_45,
    TCRT5000_OBJECT_COUNT,
} TCRT5000_ObjectSensor_t;

typedef enum {
    TCRT5000_ROUTE_ALIGNMENT_OK = 0u,
    TCRT5000_ROUTE_ALIGN_LEFT,
    TCRT5000_ROUTE_ALIGN_RIGHT,
    TCRT5000_ROUTE_WIDE,
    TCRT5000_ROUTE_SEARCH,
    TCRT5000_ROUTE_ADJUST_CENTER,
} TCRT5000_RouteAlignment_t;

typedef struct {
    TCRT5000_SensorId_t sensorId;
    const char *label;
    TCRT5000_Polarity_t polarity;
} TCRT5000_SensorDef_t;

typedef struct {
    uint16_t whiteMaxRaw;
    uint16_t blackMinRaw;
    uint16_t sideMarginRaw;
} TCRT5000_RouteProfile_t;

typedef struct {
    uint16_t black_width_mm;
    float sensor_position_mm[TCRT5000_LINE_COUNT];
    float centered_margin_mm;
} TCRT5000_LineConfig_t;

typedef struct {
    uint16_t leftRaw;
    uint16_t centerRaw;
    uint16_t rightRaw;
    uint16_t leftNorm;
    uint16_t centerNorm;
    uint16_t rightNorm;
    float leftDarkness;
    float centerDarkness;
    float rightDarkness;
    float lateralErrorMm;
    float lateralErrorNorm;
    float centeredMarginMm;
    uint16_t lineWidthMm;
    uint8_t pattern;
    uint8_t confidence;
    bool ambiguous;
    bool leftBlack;
    bool centerBlack;
    bool rightBlack;
    bool leftWhite;
    bool rightWhite;
    TCRT5000_RouteAlignment_t alignment;
} TCRT5000_RouteSample_t;


#endif
