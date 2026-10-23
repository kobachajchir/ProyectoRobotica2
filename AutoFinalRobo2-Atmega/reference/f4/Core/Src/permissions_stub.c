#include "permission_context.h"
/*
 * permissions_stub.c
 *
 * Non-blocking local PIN permission manager plus per-UNER-node remote
 * sessions. The filename is retained so the existing managed build keeps the
 * same source entry; this is no longer a grant-all stub.
 */

#include "permissions.h"

#include <stddef.h>
#include <string.h>

#define now_ms() (Permission_Context->services->now_ms())
#define MenuSys_SetCurrentScreenCode (Permission_Context->services->MenuSys_SetCurrentScreenCode)
#define OledUtils_DismissNotification (Permission_Context->services->OledUtils_DismissNotification)
#define OledUtils_SetNotificationsBlocked (Permission_Context->services->OledUtils_SetNotificationsBlocked)
#define AppPreferences_IsPinLockEnabled (Permission_Context->services->AppPreferences_IsPinLockEnabled)
#define AppPreferences_VerifyAuthPin (Permission_Context->services->AppPreferences_VerifyAuthPin)
#define ssd1306_SetColor (Permission_Context->services->ssd1306_SetColor)
#define ssd1306_DrawBitmap (Permission_Context->services->ssd1306_DrawBitmap)
#define ssd1306_DrawRect (Permission_Context->services->ssd1306_DrawRect)
#define ssd1306_FillRect (Permission_Context->services->ssd1306_FillRect)
#define ssd1306_WriteString (Permission_Context->services->ssd1306_WriteString)
#define ssd1306_SetCursor (Permission_Context->services->ssd1306_SetCursor)
#define ssd1306_Clear (Permission_Context->services->ssd1306_Clear)
#define menuSystem (*Permission_Context->menuSystem)
#define permissionManager (Permission_Context->permissionManager)
#define autoMicroSystem (*Permission_Context->system)


#define PERMISSION_DEFAULT_TTL_MS        30000u
#define PERMISSION_RESPONSE_TTL_MAX_MS  300000u
#define PERMISSION_UNER_NODE_COUNT           16u
#define PERMISSION_SUBMIT_FEEDBACK_MS        250u



static const PermissionPolicy_t permissionPolicies[PERMISSION_COUNT] = {
    [PERMISSION_NONE] = { PERMISSION_NONE, 0u, 0u },
    [PERMISSION_ESP_RESET] = { PERMISSION_ESP_RESET, 30000u, 3u },
    [PERMISSION_FACTORY_RESET] = { PERMISSION_FACTORY_RESET, 15000u, 3u },
    [PERMISSION_WIFI_CREDENTIALS] = { PERMISSION_WIFI_CREDENTIALS, 60000u, 3u },
    [PERMISSION_MOTOR_TEST] = { PERMISSION_MOTOR_TEST, 30000u, 3u },
    [PERMISSION_SYSTEM_CONFIG] = { PERMISSION_SYSTEM_CONFIG, 60000u, 3u },
    [PERMISSION_CAR_MODE_TEST] = { PERMISSION_CAR_MODE_TEST, 60000u, 3u },
};

static bool Permission_IsExpired(uint32_t deadline, uint32_t now_ms)
{
    return deadline != 0u && ((int32_t)(deadline - now_ms) <= 0);
}

static bool Permission_RemoteNodeIsValid(uint8_t source_node)
{
    return Permission_Context->services->remote_node_valid(source_node);
}

static const PermissionPolicy_t *Permission_GetPolicy(PermissionId_t permission)
{
    if (permission >= PERMISSION_COUNT) {
        return &permissionPolicies[PERMISSION_NONE];
    }

    return &permissionPolicies[permission];
}

static void Permission_EmitFor(PermissionId_t permission, PermissionEventId_t event)
{
    PermissionEvent_t permissionEvent;

    if (permissionManager.eventCallback == NULL) {
        return;
    }

    permissionEvent.event = event;
    permissionEvent.permission = permission;
    permissionEvent.request_id = permissionManager.requestId;
    permissionEvent.attempts_left = permissionManager.attemptsLeft;
    permissionManager.eventCallback(&permissionEvent);
}

static void Permission_Emit(PermissionEventId_t event)
{
    Permission_EmitFor(permissionManager.requestedPermission, event);
}

static void Permission_ClearPin(void)
{
    memset(permissionManager.pinDigits, 0, sizeof(permissionManager.pinDigits));
    permissionManager.pinIndex = 0u;
}

static void Permission_ExtendDeadline(uint32_t *deadline_ms,
                                      uint32_t extension_ms)
{
    if (deadline_ms == NULL || *deadline_ms == 0u || extension_ms == 0u) {
        return;
    }

    *deadline_ms += extension_ms;
    if (*deadline_ms == 0u) {
        *deadline_ms = 1u;
    }
}

static void Permission_RequestRender(void)
{
    menuSystem.renderFn = Permission_RenderPinEntry;
    menuSystem.userEventManagerFn = Permission_HandleUserEvent;
    menuSystem.allowPeriodicRefresh = false;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void Permission_SaveUiContext(void)
{
    permissionManager.previousRenderFn = menuSystem.renderFn;
    permissionManager.previousUserEventManagerFn = menuSystem.userEventManagerFn;
    permissionManager.previousAllowPeriodicRefresh = menuSystem.allowPeriodicRefresh;
    permissionManager.previousInsideMenu =
        menuSystem.insideMenuFlag ? *menuSystem.insideMenuFlag : 0u;
}

static void Permission_RestoreUiContext(void)
{
    menuSystem.renderFn = permissionManager.previousRenderFn;
    menuSystem.userEventManagerFn = permissionManager.previousUserEventManagerFn;
    menuSystem.allowPeriodicRefresh = permissionManager.previousAllowPeriodicRefresh;

    if (menuSystem.insideMenuFlag != NULL) {
        *menuSystem.insideMenuFlag = permissionManager.previousInsideMenu;
    }
    if (menuSystem.clearScreen != NULL) {
        menuSystem.clearScreen();
    }

    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
    OledUtils_SetNotificationsBlocked(false);
}

static void Permission_ClearPendingRequest(void)
{
    permissionManager.state = AUTH_IDLE;
    permissionManager.requestedPermission = PERMISSION_NONE;
    permissionManager.pendingAction.onGranted = NULL;
    permissionManager.pendingAction.ctx = NULL;
    permissionManager.localDeadlineMs = 0u;
    permissionManager.submitAtMs = 0u;
    permissionManager.remoteAuthNoticeId = PERMISSION_AUTH_NOTICE_NONE;
    permissionManager.remoteAuthNoticeUntilMs = 0u;
    Permission_ClearPin();
}

static uint32_t Permission_ResolveTtl(PermissionId_t permission, uint32_t ttl_ms)
{
    const PermissionPolicy_t *policy = Permission_GetPolicy(permission);
    uint32_t resolved = ttl_ms;

    if (resolved == 0u) {
        resolved = policy->ttl_ms;
    }
    if (resolved == 0u) {
        resolved = PERMISSION_DEFAULT_TTL_MS;
    }
    if (resolved > PERMISSION_RESPONSE_TTL_MAX_MS) {
        resolved = PERMISSION_RESPONSE_TTL_MAX_MS;
    }

    return resolved;
}

static void Permission_Grant(uint32_t ttl_ms)
{
    const PermissionId_t permission = permissionManager.requestedPermission;
    const uint32_t now = now_ms();
    const uint32_t resolvedTtl = Permission_ResolveTtl(permission, ttl_ms);
    PermissionContinuationFn onGranted = permissionManager.pendingAction.onGranted;
    void *ctx = permissionManager.pendingAction.ctx;

    if (permission > PERMISSION_NONE && permission < PERMISSION_COUNT) {
        permissionManager.authorizedUntil[permission] = now + resolvedTtl;
        if (permissionManager.authorizedUntil[permission] == 0u) {
            permissionManager.authorizedUntil[permission] = 1u;
        }
    }

    permissionManager.state = AUTH_GRANTED;
    Permission_Emit(PERMISSION_EVT_GRANTED);
    permissionManager.attemptsLeft = 0u;
    Permission_ClearPendingRequest();
    Permission_RestoreUiContext();

    if (onGranted != NULL) {
        onGranted(ctx);
    }
}

static void Permission_CancelRequest(void)
{
    permissionManager.state = AUTH_CANCELLED;
    Permission_Emit(PERMISSION_EVT_CANCELLED);
    Permission_ClearPendingRequest();
    Permission_RestoreUiContext();
}

static void Permission_ApplyDeniedState(void)
{
    Permission_ClearPin();
    if (permissionManager.attemptsLeft == 0u) {
        permissionManager.localDeadlineMs = 0u;
        permissionManager.localBlockedUntilMs =
            now_ms() + PERMISSION_LOCAL_BLOCK_MS;
        if (permissionManager.localBlockedUntilMs == 0u) {
            permissionManager.localBlockedUntilMs = 1u;
        }
        permissionManager.state = AUTH_LOCKED;
        Permission_Emit(PERMISSION_EVT_LOCKED);
    } else {
        permissionManager.state = AUTH_DENIED;
        Permission_Emit(PERMISSION_EVT_DENIED);
    }
    Permission_RequestRender();
}

static void Permission_StartInput(void)
{
    const PermissionPolicy_t *policy =
        Permission_GetPolicy(permissionManager.requestedPermission);

    Permission_ClearPin();
    permissionManager.state = AUTH_INPUT_PIN;
    permissionManager.submitAtMs = 0u;
    if (permissionManager.attemptsLeft == 0u) {
        permissionManager.attemptsLeft = policy->max_attempts;
    }
    permissionManager.localDeadlineMs =
        now_ms() + PERMISSION_LOCAL_REQUEST_TIMEOUT_MS;
    if (permissionManager.localDeadlineMs == 0u) {
        permissionManager.localDeadlineMs = 1u;
    }
    Permission_RequestRender();
    Permission_Emit(PERMISSION_EVT_PIN_ENTRY_STARTED);
}

static void Permission_SubmitPin(void)
{
    permissionManager.requestId++;
    if (permissionManager.requestId == 0u) {
        permissionManager.requestId = 1u;
    }
    Permission_Emit(PERMISSION_EVT_PIN_SUBMITTED);

    if (AppPreferences_VerifyAuthPin(permissionManager.pinDigits)) {
        Permission_Grant(0u);
        return;
    }

    if (permissionManager.attemptsLeft > 0u) {
        permissionManager.attemptsLeft--;
    }
    Permission_ApplyDeniedState();
}

static void Permission_BeginPinSubmit(void)
{
    permissionManager.state = AUTH_SUBMITTING;
    permissionManager.submitAtMs = now_ms() + PERMISSION_SUBMIT_FEEDBACK_MS;
    if (permissionManager.submitAtMs == 0u) {
        permissionManager.submitAtMs = 1u;
    }
    Permission_RequestRender();
}

static void Permission_ResetForRetry(void)
{
    if (permissionManager.state != AUTH_DENIED) {
        return;
    }

    Permission_ClearPin();
    permissionManager.state = AUTH_INPUT_PIN;
    Permission_RequestRender();
}

static void Permission_DrawText(uint8_t x, uint8_t y, FontDef font, const char *text)
{
    ssd1306_SetColor(White);
    ssd1306_SetCursor(x, y);
    (void)ssd1306_WriteString((char *)text, font);
}

static void Permission_DrawBaselineText(uint8_t x,
                                        uint8_t baseline_y,
                                        FontDef font,
                                        const char *text)
{
    uint8_t y = baseline_y;

    if (baseline_y >= font.FontHeight) {
        y = (uint8_t)(baseline_y - font.FontHeight);
    }

    Permission_DrawText(x, y, font, text);
}

static void Permission_DrawCenteredBaselineText(uint8_t baseline_y,
                                                FontDef font,
                                                const char *text)
{
    size_t text_len;
    uint16_t text_width;
    uint8_t text_x = 0u;

    if (text == NULL) {
        return;
    }

    text_len = strlen(text);
    text_width = (uint16_t)text_len * font.FontWidth;
    if (text_width < SSD1306_WIDTH) {
        text_x = (uint8_t)((SSD1306_WIDTH - text_width) / 2u);
    }

    Permission_DrawBaselineText(text_x, baseline_y, font, text);
}

static void Permission_RenderRemoteAuthNotice(void)
{
    const char *line1 = "PIN";
    const char *line2 = "verificado";
    ScreenCode_t screen_code = SCREEN_CODE_WARNING_PIN_CORRECT;

    switch (permissionManager.remoteAuthNoticeId) {
        case PERMISSION_AUTH_NOTICE_PIN_INCORRECT:
            screen_code = SCREEN_CODE_WARNING_PIN_INCORRECT;
            line1 = "PIN incorrecto";
            line2 = "Acceso rechazado";
            break;

        case PERMISSION_AUTH_NOTICE_PIN_BYPASSED:
            screen_code = SCREEN_CODE_WARNING_PIN_BYPASSED;
            line1 = "PIN desactivado";
            line2 = "Acceso concedido";
            break;

        case PERMISSION_AUTH_NOTICE_PIN_MODIFIED:
            screen_code = SCREEN_CODE_WARNING_PIN_MODIFIED;
            line1 = "PIN";
            line2 = "modificado";
            break;

        case PERMISSION_AUTH_NOTICE_PIN_CORRECT:
        default:
            screen_code = SCREEN_CODE_WARNING_PIN_CORRECT;
            line1 = "PIN correcto";
            line2 = "Acceso concedido";
            break;
    }

    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 screen_code,
                                 SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);
    ssd1306_DrawBitmap((SSD1306_WIDTH - LOCK_ICON_W) / 2u,
                       3u,
                       LOCK_ICON_W,
                       LOCK_ICON_H,
                       Icon_Lock_bits);
    Permission_DrawCenteredBaselineText(34u, Font_7x10, line1);
    Permission_DrawCenteredBaselineText(52u, Font_7x10, line2);
}

static void Permission_RenderPinDigits(void)
{
    char digitText[2] = { 0 };
    const FontDef digitFont = OLED_LARGE_FONT;
    const uint8_t frameY = 20u;
    const uint8_t frameW = 18u;
    const uint8_t frameH = 20u;
    const uint8_t frameX[PERMISSION_PIN_DIGITS] = { 20u, 43u, 66u, 89u };

    ssd1306_DrawBitmap(8u, 27u, 4u, 7u, Arrow_Left_bits);
    ssd1306_DrawBitmap(116u, 27u, 4u, 7u, Arrow_Right_bits);

    for (uint8_t i = 0u; i < PERMISSION_PIN_DIGITS; ++i) {
        const uint8_t textX =
            (uint8_t)(frameX[i] + ((frameW - digitFont.FontWidth) / 2u));
        const uint8_t textY =
            (uint8_t)(frameY + ((frameH - digitFont.FontHeight) / 2u));

        digitText[0] = (char)('0' + permissionManager.pinDigits[i]);
        ssd1306_SetColor(White);
        ssd1306_DrawRect(frameX[i], frameY, frameW, frameH);
        if (permissionManager.state == AUTH_INPUT_PIN &&
            i == permissionManager.pinIndex) {
            ssd1306_DrawRect((int16_t)frameX[i] - 2,
                             frameY - 2,
                             frameW + 4,
                             frameH + 4);
        }
        Permission_DrawText(textX, textY, digitFont, digitText);
    }
}

static void Permission_RenderEncoderHint(const char *label, uint8_t text_x)
{
    if (label != NULL) {
        Permission_DrawBaselineText(text_x, 60u, Font_7x10, label);
    }
    ssd1306_DrawBitmap(112u, 49u, 13u, 13u, Icon_Encoder_bits);
}

static void Permission_RenderPinForm(void)
{
    ssd1306_SetColor(White);
    ssd1306_DrawBitmap(33u, 0u, LOCK_ICON_W, LOCK_ICON_H, Icon_Lock_bits);
    Permission_DrawBaselineText(49u, 14u, Font_7x10, "PIN");
    Permission_RenderPinDigits();
    if (permissionManager.state == AUTH_SUBMITTING) {
        ssd1306_SetColor(Black);
        ssd1306_FillRect(24u, 46u, 86u, 18u);
        ssd1306_SetColor(White);
        Permission_RenderEncoderHint("Enviando", 49u);
    } else {
        Permission_RenderEncoderHint("Camb/Enviar", 31u);
        
        ssd1306_DrawBitmap(3u, 48u, 15u, 16u, Icon_UserBtn_bits);
        ssd1306_DrawBitmap(20u, 48u, 16u, 16u, Icon_Volver_bits);
    }
}

static void Permission_FormatAttempts(char *buf, size_t len, uint8_t attempts)
{
    static const char prefix[] = "Intentos: ";
    size_t pos = 0u;

    if (buf == NULL || len == 0u) {
        return;
    }
    while (prefix[pos] != '\0' && pos + 1u < len) {
        buf[pos] = prefix[pos];
        pos++;
    }
    if (pos + 1u < len) {
        buf[pos++] = (char)('0' + attempts);
    }
    buf[pos] = '\0';
}

void Permission_Init(void)
{
    PermissionEventCallback callback = permissionManager.eventCallback;

    memset(&permissionManager, 0, sizeof(permissionManager));
    permissionManager.state = AUTH_IDLE;
    permissionManager.requestedPermission = PERMISSION_NONE;
    permissionManager.eventCallback = callback;
    for (uint8_t i = 0u; i < PERMISSION_UNER_NODE_COUNT; ++i) {
        permissionManager.remote[i].attemptsLeft = PERMISSION_REMOTE_MAX_ATTEMPTS;
    }
}

void Permission_Task(uint32_t now_ms)
{
    if (permissionManager.remoteAuthNoticeId != PERMISSION_AUTH_NOTICE_NONE &&
        Permission_IsExpired(permissionManager.remoteAuthNoticeUntilMs, now_ms)) {
        permissionManager.remoteAuthNoticeId = PERMISSION_AUTH_NOTICE_NONE;
        permissionManager.remoteAuthNoticeUntilMs = 0u;
        Permission_RequestRender();
    }

    if (permissionManager.state == AUTH_SUBMITTING &&
        Permission_IsExpired(permissionManager.submitAtMs, now_ms)) {
        permissionManager.submitAtMs = 0u;
        Permission_SubmitPin();
    } else if ((permissionManager.state == AUTH_INPUT_PIN ||
         permissionManager.state == AUTH_DENIED) &&
        Permission_IsExpired(permissionManager.localDeadlineMs, now_ms)) {
        permissionManager.state = AUTH_TIMEOUT;
        Permission_Emit(PERMISSION_EVT_TIMEOUT);
        Permission_ClearPendingRequest();
        Permission_RestoreUiContext();
    } else if (permissionManager.state == AUTH_LOCKED &&
               Permission_IsExpired(permissionManager.localBlockedUntilMs, now_ms)) {
        permissionManager.localBlockedUntilMs = 0u;
        Permission_StartInput();
    } else if (permissionManager.state == AUTH_IDLE &&
               Permission_IsExpired(permissionManager.localBlockedUntilMs, now_ms)) {
        permissionManager.localBlockedUntilMs = 0u;
    }

    for (uint8_t i = (uint8_t)PERMISSION_NONE + 1u;
         i < (uint8_t)PERMISSION_COUNT;
         ++i) {
        if (Permission_IsExpired(permissionManager.authorizedUntil[i], now_ms)) {
            permissionManager.authorizedUntil[i] = 0u;
            Permission_EmitFor((PermissionId_t)i, PERMISSION_EVT_EXPIRED);
        }
    }

    for (uint8_t node = 0u; node < PERMISSION_UNER_NODE_COUNT; ++node) {
        PermissionRemoteSession_t *session = &permissionManager.remote[node];

        if (Permission_IsExpired(session->authorizedUntilMs, now_ms)) {
            session->authenticated = 0u;
            session->authorizedUntilMs = 0u;
        }
        if (Permission_IsExpired(session->blockedUntilMs, now_ms)) {
            session->blockedUntilMs = 0u;
            session->attemptsLeft = PERMISSION_REMOTE_MAX_ATTEMPTS;
        }
    }
}

bool Permission_IsAuthorized(PermissionId_t permission, uint32_t now_ms)
{
    if (permission == PERMISSION_NONE) {
        return true;
    }
    if (permission >= PERMISSION_COUNT) {
        return false;
    }

    return permissionManager.authorizedUntil[permission] != 0u &&
           !Permission_IsExpired(permissionManager.authorizedUntil[permission], now_ms);
}

bool Permission_IsAuthorizedNow(PermissionId_t permission)
{
    return Permission_IsAuthorized(permission, now_ms());
}

bool Permission_Request(PermissionId_t permission,
                        PermissionContinuationFn on_granted,
                        void *ctx)
{
    if (permission == PERMISSION_NONE) {
        if (on_granted != NULL) {
            on_granted(ctx);
        }
        return true;
    }
    if (permission >= PERMISSION_COUNT) {
        return false;
    }
    if (!AppPreferences_IsPinLockEnabled()) {
        if (on_granted != NULL) {
            on_granted(ctx);
        }
        return true;
    }
    if (Permission_IsAuthorizedNow(permission)) {
        if (on_granted != NULL) {
            on_granted(ctx);
        }
        return true;
    }
    if (permissionManager.state != AUTH_IDLE) {
        Permission_EmitFor(permission, PERMISSION_EVT_BUSY);
        return false;
    }

    OledUtils_DismissNotification();
    OledUtils_SetNotificationsBlocked(true);
    Permission_SaveUiContext();
    permissionManager.requestedPermission = permission;
    permissionManager.pendingAction.onGranted = on_granted;
    permissionManager.pendingAction.ctx = ctx;
    Permission_Emit(PERMISSION_EVT_REQUIRED);
    if (permissionManager.localBlockedUntilMs != 0u &&
        !Permission_IsExpired(permissionManager.localBlockedUntilMs, now_ms())) {
        permissionManager.state = AUTH_LOCKED;
        Permission_RequestRender();
        Permission_Emit(PERMISSION_EVT_LOCKED);
    } else {
        permissionManager.localBlockedUntilMs = 0u;
        Permission_StartInput();
    }
    return true;
}

void Permission_Revoke(PermissionId_t permission)
{
    if (permission == PERMISSION_NONE || permission >= PERMISSION_COUNT) {
        return;
    }

    permissionManager.authorizedUntil[permission] = 0u;
    Permission_EmitFor(permission, PERMISSION_EVT_REVOKED);
}

void Permission_RevokeAll(void)
{
    for (uint8_t i = (uint8_t)PERMISSION_NONE + 1u;
         i < (uint8_t)PERMISSION_COUNT;
         ++i) {
        Permission_Revoke((PermissionId_t)i);
    }
}

void Permission_CancelLocalRequest(void)
{
    if (permissionManager.requestedPermission != PERMISSION_NONE &&
        permissionManager.state != AUTH_IDLE) {
        Permission_CancelRequest();
    }
    permissionManager.attemptsLeft = 0u;
    permissionManager.localBlockedUntilMs = 0u;
}

void Permission_HandleUserEvent(UserEvent_t ev)
{
    if (permissionManager.remoteAuthNoticeId != PERMISSION_AUTH_NOTICE_NONE &&
        !Permission_IsExpired(permissionManager.remoteAuthNoticeUntilMs,
                              now_ms())) {
        return;
    }

    switch (permissionManager.state) {
        case AUTH_INPUT_PIN:
            if (ev == UE_ROTATE_CW) {
                permissionManager.pinDigits[permissionManager.pinIndex] =
                    (uint8_t)((permissionManager.pinDigits[permissionManager.pinIndex] + 1u) % 10u);
                Permission_RequestRender();
            } else if (ev == UE_ROTATE_CCW) {
                permissionManager.pinDigits[permissionManager.pinIndex] =
                    (uint8_t)((permissionManager.pinDigits[permissionManager.pinIndex] + 9u) % 10u);
                Permission_RequestRender();
            } else if (ev == UE_ENC_SHORT_PRESS) {
                permissionManager.pinIndex =
                    (uint8_t)((permissionManager.pinIndex + 1u) % PERMISSION_PIN_DIGITS);
                Permission_RequestRender();
            } else if (ev == UE_ENC_LONG_PRESS) {
                Permission_BeginPinSubmit();
            } else if (ev == UE_SHORT_PRESS) {
                Permission_CancelRequest();
            }
            break;

        case AUTH_SUBMITTING:
            break;

        case AUTH_DENIED:
            if (ev == UE_ENC_SHORT_PRESS || ev == UE_ENC_LONG_PRESS) {
                Permission_ResetForRetry();
            } else if (ev == UE_SHORT_PRESS) {
                Permission_CancelRequest();
            }
            break;

        case AUTH_TIMEOUT:
        case AUTH_LOCKED:
            if (ev == UE_SHORT_PRESS) {
                Permission_CancelRequest();
            }
            break;
        case AUTH_CANCELLED:
            break;

        case AUTH_IDLE:
        case AUTH_GRANTED:
        case AUTH_EXPIRED:
        default:
            break;
    }
}

void Permission_RenderPinEntry(void)
{
    char line[22];
    ScreenCode_t screen_code = SCREEN_CODE_WARNING_PIN_ENTRY;

    ssd1306_Clear();
    ssd1306_SetColor(White);

    if (permissionManager.remoteAuthNoticeId != PERMISSION_AUTH_NOTICE_NONE &&
        !Permission_IsExpired(permissionManager.remoteAuthNoticeUntilMs,
                              now_ms())) {
        Permission_RenderRemoteAuthNotice();
        return;
    }

    if (permissionManager.state == AUTH_DENIED) {
        screen_code = SCREEN_CODE_WARNING_PIN_DENIED;
        MenuSys_SetCurrentScreenCode(&menuSystem,
                                     screen_code,
                                     SCREEN_REPORT_SOURCE_PERMISSION);
        Permission_DrawText(15u, 14u, OLED_LARGE_FONT, "Rechazado");
        Permission_FormatAttempts(line, sizeof(line), permissionManager.attemptsLeft);
        Permission_DrawText(4u, 34u, Font_7x10, line);
        Permission_RenderEncoderHint("Reint.", 67u);
    } else if (permissionManager.state == AUTH_LOCKED) {
        screen_code = SCREEN_CODE_WARNING_PIN_BLOCKED;
        MenuSys_SetCurrentScreenCode(&menuSystem,
                                     screen_code,
                                     SCREEN_REPORT_SOURCE_PERMISSION);
        Permission_DrawText(18u, 14u, OLED_LARGE_FONT, "Bloqueado");
        Permission_DrawText(4u, 34u, Font_7x10, "Demasiados fallos");
        Permission_DrawBaselineText(43u, 60u, Font_7x10, "Espere");
    } else if (permissionManager.state == AUTH_SUBMITTING) {
        screen_code = SCREEN_CODE_WARNING_PIN_WAITING;
        MenuSys_SetCurrentScreenCode(&menuSystem,
                                     screen_code,
                                     SCREEN_REPORT_SOURCE_PERMISSION);
        Permission_RenderPinForm();
    } else {
        MenuSys_SetCurrentScreenCode(&menuSystem,
                                     screen_code,
                                     SCREEN_REPORT_SOURCE_PERMISSION);
        Permission_RenderPinForm();
    }
}

bool Permission_GrantCurrentRequest(uint32_t ttl_ms)
{
    if (!Permission_HasActiveLocalRequest()) {
        return false;
    }

    Permission_Grant(ttl_ms);
    return true;
}

bool Permission_HasActiveLocalRequest(void)
{
    return (permissionManager.state == AUTH_INPUT_PIN ||
            permissionManager.state == AUTH_SUBMITTING ||
            permissionManager.state == AUTH_DENIED) &&
           permissionManager.requestedPermission > PERMISSION_NONE &&
           permissionManager.requestedPermission < PERMISSION_COUNT;
}

PermissionId_t Permission_GetRequestedPermission(void)
{
    return permissionManager.requestedPermission;
}

AuthState_t Permission_GetState(void)
{
    return permissionManager.state;
}

void Permission_SetEventCallback(PermissionEventCallback callback)
{
    permissionManager.eventCallback = callback;
}

bool Permission_ShowRemoteAuthNotice(PermissionAuthNoticeId_t notice_id,
                                     uint16_t timeout_ms)
{
    const uint32_t now_ms = now_ms();
    uint32_t duration_ms = timeout_ms;
    uint32_t previous_remaining_ms = 0u;
    uint32_t deadline_extension_ms;

    if (notice_id <= PERMISSION_AUTH_NOTICE_NONE ||
        notice_id > PERMISSION_AUTH_NOTICE_PIN_MODIFIED ||
        permissionManager.requestedPermission == PERMISSION_NONE ||
        permissionManager.state == AUTH_IDLE) {
        return false;
    }

    if (duration_ms == 0u) {
        duration_ms = 1u;
    }
    if (permissionManager.remoteAuthNoticeId != PERMISSION_AUTH_NOTICE_NONE &&
        !Permission_IsExpired(permissionManager.remoteAuthNoticeUntilMs,
                              now_ms)) {
        previous_remaining_ms = permissionManager.remoteAuthNoticeUntilMs - now_ms;
    }

    deadline_extension_ms = (duration_ms > previous_remaining_ms)
        ? (duration_ms - previous_remaining_ms)
        : 0u;
    Permission_ExtendDeadline(&permissionManager.localDeadlineMs,
                              deadline_extension_ms);
    Permission_ExtendDeadline(&permissionManager.submitAtMs,
                              deadline_extension_ms);
    Permission_ExtendDeadline(&permissionManager.localBlockedUntilMs,
                              deadline_extension_ms);

    permissionManager.remoteAuthNoticeId = notice_id;
    permissionManager.remoteAuthNoticeUntilMs = now_ms + duration_ms;
    if (permissionManager.remoteAuthNoticeUntilMs == 0u) {
        permissionManager.remoteAuthNoticeUntilMs = 1u;
    }
    Permission_RequestRender();
    return true;
}

bool Permission_RemoteAuthenticate(
    uint8_t source_node,
    const uint8_t pin_digits[PERMISSION_PIN_DIGITS],
    uint32_t now_ms,
    uint32_t *ttl_ms,
    uint8_t *attempts_left,
    bool *new_session)
{
    PermissionRemoteSession_t *session;
    bool was_authenticated;

    if (ttl_ms != NULL) {
        *ttl_ms = 0u;
    }
    if (attempts_left != NULL) {
        *attempts_left = 0u;
    }
    if (new_session != NULL) {
        *new_session = false;
    }
    if (!Permission_RemoteNodeIsValid(source_node) || pin_digits == NULL) {
        return false;
    }

    session = &permissionManager.remote[source_node];
    if (!AppPreferences_IsPinLockEnabled()) {
        was_authenticated = Permission_IsRemoteNodeAuthorized(source_node, now_ms);
        session->authenticated = 1u;
        session->attemptsLeft = PERMISSION_REMOTE_MAX_ATTEMPTS;
        session->blockedUntilMs = 0u;
        session->authorizedUntilMs = now_ms + PERMISSION_REMOTE_SESSION_TTL_MS;
        if (session->authorizedUntilMs == 0u) {
            session->authorizedUntilMs = 1u;
        }
        if (ttl_ms != NULL) {
            *ttl_ms = PERMISSION_REMOTE_SESSION_TTL_MS;
        }
        if (attempts_left != NULL) {
            *attempts_left = session->attemptsLeft;
        }
        if (new_session != NULL) {
            *new_session = !was_authenticated;
        }
        return true;
    }

    if (session->blockedUntilMs != 0u &&
        !Permission_IsExpired(session->blockedUntilMs, now_ms)) {
        return false;
    }
    if (Permission_IsExpired(session->blockedUntilMs, now_ms)) {
        session->blockedUntilMs = 0u;
        session->attemptsLeft = PERMISSION_REMOTE_MAX_ATTEMPTS;
    }

    if (!AppPreferences_VerifyAuthPin(pin_digits)) {
        session->authenticated = 0u;
        session->authorizedUntilMs = 0u;
        if (session->attemptsLeft > 0u) {
            session->attemptsLeft--;
        }
        if (session->attemptsLeft == 0u) {
            session->blockedUntilMs = now_ms + PERMISSION_REMOTE_BLOCK_MS;
            if (session->blockedUntilMs == 0u) {
                session->blockedUntilMs = 1u;
            }
        }
        if (attempts_left != NULL) {
            *attempts_left = session->attemptsLeft;
        }
        return false;
    }

    was_authenticated = Permission_IsRemoteNodeAuthorized(source_node, now_ms);
    session->authenticated = 1u;
    session->attemptsLeft = PERMISSION_REMOTE_MAX_ATTEMPTS;
    session->blockedUntilMs = 0u;
    session->authorizedUntilMs = now_ms + PERMISSION_REMOTE_SESSION_TTL_MS;
    if (session->authorizedUntilMs == 0u) {
        session->authorizedUntilMs = 1u;
    }

    if (ttl_ms != NULL) {
        *ttl_ms = PERMISSION_REMOTE_SESSION_TTL_MS;
    }
    if (attempts_left != NULL) {
        *attempts_left = session->attemptsLeft;
    }
    if (new_session != NULL) {
        *new_session = !was_authenticated;
    }
    return true;
}

bool Permission_IsRemoteNodeAuthorized(uint8_t source_node, uint32_t now_ms)
{
    PermissionRemoteSession_t *session;

    if (!Permission_RemoteNodeIsValid(source_node)) {
        return false;
    }

    session = &permissionManager.remote[source_node];
    if (!session->authenticated ||
        Permission_IsExpired(session->authorizedUntilMs, now_ms)) {
        session->authenticated = 0u;
        session->authorizedUntilMs = 0u;
        return false;
    }

    return true;
}

bool Permission_IsRemoteNodeAuthorizedNow(uint8_t source_node)
{
    return Permission_IsRemoteNodeAuthorized(source_node, now_ms());
}

void Permission_TouchRemoteNode(uint8_t source_node, uint32_t now_ms)
{
    PermissionRemoteSession_t *session;

    if (!Permission_IsRemoteNodeAuthorized(source_node, now_ms)) {
        return;
    }

    session = &permissionManager.remote[source_node];
    session->authorizedUntilMs = now_ms + PERMISSION_REMOTE_SESSION_TTL_MS;
    if (session->authorizedUntilMs == 0u) {
        session->authorizedUntilMs = 1u;
    }
}

void Permission_RevokeRemoteNode(uint8_t source_node)
{
    PermissionRemoteSession_t *session;

    if (!Permission_RemoteNodeIsValid(source_node)) {
        return;
    }

    session = &permissionManager.remote[source_node];
    session->authenticated = 0u;
    session->authorizedUntilMs = 0u;
}

void Permission_RevokeAllRemote(void)
{
    for (uint8_t node = 0u; node < PERMISSION_UNER_NODE_COUNT; ++node) {
        Permission_RevokeRemoteNode(node);
    }
}

uint32_t Permission_GetRemoteTtlRemaining(uint8_t source_node, uint32_t now_ms)
{
    PermissionRemoteSession_t *session;

    if (!Permission_IsRemoteNodeAuthorized(source_node, now_ms)) {
        return 0u;
    }

    session = &permissionManager.remote[source_node];
    return (uint32_t)(session->authorizedUntilMs - now_ms);
}
