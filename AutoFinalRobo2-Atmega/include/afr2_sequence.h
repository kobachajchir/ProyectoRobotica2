/* AFR2-20261024-01. Application-owned, bounded sequence runner. */
#ifndef AFR2_SEQUENCE_H
#define AFR2_SEQUENCE_H
#include <stdint.h>
/* AFR2-20261107-40: habilitado por pedido del usuario (32 pasos, 656 B de SRAM). Compilar con
 * -DAFR2_SEQUENCE_MEMORY_APPROVED=0 recupera el perfil sin almacenamiento de pasos. */
#ifndef AFR2_SEQUENCE_MEMORY_APPROVED
#define AFR2_SEQUENCE_MEMORY_APPROVED 1
#endif
#define AFR2_SEQUENCE_MAX 32u
#define AFR2_SEQUENCE_TIME_MAX 120000UL
typedef struct { uint32_t ms; int16_t from[4],to[4]; } Afr2Segment;
/* v6 uses the SAME 640-byte buffer. A pulse step occupies 20 bytes. */
typedef struct { uint32_t limit,timeout,ramp; int16_t from,to; uint8_t wheel,condition,next,kind; } Afr2WheelStep;
typedef struct {
    /* AFR2-20261106-04: disabled profile has no dormant step storage. */
#if AFR2_SEQUENCE_MEMORY_APPROVED
    union { Afr2Segment segments[AFR2_SEQUENCE_MAX]; Afr2WheelStep steps[AFR2_SEQUENCE_MAX]; };
    uint8_t kinds[(AFR2_SEQUENCE_MAX+1)/2]; /* packed semantic IDs; +16 SRAM bytes */
#endif
    uint32_t token,execution,total,sum,started,elapsed,source_session;
    uint8_t state,count,loaded,index;
    uint8_t mode,first[4],cursor[4],completed,done;
    uint32_t wheel_started[4],wheel_goal[4],wheel_pulses[4];
    int32_t wheel_previous[4];
} Afr2Sequence;
struct Afr2Product;
uint8_t Afr2Sequence_Available(const struct Afr2Product *p);
void Afr2Sequence_Tick(struct Afr2Product *p);
uint8_t Afr2Sequence_Command(struct Afr2Product *p,uint8_t cmd,const uint8_t *d,
                            uint8_t n,uint8_t *out,uint8_t *length);
#endif
