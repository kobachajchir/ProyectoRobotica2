/*
 * oled_handle.h
 *
 *  Created on: Sep 21, 2026
 *      Author: Codex
 */

#ifndef INC_OLED_HANDLE_H_
#define INC_OLED_HANDLE_H_

#include "types/menu_types.h"
#include <stdbool.h>
#include <stdint.h>

typedef void (*OledNotificationHook)(void);

#define OLED_MAX_NOTIFICATIONS 8
#define OLED_NOTIFICATION_DEFAULT_MS 3000U

typedef struct OledNotificationData {
    RenderFunction renderFn;
    uint8_t simpleId;
    uint8_t clientNumber;
    uint16_t timeoutTicks;
    uint16_t totalTicks;
    bool needsFullRender;
    OledNotificationHook onShow;
    OledNotificationHook onHide;
} OledNotificationData;

typedef struct OledNotificationState {
    bool active;

    OledNotificationData queue[OLED_MAX_NOTIFICATIONS];
    uint8_t head;
    uint8_t tail;
    uint8_t count;

    RenderFunction previousRenderFn;
    UserEventManagerFn previousEventManagerFn;
    bool previousAllowPeriodicRefresh;
    bool previousRenderFlag;
    ScreenCode_t previousScreenCode;
    uint8_t previousScreenSource;

    struct {
        bool valid;
        OledNotificationData data;
    } suspended;
} OledNotificationState;

typedef struct OledHandle {
    OledNotificationState notification;
} OledHandle;

#endif /* INC_OLED_HANDLE_H_ */
