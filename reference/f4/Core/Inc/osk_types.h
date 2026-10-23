/*
 * osk.h
 *
 * On-Screen Keyboard library for monochrome 128x64 OLED displays.
 *
 * This module is intentionally hardware-agnostic: it depends on ZERO
 * project headers (no ssd1306.h, no encoder.h, no HAL).  All drawing
 * is performed through the function-pointer table in OSK_DrawHandle,
 * and all input is delivered through explicit OSK_On*() calls.
 *
 * Planned use-cases:
 *   - WiFi STA password entry         (OSK_TYPE_ALPHA)
 *   - AP SSID / AP password config    (OSK_TYPE_ALPHA)
 *   - PID Kp / Ki / Kd value entry    (OSK_TYPE_NUMERIC)
 *
 * Created on: Aug 02, 2026
 */

#ifndef INC_OSK_H_DATA
#define INC_OSK_H_DATA

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Constants                                                         */
/* ------------------------------------------------------------------ */

/** Maximum characters the text buffer can hold (excluding '\0'). */
#define OSK_MAX_TEXT_LEN   63u

/** OLED geometry the layouts are designed for. */
#define OSK_SCREEN_W      128u
#define OSK_SCREEN_H       64u

/** Enable floating point support (set to 0 to compile FPU logic out) */
#ifndef OSK_SUPPORT_FLOAT
#define OSK_SUPPORT_FLOAT 1
#endif

/* ------------------------------------------------------------------ */
/*  Enums                                                             */
/* ------------------------------------------------------------------ */

/** Keyboard type selector. */
typedef enum {
    OSK_TYPE_ALPHA   = 0,   /**< Full QWERTY + symbols keyboard. */
    OSK_TYPE_NUMERIC = 1,   /**< Numeric-only keypad.            */
} OSK_Type;

/** Numeric keypad mode. */
typedef enum {
    OSK_NUMERIC_MODE_INT = 0,
#if OSK_SUPPORT_FLOAT
    OSK_NUMERIC_MODE_FLOAT = 1,
#endif
} OSK_NumericMode;

/** Current input mode for the alpha keyboard. */
typedef enum {
    OSK_MODE_LOWER  = 0,    /**< Lowercase QWERTY.     */
    OSK_MODE_UPPER  = 1,    /**< Uppercase QWERTY.     */
    OSK_MODE_SYMBOL = 2,    /**< Symbols and digits.   */
} OSK_Mode;

/** Result passed to the completion callback. */
typedef enum {
    OSK_RESULT_OK     = 0,  /**< User confirmed with OK.    */
    OSK_RESULT_CANCEL = 1,  /**< User pressed Back/Cancel.  */
} OSK_Result;

/* ------------------------------------------------------------------ */
/*  SSD1306_COLOR mirror (so we don't include ssd1306.h)              */
/* ------------------------------------------------------------------ */

#define OSK_COLOR_BLACK   0u
#define OSK_COLOR_WHITE   1u
#define OSK_COLOR_INVERSE 2u

/* ------------------------------------------------------------------ */
/*  Draw handle — OLED abstraction via function pointers              */
/* ------------------------------------------------------------------ */

/**
 * @brief Rendering primitives the integrator fills in.
 *
 * Every pointer MUST be non-NULL when OSK_Open() is called.
 * Signatures mirror the ssd1306 driver API so the wrapper layer
 * is trivially thin.
 */
typedef struct {
    void (*clearScreen)(void);
    void (*setColor)(uint8_t color);  /* OSK_COLOR_* */
    void (*setCursor)(uint8_t x, uint8_t y);
    void (*writeChar)(char ch);
    void (*writeString)(char *str);
    void (*drawRect)(int16_t x, int16_t y, int16_t w, int16_t h);
    void (*fillRect)(int16_t x, int16_t y, int16_t w, int16_t h);
    void (*drawHLine)(int16_t x, int16_t y, int16_t len);
    void (*drawVLine)(int16_t x, int16_t y, int16_t len);
    void (*updateScreen)(void);                         /* full flush  */
    void (*requestPageUpdate)(uint8_t page);            /* single page */
} OSK_DrawHandle;

/* ------------------------------------------------------------------ */
/*  Numeric keypad configuration                                      */
/* ------------------------------------------------------------------ */

typedef struct {
    OSK_NumericMode mode;               /**< Integer or Floating point mode. */
    uint8_t  maxIntegerDigits;          /**< Max digits before the decimal point (1..8). */
    bool     allowNegative;             /**< If true, a ± key is shown. */
    bool     prefillZero;               /**< If true, an initial '0' is placed if no initial text is provided. */
    int32_t  minIntValue;               /**< Minimum accepted integer value (inclusive). */
    int32_t  maxIntValue;               /**< Maximum accepted integer value (inclusive). */
    int32_t  initialIntValue;           /**< Value shown when the keypad opens in INT mode. */
#if OSK_SUPPORT_FLOAT
    uint8_t  maxDecimalDigits;          /**< Max digits after the decimal point (0..4, 0 = no dot). */
    float    minFloatValue;             /**< Minimum accepted float value (inclusive). */
    float    maxFloatValue;             /**< Maximum accepted float value (inclusive). */
    float    initialFloatValue;         /**< Value shown when the keypad opens in FLOAT mode. */
#endif
} OSK_NumericConfig;

/* ------------------------------------------------------------------ */
/*  Alpha keyboard configuration                                      */
/* ------------------------------------------------------------------ */

typedef enum {
    OSK_INITIAL_TEXT_PREFILL = 0,
    OSK_INITIAL_TEXT_EMPTY = 1,
} OSK_InitialTextMode;

typedef struct {
    bool allowLowercase; /**< If true, allows lowercase letters. */
    bool allowUppercase; /**< If true, allows uppercase letters (enables Shift key). */
    bool allowNumbers;   /**< If true, allows numbers in symbol view. */
    bool allowSymbols;   /**< If true, allows symbols. */
    OSK_InitialTextMode initialTextMode; /**< PREFILL by default; EMPTY hides supplied text. */
    const char *allowedChars; /**< Optional exact character allow-list. */
} OSK_AlphaConfig;

/* ------------------------------------------------------------------ */
/*  Callbacks                                                         */
/* ------------------------------------------------------------------ */

/* Forward declaration for the callback */
typedef struct OSK_State_t OSK_State;

/**
 * @brief Called when the user confirms input (OK key).
 * @param state Pointer to the keyboard state. Use OSK_GetText, OSK_GetNumericValueInt, etc.
 * @param ctx   Opaque context supplied in OSK_Config.
 */
typedef void (*OSK_CompleteFn)(OSK_State *state, void *ctx);

/**
 * @brief Called when the user cancels input (Back key).
 * @param ctx   Opaque context supplied in OSK_Config.
 */
typedef void (*OSK_CancelFn)(void *ctx);

/* ------------------------------------------------------------------ */
/*  Configuration                                                     */
/* ------------------------------------------------------------------ */

typedef struct {
    OSK_Type           type;          /**< ALPHA or NUMERIC.                     */
    OSK_DrawHandle     draw;          /**< Rendering function pointers.          */
    OSK_CompleteFn     onComplete;    /**< OK callback.                          */
    OSK_CancelFn       onCancel;      /**< Cancel/Back callback.                 */
    void              *callbackCtx;   /**< Forwarded to onComplete / onCancel.   */
    const char        *initialText;   /**< Starting buffer content.              */
    uint8_t            maxLen;        /**< Max text length (ALPHA, 1..63).       */
    OSK_NumericConfig  numeric;       /**< Numeric config (NUMERIC only).        */
    OSK_AlphaConfig    alpha;         /**< Alpha config (ALPHA only).            */
} OSK_Config;

/* ------------------------------------------------------------------ */
/*  Internal layout helpers (exposed for static sizing only)          */
/* ------------------------------------------------------------------ */

/** Maximum number of rows in any layout (symbol layout has 5 key rows). */
#define OSK_MAX_ROWS      6u
/** Maximum number of columns in any row. */
#define OSK_MAX_COLS     12u

/* ------------------------------------------------------------------ */
/*  State                                                             */
/* ------------------------------------------------------------------ */

struct OSK_State_t {
    /* ---- active configuration ---- */
    OSK_Config  cfg;
    OSK_Mode    mode;

    /* ---- text buffer ---- */
    char        textBuf[OSK_MAX_TEXT_LEN + 1];
    uint8_t     textLen;

    /* ---- keyboard cursor ---- */
    uint8_t     curRow;
    uint8_t     curCol;

    /* ---- dirty tracking ---- */
    bool        active;
    bool        needsFullRedraw;
    bool        needsTextRedraw;
    bool        needsKeyHighlight;   /**< Only prev/cur key changed.      */
    uint8_t     prevRow;             /**< Previous cursor row for unhighlight. */
    uint8_t     prevCol;             /**< Previous cursor col for unhighlight. */

    /* ---- numeric-specific ---- */
    bool        hasDecimalPoint;     /**< Dot already placed in buffer.   */
    bool        isNegative;          /**< Leading minus sign active.      */
    bool        validationError;     /**< Flash feedback on range error.  */
};

/* ------------------------------------------------------------------ */
/*  Public API                                                        */
/* ------------------------------------------------------------------ */

/**
 * @brief Initialise and open the on-screen keyboard.
 *
 * After this call, OSK_Render() must be invoked to draw the initial
 * frame, and subsequently after each OSK_On*() event.
 */

#ifdef __cplusplus
}
#endif
#endif
