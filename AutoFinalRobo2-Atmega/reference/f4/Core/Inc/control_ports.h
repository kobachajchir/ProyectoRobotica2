#ifndef CONTROL_PORTS_H
#define CONTROL_PORTS_H
#include "pid_controller_data.h"
#include "mpu9250_data.h"
#include "balance_control_data.h"
#include "motion_control_data.h"
#include "motion_estimator_data.h"
#include "tcrt5000_data.h"
typedef struct AM_PidOps {
    void (*defaults)(PidController_Config_t *);
    AM_Result (*init)(void *,const PidController_Config_t *);
    void (*reset)(void *);
    void (*setpoint)(void *,float);
    void (*gains)(void *,float,float,float);
    AM_Result (*update)(void *,float,float,float);
    float (*output)(const void *);
    const PidController_Status_t *(*status)(const void *);
} AM_PidOps;
typedef struct AM_OrientationOps {
    bool (*ready)(const void *);
    uint8_t (*calibrating)(const void *);
    uint8_t (*fresh)(const void *,uint32_t);
    float (*front_tilt)(const void *);
    uint16_t (*sequence)(const void *);
    uint32_t (*attitude_ms)(const void *);
    AM_Result (*snapshot)(void *,MPU9250_Snapshot_t *);
    uint32_t (*now_ms)(const void *);
} AM_OrientationOps;
typedef struct AM_BalanceOps {
    void (*target)(void *,float);
    void (*enable)(void *,uint8_t);
    void (*reset)(void *);
    AM_Result (*update)(void *,float,uint32_t);
    float (*output)(const void *);
} AM_BalanceOps;
typedef struct AM_DriveOps {
    void (*stop)(void *);
    void (*brake)(void *);
    void (*apply)(void *,float,float);
} AM_DriveOps;
typedef struct AM_LineOps {
    void (*command_defaults)(MotionControl_Command_t *);
    AM_Result (*command)(void *,const MotionControl_Command_t *,uint32_t);
    uint8_t (*armed)(const void *);
    const MotionEstimator_Status_t *(*estimate)(const void *);
    void (*line_defaults)(TCRT5000_LineConfig_t *);
    void (*line_width)(TCRT5000_LineConfig_t *,uint16_t);
    bool (*emitters_on)(void *);
    void (*read_line)(const volatile uint16_t *,bool,const TCRT5000_LineConfig_t *,TCRT5000_RouteSample_t *);
} AM_LineOps;
#endif
