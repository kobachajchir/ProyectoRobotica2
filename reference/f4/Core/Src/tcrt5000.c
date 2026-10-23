#include "tcrt5000.h"



#include <stddef.h>
#include <string.h>

enum {
    TCRT5000_WHITE_MAX_RAW_LIGHTS_OFF = 700u,
    TCRT5000_BLACK_MIN_RAW_LIGHTS_OFF = 1500u,
    TCRT5000_SIDE_MARGIN_RAW_LIGHTS_OFF = 250u,
    TCRT5000_WHITE_MAX_RAW_LIGHTS_ON = 900u,
    TCRT5000_BLACK_MIN_RAW_LIGHTS_ON = 2400u,
    TCRT5000_SIDE_MARGIN_RAW_LIGHTS_ON = 250u,
};

AM_Result TCRT5000_Init(TCRT5000_Handle *h,const TCRT5000_Port *port)
{
    if (!h || !port || !port->set_emitters || !port->initialize) return AM_ERROR;
    memset(h,0,sizeof(*h)); h->port=*port;
    return h->port.initialize(h->port.ctx);
}
void TCRT5000_EmittersGroup5Set(TCRT5000_Handle *h,bool enabled) {
    h->port.set_emitters(h->port.ctx,5,enabled); h->emitters_group5_on=enabled;
}
void TCRT5000_EmittersGroup3Set(TCRT5000_Handle *h,bool enabled) {
    h->port.set_emitters(h->port.ctx,3,enabled); h->emitters_group3_on=enabled;
}
void TCRT5000_EmittersGroup5On(TCRT5000_Handle *h) { TCRT5000_EmittersGroup5Set(h,true); }
void TCRT5000_EmittersGroup5Off(TCRT5000_Handle *h) { TCRT5000_EmittersGroup5Set(h,false); }
void TCRT5000_EmittersGroup3On(TCRT5000_Handle *h) { TCRT5000_EmittersGroup3Set(h,true); }
void TCRT5000_EmittersGroup3Off(TCRT5000_Handle *h) { TCRT5000_EmittersGroup3Set(h,false); }
bool TCRT5000_EmittersGroup5IsOn(TCRT5000_Handle *h) { return h->emitters_group5_on; }
bool TCRT5000_EmittersGroup3IsOn(TCRT5000_Handle *h) { return h->emitters_group3_on; }

static const TCRT5000_SensorDef_t tcrt5000_sensors[TCRT5000_SENSOR_COUNT] = {
    [TCRT5000_SENSOR_PA1] = { TCRT5000_SENSOR_PA1, "PA1", TCRT5000_POLARITY_PULL_UP },
    [TCRT5000_SENSOR_PA2] = { TCRT5000_SENSOR_PA2, "PA2", TCRT5000_POLARITY_PULL_UP },
    [TCRT5000_SENSOR_PB1] = { TCRT5000_SENSOR_PB1, "PB1", TCRT5000_POLARITY_PULL_DOWN },
    [TCRT5000_SENSOR_PA0] = { TCRT5000_SENSOR_PA0, "PA0", TCRT5000_POLARITY_PULL_UP },
    [TCRT5000_SENSOR_PA4] = { TCRT5000_SENSOR_PA4, "PA4", TCRT5000_POLARITY_PULL_DOWN },
    [TCRT5000_SENSOR_PA5] = { TCRT5000_SENSOR_PA5, "PA5", TCRT5000_POLARITY_PULL_DOWN },
    [TCRT5000_SENSOR_PA6] = { TCRT5000_SENSOR_PA6, "PA6", TCRT5000_POLARITY_PULL_DOWN },
    [TCRT5000_SENSOR_PA7] = { TCRT5000_SENSOR_PA7, "PA7", TCRT5000_POLARITY_PULL_DOWN },
};

static const TCRT5000_LineSensor_t tcrt5000_route_order[TCRT5000_LINE_COUNT] = {
    TCRT5000_LINE_LEFT,
    TCRT5000_LINE_CENTER,
    TCRT5000_LINE_RIGHT,
};

static const TCRT5000_SensorId_t tcrt5000_line_sensor_ids[TCRT5000_LINE_COUNT] = {
    [TCRT5000_LINE_LEFT] = TCRT5000_SENSOR_PA0,
    [TCRT5000_LINE_CENTER] = TCRT5000_SENSOR_PA1,
    [TCRT5000_LINE_RIGHT] = TCRT5000_SENSOR_PA2,
};

static const float tcrt5000_line_positions_mm[TCRT5000_LINE_COUNT] = {
    [TCRT5000_LINE_LEFT] = -TCRT5000_LINE_SENSOR_SPACING_MM,
    [TCRT5000_LINE_CENTER] = 0.0f,
    [TCRT5000_LINE_RIGHT] = TCRT5000_LINE_SENSOR_SPACING_MM,
};

static const TCRT5000_SensorId_t tcrt5000_object_sensor_ids[TCRT5000_OBJECT_COUNT] = {
    [TCRT5000_OBJECT_LEFT_45] = TCRT5000_SENSOR_PA6,
    [TCRT5000_OBJECT_LEFT_CENTER] = TCRT5000_SENSOR_PA4,
    [TCRT5000_OBJECT_CENTER] = TCRT5000_SENSOR_PB1,
    [TCRT5000_OBJECT_RIGHT_CENTER] = TCRT5000_SENSOR_PA5,
    [TCRT5000_OBJECT_RIGHT_45] = TCRT5000_SENSOR_PA7,
};

static const TCRT5000_RouteProfile_t tcrt5000_route_profile_lights_off = {
    TCRT5000_WHITE_MAX_RAW_LIGHTS_OFF,
    TCRT5000_BLACK_MIN_RAW_LIGHTS_OFF,
    TCRT5000_SIDE_MARGIN_RAW_LIGHTS_OFF,
};

static const TCRT5000_RouteProfile_t tcrt5000_route_profile_lights_on = {
    TCRT5000_WHITE_MAX_RAW_LIGHTS_ON,
    TCRT5000_BLACK_MIN_RAW_LIGHTS_ON,
    TCRT5000_SIDE_MARGIN_RAW_LIGHTS_ON,
};

static const TCRT5000_SensorDef_t *TCRT5000_GetSensorDef(TCRT5000_SensorId_t sensorId)
{
    if ((uint8_t)sensorId >= TCRT5000_SENSOR_COUNT) {
        return NULL;
    }

    return &tcrt5000_sensors[(uint8_t)sensorId];
}

static TCRT5000_RouteAlignment_t TCRT5000_EvaluateRouteAlignment(const TCRT5000_RouteSample_t *sample,
                                                                 const TCRT5000_RouteProfile_t *profile)
{
    if (!sample || !profile) {
        return TCRT5000_ROUTE_SEARCH;
    }

    if (sample->centerBlack && sample->leftWhite && sample->rightWhite) {
        return TCRT5000_ROUTE_ALIGNMENT_OK;
    }
    if (sample->leftBlack && !sample->rightBlack) {
        return TCRT5000_ROUTE_ALIGN_LEFT;
    }
    if (sample->rightBlack && !sample->leftBlack) {
        return TCRT5000_ROUTE_ALIGN_RIGHT;
    }
    if (sample->leftBlack && sample->rightBlack) {
        return TCRT5000_ROUTE_WIDE;
    }
    if (!sample->centerBlack) {
        return TCRT5000_ROUTE_SEARCH;
    }
    if (sample->leftNorm > (uint16_t)(sample->rightNorm + profile->sideMarginRaw)) {
        return TCRT5000_ROUTE_ALIGN_LEFT;
    }
    if (sample->rightNorm > (uint16_t)(sample->leftNorm + profile->sideMarginRaw)) {
        return TCRT5000_ROUTE_ALIGN_RIGHT;
    }

    return TCRT5000_ROUTE_ADJUST_CENTER;
}

const TCRT5000_SensorDef_t *TCRT5000_GetLineSensorDef(TCRT5000_LineSensor_t sensor)
{
    if ((uint8_t)sensor >= TCRT5000_LINE_COUNT) {
        return NULL;
    }

    return TCRT5000_GetSensorDef(tcrt5000_line_sensor_ids[(uint8_t)sensor]);
}

const TCRT5000_SensorDef_t *TCRT5000_GetObjectSensorDef(TCRT5000_ObjectSensor_t sensor)
{
    if ((uint8_t)sensor >= TCRT5000_OBJECT_COUNT) {
        return NULL;
    }

    return TCRT5000_GetSensorDef(tcrt5000_object_sensor_ids[(uint8_t)sensor]);
}

const char *TCRT5000_GetSensorLabel(TCRT5000_SensorId_t sensorId)
{
    const TCRT5000_SensorDef_t *sensor = TCRT5000_GetSensorDef(sensorId);

    return sensor ? sensor->label : "";
}

uint16_t TCRT5000_GetRaw(const volatile uint16_t *sensorData,
                         const TCRT5000_SensorDef_t *sensor)
{
    if (!sensorData || !sensor || (uint8_t)sensor->sensorId >= TCRT5000_SENSOR_COUNT) {
        return 0u;
    }

    return sensorData[(uint8_t)sensor->sensorId];
}

uint16_t TCRT5000_GetRawById(const volatile uint16_t *sensorData,
                             TCRT5000_SensorId_t sensorId)
{
    return TCRT5000_GetRaw(sensorData, TCRT5000_GetSensorDef(sensorId));
}

uint16_t TCRT5000_NormalizeForDisplay(uint16_t raw, TCRT5000_Polarity_t polarity)
{
    uint16_t value = (raw > TCRT5000_ADC_MAX_VALUE) ? TCRT5000_ADC_MAX_VALUE : raw;

    if (polarity == TCRT5000_POLARITY_PULL_UP) {
        value = (uint16_t)(TCRT5000_ADC_MAX_VALUE - value);
    }

    return value;
}

const TCRT5000_RouteProfile_t *TCRT5000_GetRouteProfile(bool irLightsOn)
{
    return irLightsOn
        ? &tcrt5000_route_profile_lights_on
        : &tcrt5000_route_profile_lights_off;
}

bool TCRT5000_IsBlack(uint16_t raw, const TCRT5000_RouteProfile_t *profile)
{
    return profile && raw >= profile->blackMinRaw;
}

bool TCRT5000_IsWhite(uint16_t raw, const TCRT5000_RouteProfile_t *profile)
{
    return profile && raw <= profile->whiteMaxRaw;
}

static float TCRT5000_NormalizedDarkness(uint16_t normalized,
                                         const TCRT5000_RouteProfile_t *profile)
{
    uint16_t span;

    if (profile == NULL) {
        return 0.0f;
    }
    if (normalized <= profile->whiteMaxRaw) {
        return 0.0f;
    }
    if (normalized >= profile->blackMinRaw) {
        return 1.0f;
    }

    span = (uint16_t)(profile->blackMinRaw - profile->whiteMaxRaw);
    if (span == 0u) {
        return 0.0f;
    }

    return (float)(normalized - profile->whiteMaxRaw) / (float)span;
}

static void TCRT5000_UpdateLineGeometry(TCRT5000_LineConfig_t *config)
{
    uint16_t width;

    if (config == NULL) {
        return;
    }

    width = config->black_width_mm;
    if (width == 0u) {
        width = TCRT5000_LINE_BLACK_WIDTH_DEFAULT_MM;
    }

    config->black_width_mm = width;
    config->sensor_position_mm[TCRT5000_LINE_LEFT] =
        tcrt5000_line_positions_mm[TCRT5000_LINE_LEFT];
    config->sensor_position_mm[TCRT5000_LINE_CENTER] =
        tcrt5000_line_positions_mm[TCRT5000_LINE_CENTER];
    config->sensor_position_mm[TCRT5000_LINE_RIGHT] =
        tcrt5000_line_positions_mm[TCRT5000_LINE_RIGHT];
    config->centered_margin_mm =
        TCRT5000_LINE_SENSOR_SPACING_MM - ((float)width * 0.5f);
    if (config->centered_margin_mm < 0.0f) {
        config->centered_margin_mm = 0.0f;
    }
}

void TCRT5000_GetDefaultLineConfig(TCRT5000_LineConfig_t *config)
{
    if (config == NULL) {
        return;
    }

    memset(config, 0, sizeof(*config));
    config->black_width_mm = TCRT5000_LINE_BLACK_WIDTH_DEFAULT_MM;
    TCRT5000_UpdateLineGeometry(config);
}

void TCRT5000_SetLineBlackWidth(TCRT5000_LineConfig_t *config, uint16_t blackWidthMm)
{
    if (config == NULL) {
        return;
    }

    config->black_width_mm = blackWidthMm;
    TCRT5000_UpdateLineGeometry(config);
}

float TCRT5000_GetRouteSensorPositionMm(TCRT5000_LineSensor_t sensor)
{
    if ((uint8_t)sensor >= TCRT5000_LINE_COUNT) {
        return 0.0f;
    }

    return tcrt5000_line_positions_mm[(uint8_t)sensor];
}

void TCRT5000_ReadRouteSensorsEx(const volatile uint16_t *sensorData,
                                 bool irLightsOn,
                                 const TCRT5000_LineConfig_t *config,
                                 TCRT5000_RouteSample_t *sample)
{
    const TCRT5000_RouteProfile_t *profile = TCRT5000_GetRouteProfile(irLightsOn);
    TCRT5000_LineConfig_t default_config;
    const TCRT5000_LineConfig_t *line_config = config;
    const TCRT5000_SensorDef_t *left;
    const TCRT5000_SensorDef_t *center;
    const TCRT5000_SensorDef_t *right;
    float weighted_sum;
    float darkness_sum;

    if (!sample) {
        return;
    }
    memset(sample, 0, sizeof(*sample));

    if (line_config == NULL) {
        TCRT5000_GetDefaultLineConfig(&default_config);
        line_config = &default_config;
    }

    left = TCRT5000_GetLineSensorDef(tcrt5000_route_order[TCRT5000_LINE_LEFT]);
    center = TCRT5000_GetLineSensorDef(tcrt5000_route_order[TCRT5000_LINE_CENTER]);
    right = TCRT5000_GetLineSensorDef(tcrt5000_route_order[TCRT5000_LINE_RIGHT]);

    sample->leftRaw = TCRT5000_GetRaw(sensorData, left);
    sample->centerRaw = TCRT5000_GetRaw(sensorData, center);
    sample->rightRaw = TCRT5000_GetRaw(sensorData, right);

    sample->leftNorm = TCRT5000_NormalizeForDisplay(
        sample->leftRaw,
        left ? left->polarity : TCRT5000_POLARITY_PULL_DOWN);
    sample->centerNorm = TCRT5000_NormalizeForDisplay(
        sample->centerRaw,
        center ? center->polarity : TCRT5000_POLARITY_PULL_DOWN);
    sample->rightNorm = TCRT5000_NormalizeForDisplay(
        sample->rightRaw,
        right ? right->polarity : TCRT5000_POLARITY_PULL_DOWN);

    sample->leftDarkness = TCRT5000_NormalizedDarkness(sample->leftNorm, profile);
    sample->centerDarkness = TCRT5000_NormalizedDarkness(sample->centerNorm, profile);
    sample->rightDarkness = TCRT5000_NormalizedDarkness(sample->rightNorm, profile);

    sample->leftBlack = TCRT5000_IsBlack(sample->leftNorm, profile);
    sample->centerBlack = TCRT5000_IsBlack(sample->centerNorm, profile);
    sample->rightBlack = TCRT5000_IsBlack(sample->rightNorm, profile);
    sample->leftWhite = TCRT5000_IsWhite(sample->leftNorm, profile);
    sample->rightWhite = TCRT5000_IsWhite(sample->rightNorm, profile);
    sample->pattern = (sample->leftBlack ? TCRT5000_LINE_PATTERN_LEFT : 0u) |
                      (sample->centerBlack ? TCRT5000_LINE_PATTERN_CENTER : 0u) |
                      (sample->rightBlack ? TCRT5000_LINE_PATTERN_RIGHT : 0u);

    darkness_sum = sample->leftDarkness + sample->centerDarkness + sample->rightDarkness;
    if (darkness_sum > 0.01f) {
        weighted_sum =
            (sample->leftDarkness * line_config->sensor_position_mm[TCRT5000_LINE_LEFT]) +
            (sample->centerDarkness * line_config->sensor_position_mm[TCRT5000_LINE_CENTER]) +
            (sample->rightDarkness * line_config->sensor_position_mm[TCRT5000_LINE_RIGHT]);
        sample->lateralErrorMm = weighted_sum / darkness_sum;
        sample->lateralErrorNorm =
            sample->lateralErrorMm / TCRT5000_LINE_SENSOR_SPACING_MM;
        if (sample->lateralErrorNorm < -1.0f) {
            sample->lateralErrorNorm = -1.0f;
        } else if (sample->lateralErrorNorm > 1.0f) {
            sample->lateralErrorNorm = 1.0f;
        }
        sample->confidence = (uint8_t)(darkness_sum * 33.0f);
        if (sample->confidence > 100u) {
            sample->confidence = 100u;
        }
    }

    sample->lineWidthMm = line_config->black_width_mm;
    sample->centeredMarginMm = line_config->centered_margin_mm;
    sample->ambiguous = (sample->pattern == (TCRT5000_LINE_PATTERN_LEFT |
                                             TCRT5000_LINE_PATTERN_RIGHT));
    sample->alignment = TCRT5000_EvaluateRouteAlignment(sample, profile);
}

void TCRT5000_ReadRouteSensors(const volatile uint16_t *sensorData,
                               bool irLightsOn,
                               TCRT5000_RouteSample_t *sample)
{
    TCRT5000_ReadRouteSensorsEx(sensorData, irLightsOn, NULL, sample);
}

const char *TCRT5000_GetRouteAlignmentText(TCRT5000_RouteAlignment_t alignment)
{
    switch (alignment) {
        case TCRT5000_ROUTE_ALIGNMENT_OK:
            return "Alineacion OK";
        case TCRT5000_ROUTE_ALIGN_LEFT:
            return "Alinear izquierda";
        case TCRT5000_ROUTE_ALIGN_RIGHT:
            return "Alinear derecha";
        case TCRT5000_ROUTE_WIDE:
            return "Ruta ancha";
        case TCRT5000_ROUTE_SEARCH:
            return "Buscar camino";
        case TCRT5000_ROUTE_ADJUST_CENTER:
        default:
            return "Ajustar centro";
    }
}
