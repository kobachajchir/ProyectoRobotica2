#include "screens_internal.h"
/*
 * osk_integration.c
 *
 * Connects the hardware-agnostic OSK library to the project's
 * SSD1306 display driver and MenuSystem UI framework.
 *
 * Pattern follows the same UI-hijack approach used by permissions_stub.c:
 *  1. Save menuSystem.renderFn / userEventManagerFn / allowPeriodicRefresh
 *  2. Replace them with OSK-specific handlers
 *  3. Restore on completion or cancellation
 *
 * Created on: Aug 02, 2026
 */

#include "osk_integration.h"

#include "fonts.h"
#include "screen_codes.h"
#include "oled_utils.h"

#include <string.h>


/* ================================================================== */
/*  Saved UI context (restored when OSK closes)                       */
/* ================================================================== */



/* ================================================================== */
/*  Module-level state                                                */
/* ================================================================== */






/* ================================================================== */
/*  Draw handle wrappers — thin shims around ssd1306_*                */
/* ================================================================== */

static void wrap_clearScreen(void)
{
    ssd1306_Clear();
}

static void wrap_setColor(uint8_t color)
{
    SSD1306_COLOR c;
    switch (color) {
        case OSK_COLOR_WHITE:   c = White;   break;
        case OSK_COLOR_INVERSE: c = Inverse; break;
        default:                c = Black;   break;
    }
    ssd1306_SetColor(c);
}

static void wrap_setCursor(uint8_t x, uint8_t y)
{
    ssd1306_SetCursor(x, y);
}

static void wrap_writeChar(char ch)
{
    ssd1306_WriteChar(ch, Font_7x10);
}

static void wrap_writeString(char *str)
{
    ssd1306_WriteString(str, Font_7x10);
}

static void wrap_drawRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
    ssd1306_DrawRect(x, y, w, h);
}

static void wrap_fillRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
    ssd1306_FillRect(x, y, w, h);
}

static void wrap_drawHLine(int16_t x, int16_t y, int16_t len)
{
    ssd1306_DrawHorizontalLine(x, y, len);
}

static void wrap_drawVLine(int16_t x, int16_t y, int16_t len)
{
    ssd1306_DrawVerticalLine(x, y, len);
}

static void wrap_updateScreen(void)
{
    ssd1306_UpdateScreen();
}

static void wrap_requestPageUpdate(uint8_t page)
{
    ssd1306_RequestPageUpdate(page, I2C_REQ_PRIO_REGULAR, 0u);
}

/* ================================================================== */
/*  UI context save / restore                                         */
/* ================================================================== */

static void osk_SaveUiContext(void)
{
    s_savedCtx.prevRenderFn       = menuSystem.renderFn;
    s_savedCtx.prevEventManagerFn = menuSystem.userEventManagerFn;
    s_savedCtx.prevAllowPeriodicRefresh = menuSystem.allowPeriodicRefresh;
    s_savedCtx.prevInsideMenu     = menuSystem.insideMenuFlag
                                    ? *menuSystem.insideMenuFlag : 0u;
    s_savedCtx.prevScreenCode     = menuSystem.current_screen_code;
}

static void osk_RestoreUiContext(void)
{
    menuSystem.renderFn           = s_savedCtx.prevRenderFn;
    menuSystem.userEventManagerFn = s_savedCtx.prevEventManagerFn;
    menuSystem.allowPeriodicRefresh = s_savedCtx.prevAllowPeriodicRefresh;

    if (menuSystem.insideMenuFlag != NULL) {
        *menuSystem.insideMenuFlag = s_savedCtx.prevInsideMenu;
    }

    menuSystem.current_screen_code = s_savedCtx.prevScreenCode;

    if (menuSystem.clearScreen != NULL) {
        menuSystem.clearScreen();
    }

    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
    OledUtils_SetNotificationsBlocked(false);
}

static void osk_ActivateUi(ScreenCode_t screenCode)
{
    osk_SaveUiContext();

    menuSystem.renderFn           = OSK_Integration_Render;
    menuSystem.userEventManagerFn = OSK_Integration_EventManager;
    menuSystem.allowPeriodicRefresh = false;
    menuSystem.current_screen_code = screenCode;
    menuSystem.renderFlag         = true;
    autoMicroSystem.runtime.oled_first_draw               = true;

    OledUtils_SetNotificationsBlocked(true);
}

/* ================================================================== */
/*  Completion / cancellation callbacks (called by the OSK library)   */
/* ================================================================== */

/**
 * Internal wrapper: the real user callback is stored in cfg.onComplete.
 * We need to restore UI context AFTER calling the user callback so
 * the user callback can optionally open another screen.
 */




static void osk_InternalOnComplete(OSK_State *state, void *ctx)
{
    (void)ctx;
    OSK_CompleteFn userCb  = s_userOnComplete;
    void          *userCtx = s_userCtx;

    s_userOnComplete = NULL;
    s_userOnCancel   = NULL;
    s_userCtx        = NULL;

    osk_RestoreUiContext();

    if (userCb) {
        userCb(state, userCtx);
    }
}

static void osk_InternalOnCancel(void *ctx)
{
    (void)ctx;
    OSK_CancelFn userCb  = s_userOnCancel;
    void        *userCtx = s_userCtx;

    s_userOnComplete = NULL;
    s_userOnCancel   = NULL;
    s_userCtx        = NULL;

    osk_RestoreUiContext();

    if (userCb) {
        userCb(userCtx);
    }
}

/* ================================================================== */
/*  PUBLIC API                                                        */
/* ================================================================== */

void OSK_Integration_Init(void)
{
    s_drawHandle.clearScreen      = wrap_clearScreen;
    s_drawHandle.setColor         = wrap_setColor;
    s_drawHandle.setCursor        = wrap_setCursor;
    s_drawHandle.writeChar        = wrap_writeChar;
    s_drawHandle.writeString      = wrap_writeString;
    s_drawHandle.drawRect         = wrap_drawRect;
    s_drawHandle.fillRect         = wrap_fillRect;
    s_drawHandle.drawHLine        = wrap_drawHLine;
    s_drawHandle.drawVLine        = wrap_drawVLine;
    s_drawHandle.updateScreen     = wrap_updateScreen;
    s_drawHandle.requestPageUpdate = wrap_requestPageUpdate;

    s_initialized = true;
}

void OSK_Integration_OpenAlpha(const char *initialText,
                                uint8_t maxLen,
                                const OSK_AlphaConfig *alphaCfg,
                                OSK_CompleteFn onComplete,
                                OSK_CancelFn onCancel,
                                void *ctx)
{
    if (!s_initialized) OSK_Integration_Init();

    /* Store user callbacks (we wrap them for UI restore) */
    s_userOnComplete = onComplete;
    s_userOnCancel   = onCancel;
    s_userCtx        = ctx;

    OSK_Config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.type        = OSK_TYPE_ALPHA;
    cfg.draw        = s_drawHandle;
    cfg.onComplete  = osk_InternalOnComplete;
    cfg.onCancel    = osk_InternalOnCancel;
    cfg.callbackCtx = NULL;
    cfg.initialText = initialText;
    cfg.maxLen      = maxLen;
    if (alphaCfg) {
        cfg.alpha = *alphaCfg;
    } else {
        /* Default generic alpha config */
        cfg.alpha.allowLowercase = true;
        cfg.alpha.allowUppercase = true;
        cfg.alpha.allowSymbols   = true;
        cfg.alpha.allowNumbers   = true;
    }

    OSK_Open(&s_oskState, &cfg);

    osk_ActivateUi(SCREEN_CODE_INPUT_KEYBOARD_ALPHA);
}

void OSK_Integration_OpenNumeric(const OSK_NumericConfig *numCfg,
                                  OSK_CompleteFn onComplete,
                                  OSK_CancelFn onCancel,
                                  void *ctx)
{
    if (!s_initialized) OSK_Integration_Init();

    s_userOnComplete = onComplete;
    s_userOnCancel   = onCancel;
    s_userCtx        = ctx;

    OSK_Config cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.type        = OSK_TYPE_NUMERIC;
    cfg.draw        = s_drawHandle;
    cfg.onComplete  = osk_InternalOnComplete;
    cfg.onCancel    = osk_InternalOnCancel;
    cfg.callbackCtx = NULL;
    cfg.numeric     = *numCfg;

    OSK_Open(&s_oskState, &cfg);

    osk_ActivateUi(SCREEN_CODE_INPUT_KEYBOARD_NUMERIC);
}

void OSK_Integration_EventManager(UserEvent_t ev)
{
    switch (ev) {
        case UE_ROTATE_CCW:
            OSK_OnRight(&s_oskState);
            break;
        case UE_ROTATE_CW:
            OSK_OnLeft(&s_oskState);
            break;
        case UE_ENC_SHORT_PRESS:
            OSK_OnSelect(&s_oskState);
            break;
        case UE_SHORT_PRESS:    /* User/Back button */
            OSK_OnBack(&s_oskState);
            break;
        case UE_LONG_PRESS:     /* Long press = also cancel */
            OSK_OnBack(&s_oskState);
            break;
        default:
            break;
    }

    /* Trigger render after any event */
    if (OSK_IsActive(&s_oskState)) {
        menuSystem.renderFlag = true;
    }
}

void OSK_Integration_Render(void)
{
    OSK_Render(&s_oskState);
}

bool OSK_Integration_IsActive(void)
{
    return OSK_IsActive(&s_oskState);
}

void Screens_Init_osk_integration(void)
{

}
