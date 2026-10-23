/*
 * osk_integration.h
 *
 * Integration layer that connects the hardware-agnostic OSK library
 * with the project's SSD1306 driver, MenuSystem, and UserEvent_t
 * infrastructure.
 *
 * Created on: Aug 02, 2026
 */

#ifndef INC_OSK_INTEGRATION_H_
#define INC_OSK_INTEGRATION_H_

#include "osk_types.h"
#include "types/userEvent_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise the OSK integration module (call once at startup).
 *
 * Fills in the OSK_DrawHandle with wrappers around ssd1306_*
 * functions, so the OSK library can render without knowing about
 * the SSD1306 driver.
 */
void OSK_Integration_Init(void);

/**
 * @brief Open the alpha keyboard for text input.
 *
 * Saves the current MenuSystem context (renderFn, eventManagerFn)
 * and replaces it with OSK rendering and event handling.
 *
 * @param initialText Starting text (may be NULL).
 * @param maxLen      Max characters allowed (1..63).
 * @param onComplete  Callback when user presses OK.
 * @param onCancel    Callback when user presses Back.
 * @param ctx         Opaque context forwarded to callbacks.
 */
void OSK_Integration_OpenAlpha(const char *initialText,
                                uint8_t maxLen,
                                const OSK_AlphaConfig *alphaCfg,
                                OSK_CompleteFn onComplete,
                                OSK_CancelFn onCancel,
                                void *ctx);

/**
 * @brief Open the numeric keypad for value input.
 *
 * @param numCfg     Numeric configuration (digits, range, initial value).
 * @param onComplete Callback when user presses OK (text is the value string).
 * @param onCancel   Callback when user presses Back.
 * @param ctx        Opaque context forwarded to callbacks.
 */
void OSK_Integration_OpenNumeric(const OSK_NumericConfig *numCfg,
                                  OSK_CompleteFn onComplete,
                                  OSK_CancelFn onCancel,
                                  void *ctx);

/**
 * @brief UserEvent_t handler — set as menuSystem.userEventManagerFn
 *        while the keyboard is active.
 */
void OSK_Integration_EventManager(UserEvent_t ev);

/**
 * @brief Render wrapper — set as menuSystem.renderFn while the
 *        keyboard is active.
 */
void OSK_Integration_Render(void);

/**
 * @brief Check if the OSK is currently active.
 * @return true if the keyboard is open.
 */
bool OSK_Integration_IsActive(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSK_INTEGRATION_H_ */
