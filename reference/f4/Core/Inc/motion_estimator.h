#ifndef INC_MOTION_ESTIMATOR_H_
#define INC_MOTION_ESTIMATOR_H_
#include "motion_estimator_types.h"
#ifdef __cplusplus
extern "C" {
#endif
void MotionEstimator_GetDefaultConfig(MotionEstimator_Config_t *config);
AM_Result MotionEstimator_Init(MotionEstimator_Handle *estimator,
                                       const MotionEstimator_Config_t *config);
void MotionEstimator_Reset(MotionEstimator_Handle *estimator);
void MotionEstimator_MarkExternalReference(MotionEstimator_Handle *estimator,
                                           float velocity_mps,
                                           uint32_t now_ms);
void MotionEstimator_Update(MotionEstimator_Handle *estimator,
                            uint32_t now_ms,
                            uint8_t motors_active,
                            uint8_t control_saturated);
const MotionEstimator_Status_t *MotionEstimator_GetStatus(const MotionEstimator_Handle *estimator);

#ifdef __cplusplus
}
#endif

#endif /* INC_MOTION_ESTIMATOR_H_ */
