/*
 * tcrt5000.h
 *
 * Logical model for the 8 TCRT5000 IR sensors sampled by ADC DMA.
 */

#ifndef INC_TCRT5000_H_DATA
#define INC_TCRT5000_H_DATA

#include "am_port.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    void *ctx;
    AM_Result (*initialize)(void *ctx);
    void (*set_emitters)(void *ctx, uint8_t group, bool enabled);
} TCRT5000_Port;
typedef struct {
    TCRT5000_Port port;
    bool emitters_group5_on, emitters_group3_on;
} TCRT5000_Handle;

#ifdef __cplusplus
}
#endif
#endif
