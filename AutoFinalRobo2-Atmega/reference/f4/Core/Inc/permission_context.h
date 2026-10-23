#ifndef PERMISSION_CONTEXT_H
#define PERMISSION_CONTEXT_H
#include "permissions_types.h"
#include "ssd1306_types.h"
#include "app_preferences_types.h"
#include "types/menu_types.h"
#include "system.h"
#include "fonts.h"
#define PERMISSION_UNER_NODE_COUNT 16u
typedef struct {
    PermissionContinuationFn onGranted;
    void *ctx;
} PendingPermissionAction_t;

typedef struct {
    uint8_t authenticated;
    uint8_t attemptsLeft;
    uint32_t authorizedUntilMs;
    uint32_t blockedUntilMs;
} PermissionRemoteSession_t;

typedef struct {
    AuthState_t state;
    PermissionId_t requestedPermission;
    PendingPermissionAction_t pendingAction;
    uint8_t requestId;
    uint8_t pinDigits[PERMISSION_PIN_DIGITS];
    uint8_t pinIndex;
    uint8_t attemptsLeft;
    uint32_t localDeadlineMs;
    uint32_t submitAtMs;
    uint32_t localBlockedUntilMs;
    PermissionAuthNoticeId_t remoteAuthNoticeId;
    uint32_t remoteAuthNoticeUntilMs;
    uint32_t authorizedUntil[PERMISSION_COUNT];
    RenderFunction previousRenderFn;
    UserEventManagerFn previousUserEventManagerFn;
    bool previousAllowPeriodicRefresh;
    uint8_t previousInsideMenu;
    PermissionEventCallback eventCallback;
    PermissionRemoteSession_t remote[PERMISSION_UNER_NODE_COUNT];
} PermissionManager_t;

typedef struct {
    uint32_t (*now_ms)(void);
    bool (*remote_node_valid)(uint8_t node);
    bool (*AppPreferences_IsPinLockEnabled)(void);
    bool (*AppPreferences_VerifyAuthPin)(const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS]);
    void (*MenuSys_SetCurrentScreenCode)(MenuSystem *ms, ScreenCode_t screen_code, ScreenReportSource_t source);
    void (*OledUtils_DismissNotification)(void);
    void (*OledUtils_SetNotificationsBlocked)(bool blocked);
    void (*ssd1306_Clear)(void);
    void (*ssd1306_DrawBitmap)(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP);
    void (*ssd1306_DrawRect)(int16_t x, int16_t y, int16_t width, int16_t height);
    void (*ssd1306_FillRect)(int16_t xMove, int16_t yMove, int16_t width, int16_t height);
    void (*ssd1306_SetColor)(SSD1306_COLOR color);
    void (*ssd1306_SetCursor)(uint8_t x, uint8_t y);
    char (*ssd1306_WriteString)(char* str, FontDef Font);
} PermissionServices;
typedef struct {
    const PermissionServices *services;
    SystemHandle *system;
    MenuSystem *menuSystem;
    PermissionManager_t permissionManager;
} PermissionHandle;
extern PermissionHandle *Permission_Context;
#endif
