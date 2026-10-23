#ifndef INC_TCRT5000_H_
#define INC_TCRT5000_H_
#include "tcrt5000_types.h"
#ifdef __cplusplus
extern "C" {
#endif
AM_Result TCRT5000_Init(TCRT5000_Handle *h, const TCRT5000_Port *port);

/* Semantic control: the electrical polarity is private to tcrt5000.c. */
void TCRT5000_EmittersGroup5On(TCRT5000_Handle *h);
void TCRT5000_EmittersGroup5Off(TCRT5000_Handle *h);
void TCRT5000_EmittersGroup5Set(TCRT5000_Handle *h, bool enabled);
bool TCRT5000_EmittersGroup5IsOn(TCRT5000_Handle *h);
void TCRT5000_EmittersGroup3On(TCRT5000_Handle *h);
void TCRT5000_EmittersGroup3Off(TCRT5000_Handle *h);
void TCRT5000_EmittersGroup3Set(TCRT5000_Handle *h, bool enabled);
bool TCRT5000_EmittersGroup3IsOn(TCRT5000_Handle *h);

#include "tcrt5000_data.h"
const TCRT5000_SensorDef_t *TCRT5000_GetLineSensorDef(TCRT5000_LineSensor_t sensor);
const TCRT5000_SensorDef_t *TCRT5000_GetObjectSensorDef(TCRT5000_ObjectSensor_t sensor);
const char *TCRT5000_GetSensorLabel(TCRT5000_SensorId_t sensorId);
uint16_t TCRT5000_GetRaw(const volatile uint16_t *sensorData,
                         const TCRT5000_SensorDef_t *sensor);
uint16_t TCRT5000_GetRawById(const volatile uint16_t *sensorData,
                             TCRT5000_SensorId_t sensorId);
uint16_t TCRT5000_NormalizeForDisplay(uint16_t raw, TCRT5000_Polarity_t polarity);
const TCRT5000_RouteProfile_t *TCRT5000_GetRouteProfile(bool irLightsOn);
bool TCRT5000_IsBlack(uint16_t raw, const TCRT5000_RouteProfile_t *profile);
bool TCRT5000_IsWhite(uint16_t raw, const TCRT5000_RouteProfile_t *profile);
void TCRT5000_GetDefaultLineConfig(TCRT5000_LineConfig_t *config);
void TCRT5000_SetLineBlackWidth(TCRT5000_LineConfig_t *config, uint16_t blackWidthMm);
float TCRT5000_GetRouteSensorPositionMm(TCRT5000_LineSensor_t sensor);
void TCRT5000_ReadRouteSensors(const volatile uint16_t *sensorData,
                               bool irLightsOn,
                               TCRT5000_RouteSample_t *sample);
void TCRT5000_ReadRouteSensorsEx(const volatile uint16_t *sensorData,
                                 bool irLightsOn,
                                 const TCRT5000_LineConfig_t *config,
                                 TCRT5000_RouteSample_t *sample);
const char *TCRT5000_GetRouteAlignmentText(TCRT5000_RouteAlignment_t alignment);

#ifdef __cplusplus
}
#endif

#endif /* INC_TCRT5000_H_ */
