/*
 * motion_estimator.h
 *
 * Short-term motion estimator for the balancing robot without wheel encoders.
 * It estimates relative longitudinal velocity from MPU9250 linear acceleration;
 * it is not absolute odometry.
 */

#ifndef INC_MOTION_ESTIMATOR_H_DATA
#define INC_MOTION_ESTIMATOR_H_DATA

#include "control_ports.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "motion_estimator_data.h"
typedef struct {
    MotionEstimator_Config_t config;
    MotionEstimator_Status_t status;
    uint16_t last_seq;
    uint8_t initialized;
} MotionEstimator_Handle;


#ifdef __cplusplus
}
#endif
#endif
