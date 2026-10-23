#ifndef INC_SCREEN_LOCK_H_
#define INC_SCREEN_LOCK_H_

#include <stdbool.h>
#include <stdint.h>
#include "types/userEvent_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SCREEN_LOCK_EVENT_PASS = 0u,
    SCREEN_LOCK_EVENT_CONSUMED
} ScreenLock_EventResult_t;

void ScreenLock_Init(void);
void ScreenLock_Task10ms(void);
void ScreenLock_OnPreferencesChanged(void);
ScreenLock_EventResult_t ScreenLock_ProcessEvent(UserEvent_t ev);

bool ScreenLock_IsLocked(void);
uint16_t ScreenLock_GetTimeoutDraftSec(void);
void ScreenLock_BeginTimeoutEdit(void);
void ScreenLock_AdjustTimeoutDraft(int16_t deltaSec);
void ScreenLock_SaveTimeoutDraft(void);
void ScreenLock_CancelTimeoutDraft(void);
void ScreenLock_Unlock(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SCREEN_LOCK_H_ */
