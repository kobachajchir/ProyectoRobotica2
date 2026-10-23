#pragma once

#include <stdint.h>

#include "stream_broker.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SRW_CAPACITY 8u
#define SRW_REQUEST_ID_CAPACITY 65u
#define SRW_COMMAND_CAPACITY 65u

typedef enum {
    SRW_ADD_OK = 0,
    SRW_ADD_INVALID,
    SRW_ADD_FULL,
    SRW_ADD_ALREADY_PENDING
} SrwAddResult;

typedef enum {
    SRW_COMPLETED_CONVERGED = 0,
    SRW_COMPLETED_TIMEOUT,
    SRW_COMPLETED_F4_NACK,
    SRW_COMPLETED_UART_ERROR,
    SRW_COMPLETED_BUSY,
    SRW_COMPLETED_CANCELED
} SrwCompletionStatus;

typedef struct {
    uint8_t used;
    uint32_t client_id;
    uint32_t generation;
    uint8_t api_v1;
    SbStream stream;
    uint8_t subscribed;
    uint16_t period_ms;
    uint32_t deadline_ms;
    char request_id[SRW_REQUEST_ID_CAPACITY];
    char command[SRW_COMMAND_CAPACITY];
} SrwWaiter;

typedef struct {
    SrwWaiter waiter;
    SrwCompletionStatus status;
} SrwCompletion;

typedef struct {
    uint32_t added;
    uint32_t rejected_full;
    uint32_t rejected_duplicate;
    uint32_t converged;
    uint32_t timed_out;
    uint32_t failed;
    uint32_t canceled;
    uint8_t high_watermark;
} SrwMetrics;

typedef struct {
    SrwWaiter waiters[SRW_CAPACITY];
    SrwMetrics metrics;
} StreamRequestWaiters;

void srw_init(StreamRequestWaiters *manager);

SrwAddResult srw_add(StreamRequestWaiters *manager,
                     uint32_t client_id,
                     uint32_t generation,
                     uint8_t api_v1,
                     SbStream stream,
                     uint8_t subscribed,
                     uint16_t period_ms,
                     const char *request_id,
                     const char *command,
                     uint32_t deadline_ms);

uint8_t srw_has_pending(const StreamRequestWaiters *manager,
                        uint32_t client_id,
                        uint32_t generation,
                        SbStream stream);

uint8_t srw_take_for_stream(StreamRequestWaiters *manager,
                            SbStream stream,
                            SrwCompletionStatus status,
                            SrwCompletion *completion);

uint8_t srw_take_endpoint_stream(StreamRequestWaiters *manager,
                                 uint32_t client_id,
                                 uint32_t generation,
                                 SbStream stream,
                                 SrwCompletionStatus status,
                                 SrwCompletion *completion);

uint8_t srw_take_expired(StreamRequestWaiters *manager,
                         uint32_t now_ms,
                         SrwCompletion *completion);

uint8_t srw_remove_client(StreamRequestWaiters *manager,
                          uint32_t client_id,
                          uint32_t generation);

uint8_t srw_count(const StreamRequestWaiters *manager);
const SrwMetrics *srw_metrics(const StreamRequestWaiters *manager);

#ifdef __cplusplus
}
#endif
