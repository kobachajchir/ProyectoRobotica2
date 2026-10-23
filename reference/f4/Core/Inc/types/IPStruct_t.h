/*
 * IPStruct_t.h
 *
 * Small IPv4 value type used by the UI/status screens.
 */

#ifndef INC_TYPES_IPSTRUCT_T_H_
#define INC_TYPES_IPSTRUCT_T_H_

#include <stdint.h>

typedef struct {
    uint8_t block1;
    uint8_t block2;
    uint8_t block3;
    uint8_t block4;
} IPStruct_t;

#endif /* INC_TYPES_IPSTRUCT_T_H_ */
