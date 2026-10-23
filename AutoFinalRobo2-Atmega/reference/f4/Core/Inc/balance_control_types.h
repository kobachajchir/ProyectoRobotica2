/*
 * balance_control.h
 *
 * Pitch balance PID layer. It consumes an already-filtered pitch angle and
 * produces a signed motor effort percentage. It does not know about motors,
 * Web, nRF, menus or the MPU transport.
 */

#ifndef INC_BALANCE_CONTROL_H_DATA
#define INC_BALANCE_CONTROL_H_DATA

#include "control_ports.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "balance_control_data.h"
typedef struct {
    BalanceControl_Config_t config;
    BalanceControl_Status_t status;

} BalanceControl_Handle;


#ifdef __cplusplus
}
#endif
#endif
