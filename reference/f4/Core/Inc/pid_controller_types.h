/*
 * pid_controller.h
 *
 * Reusable PID core for fixed-rate embedded control loops. The implementation
 * uses caller-owned storage only and is safe for STM32 HAL projects without
 * dynamic allocation.
 */

#ifndef INC_PID_CONTROLLER_H_DATA
#define INC_PID_CONTROLLER_H_DATA

#include "am_port.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "pid_controller_data.h"
typedef struct {
    PidController_Config_t config;
    PidController_Status_t status;
} PidController_Handle;


#ifdef __cplusplus
}
#endif
#endif
