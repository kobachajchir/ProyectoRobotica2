#ifndef AM_PORT_H
#define AM_PORT_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
/* Platform-independent results. Numeric values retain existing adapter semantics. */
typedef int32_t AM_Result;
enum { AM_OK=0, AM_ERROR=1, AM_BUSY=2, AM_TIMEOUT=3 };
typedef struct {
    void *ctx;
    uint32_t (*now_ms)(void *ctx);
    uint32_t (*now_us)(void *ctx);
} AM_Clock;
#endif
