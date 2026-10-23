/*
 * user_button.c
 *
 *  Created on: Sep 22, 2025
 *      Author: kobac
 */

#include "user_button.h"
#include "utils/macros_utils.h"

void UserButton_Init(UserButton_Handle_t *btn,
                     const UserButton_Port *port,
                     UserButton_ActiveLevel activeLevel)
{
    if (!btn || !port || !port->read_level) return;
    btn->state.flags.byte = 0;
    btn->state.counter = 0;
    btn->port = *port;
    btn->activeLevel = activeLevel;
}

void UserButton_Task10ms(UserButton_Handle_t *btn)
{
    if (!btn) return;

    bool raw = btn->port.read_level(btn->port.ctx);
    bool pressed = (btn->activeLevel == USER_BUTTON_ACTIVE_HIGH)
                   ? (raw == true)
                   : (raw == false);

    if (pressed && !IS_FLAG_SET(btn->state.flags, BTN_USER_PREVSTATE)) {
        btn->state.counter = 1;
        SET_FLAG(btn->state.flags, BTN_USER_PREVSTATE);
        CLEAR_FLAG(btn->state.flags, BTN_USER_SHORT_PRESS);
        CLEAR_FLAG(btn->state.flags, BTN_USER_LONG_PRESS);
        NIBBLEH_SET_STATE(btn->state.flags, 0);
    } else if (pressed && IS_FLAG_SET(btn->state.flags, BTN_USER_PREVSTATE)) {
        if (btn->state.counter < 255) {
            btn->state.counter++;
        }

        if (btn->state.counter > 100) {
            btn->state.counter = 0;
            uint8_t ovf = NIBBLEH_GET_STATE(btn->state.flags);
            if (ovf < BTN_USER_OVF_MAX) {
                ovf++;
                NIBBLEH_SET_STATE(btn->state.flags, ovf);
            }
        }
    } else if (!pressed && IS_FLAG_SET(btn->state.flags, BTN_USER_PREVSTATE)) {
        if (NIBBLEH_GET_STATE(btn->state.flags) == 0u) {
            if (btn->state.counter >= 10 && btn->state.counter < 30) {
                SET_FLAG(btn->state.flags, BTN_USER_SHORT_PRESS);
            } else if (btn->state.counter >= 30 && btn->state.counter <= 100) {
                SET_FLAG(btn->state.flags, BTN_USER_LONG_PRESS);
            }
        } else {
            /* A hold longer than one second is still one long press. */
            SET_FLAG(btn->state.flags, BTN_USER_LONG_PRESS);
        }

        CLEAR_FLAG(btn->state.flags, BTN_USER_PREVSTATE);
        btn->state.counter = 0;
    }
}
