#ifndef INC_BALANCE_CONTROL_H_
#define INC_BALANCE_CONTROL_H_
#include "balance_control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
void BalanceControl_GetDefaultConfig(BalanceControl_Config_t *config);
AM_Result BalanceControl_Init(BalanceControl_Handle *balance,
                                      const BalanceControl_Config_t *config);
void BalanceControl_Reset(BalanceControl_Handle *balance);
void BalanceControl_SetEnabled(BalanceControl_Handle *balance, uint8_t enabled);
void BalanceControl_SetTargetPitchDeg(BalanceControl_Handle *balance,
                                      float target_pitch_deg);
void BalanceControl_SetPid(BalanceControl_Handle *balance,
                           float kp,
                           float ki,
                           float kd);
AM_Result BalanceControl_Update(BalanceControl_Handle *balance,
                                        float measured_pitch_deg,
                                        uint32_t now_ms);
float BalanceControl_GetOutputPercent(const BalanceControl_Handle *balance);
const BalanceControl_Status_t *BalanceControl_GetStatus(const BalanceControl_Handle *balance);

#ifdef __cplusplus
}
#endif

#endif /* INC_BALANCE_CONTROL_H_ */
