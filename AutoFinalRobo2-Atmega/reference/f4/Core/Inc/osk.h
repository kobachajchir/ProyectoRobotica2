#ifndef INC_OSK_H_
#define INC_OSK_H_
#include "osk_types.h"
#ifdef __cplusplus
extern "C" {
#endif
void OSK_Open(OSK_State *state, const OSK_Config *cfg);

/** Close the keyboard without invoking any callback. */
void OSK_Close(OSK_State *state);

/* ---- Input events (called by the integrator) ---- */

void OSK_OnLeft(OSK_State *state);     /**< Encoder CCW / navigate left. */
void OSK_OnRight(OSK_State *state);    /**< Encoder CW  / navigate right.*/
void OSK_OnSelect(OSK_State *state);   /**< Encoder press / select key.  */
void OSK_OnBack(OSK_State *state);     /**< Back button / cancel.        */

/* ---- Rendering ---- */

/**
 * @brief Render (or re-render) the keyboard to the display.
 *
 * Only the dirty regions are redrawn; call after every OSK_On*() event.
 */
void OSK_Render(OSK_State *state);

/* ---- Queries ---- */

bool        OSK_IsActive(const OSK_State *state);
const char *OSK_GetText(const OSK_State *state);

/**
 * @brief Parse the current text buffer as an integer (NUMERIC mode only).
 * @return The numeric value, or 0 if parsing fails.
 */
int32_t OSK_GetNumericValueInt(const OSK_State *state);

#if OSK_SUPPORT_FLOAT
/**
 * @brief Parse the current text buffer as a float (NUMERIC mode only).
 * @return The numeric value, or 0.0f if parsing fails.
 */
float OSK_GetNumericValueFloat(const OSK_State *state);
#endif

#ifdef __cplusplus
}
#endif

#endif /* INC_OSK_H_ */
