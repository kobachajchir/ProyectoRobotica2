/*
 * encoder.c
 *
 *  Created on: Jun 13, 2025
 *      Author: kobac
 */


// encoder.c
#include "encoder.h"

void ENC_Init(ENC_Handle_t *h, const ENC_Port *port, uint32_t sampleTenMs, uint8_t calibrateCountPerStep)
{
    if (!h || !port || !port->read_count || !port->reset_count || !port->start || !port->button_level) return;
    h->port = *port;
    h->sampleTenMs  = sampleTenMs;
    h->dir          = ENC_DIR_NONE;
    h->flags.byte 	= 0;
    h->data.allData = 0;
    h->prevData.allData = 0;
    h->calibrateCountPerStep   = calibrateCountPerStep;      // 2 pulsos raw = 1 paso
    h->accumCount              = 0;
    h->prevStepDelta = 0;
    h->lastStepDelta = 0;
    h->lastStepAcc = 0;
    h->invertDirection = false;
    // Inicializar botón interno
    h->btnState.flags.byte = 0;
    h->btnState.counter    = 0;

    h->prevCount = (int16_t)h->port.read_count(h->port.ctx);
}

void ENC_Start(ENC_Handle_t *h)
{
    h->port.start(h->port.ctx);
    h->port.reset_count(h->port.ctx);
    h->prevCount = 0U;
    h->dir = ENC_DIR_NONE;
    h->flags.byte = 0U;
    h->data.allData = 0U;
    h->prevData.allData = 0U;
    h->accumCount = 0;
    h->prevStepDelta = 0;
    h->lastStepDelta = 0;
    h->lastStepAcc = 0;
}

/**
 * @brief Invierte la direccion de conteo por software sin tocar el hardware.
 */
void ENC_SetDirectionInverted(ENC_Handle_t *h, bool inverted)
{
    if (!h) {
        return;
    }

    h->invertDirection = inverted;
    h->dir = ENC_DIR_NONE;
    h->accumCount = 0;
    h->prevStepDelta = 0;
    h->lastStepDelta = 0;
    h->lastStepAcc = 0;
    h->data.allData = 0;
    h->prevData.allData = 0;

    if (h->port.read_count) {
        h->prevCount = (uint16_t)h->port.read_count(h->port.ctx);
    }
}

bool ENC_IsDirectionInverted(const ENC_Handle_t *h)
{
    return (h != NULL) ? h->invertDirection : false;
}

/**
 * @brief Se invoca cada 10 ms para actualizar el estado del pulsador del encoder.
 */
void ENC_Button_Task_10ms(ENC_Handle_t *h)
{
    ENC_ButtonState_t *btn = &h->btnState;
    bool raw = h->port.button_level(h->port.ctx);

    // Considero pressed cuando el pin está a 0
   uint8_t pressed = (raw == false);



    // Flanco de subida (abierto→cerrado)
    if (pressed && !IS_FLAG_SET(btn->flags, ENC_BTN_PREVSTATE)) {
        btn->counter = 1;
        SET_FLAG  (btn->flags, ENC_BTN_PREVSTATE);
        CLEAR_FLAG(btn->flags, ENC_BTN_SHORT_PRESS);
        CLEAR_FLAG(btn->flags, ENC_BTN_LONG_PRESS);
        NIBBLEH_SET_STATE(btn->flags, 0);
    }
    // Mantenido presionado
    else if (pressed && IS_FLAG_SET(btn->flags, ENC_BTN_PREVSTATE)) {
        if (btn->counter < UINT8_MAX) btn->counter++;
        if (btn->counter > 100) {
            btn->counter = 0;
            uint8_t ovf = NIBBLEH_GET_STATE(btn->flags);
            if (ovf < ENC_BTN_OVF_MAX) {
                ovf++;
                NIBBLEH_SET_STATE(btn->flags, ovf);
            }
        }
    }
    // Flanco de bajada (cerrado→abierto)
    else if (!pressed && IS_FLAG_SET(btn->flags, ENC_BTN_PREVSTATE)) {
        if (NIBBLEH_GET_STATE(btn->flags) == 0u) {
            if (btn->counter >= 10 && btn->counter < 30) {
                SET_FLAG(btn->flags, ENC_BTN_SHORT_PRESS);
            } else if (btn->counter >= 30 && btn->counter <= 100) {
                SET_FLAG(btn->flags, ENC_BTN_LONG_PRESS);
            }
        } else {
            /* A hold longer than one second is still one long press. */
            SET_FLAG(btn->flags, ENC_BTN_LONG_PRESS);
        }
        CLEAR_FLAG(btn->flags, ENC_BTN_PREVSTATE);
        btn->counter = 0;
    }
}


void ENC_Task_N10ms(ENC_Handle_t *h)
{
    // —— Guards para evitar null pointers ——
    if (!h) return;
    if (!h->port.read_count) return;

    // 1) Debounce y flags del botón interno
    ENC_Button_Task_10ms(h);

    // 2) Leo el contador bruto actual
    int16_t cnt = (int16_t)h->port.read_count(h->port.ctx);

    // 3) Calculo delta raw con wrap-around
    int32_t delta = (int32_t)cnt - (int32_t)h->prevCount;
    if (delta < -32768) delta += 65536;
    else if (delta >  32768) delta -= 65536;

    /* Sentido base para el montaje fisico invertido; la preferencia se aplica despues. */
    delta = -delta;

    if (h->invertDirection) {
        delta = -delta;
    }

    // 4) Actualizo prevCount para la próxima
    h->prevCount = cnt;

    // 5) Acumulo esos pulsos raw
    h->accumCount += delta;

    // 6) Extraigo pasos completos
    int32_t steps = h->accumCount / h->calibrateCountPerStep;
    h->accumCount %= h->calibrateCountPerStep;  // resto “incompleto”

    // 6.1) Guardar pasos con signo para escalado
    h->lastStepDelta = (int16_t)steps;
    h->lastStepAcc = (int16_t)(steps - h->prevStepDelta);
    h->prevStepDelta = (int16_t)steps;

    // 7) Velocidad y aceleración en pasos
    h->data.pair.vel      = (uint16_t)steps;
    h->data.pair.acc      = (uint16_t)(steps - (int16_t)h->prevData.pair.vel);
    h->prevData.allData   = h->data.allData;

    // 8) Marco update y dirección
    if (steps != 0) {
        SET_FLAG(h->flags, ENC_FLAG_UPDATED);
        h->dir = (steps > 0 ? ENC_DIR_CW : ENC_DIR_CCW);
    } else {
        h->dir = ENC_DIR_NONE;
    }
}


ENC_Direction_t ENC_GetDirection(ENC_Handle_t *h)
{
    return h->dir;
}

uint16_t ENC_GetVelocity(ENC_Handle_t *h)
{
    return h->data.pair.vel;
}

uint16_t ENC_GetAcceleration(ENC_Handle_t *h)
{
    return h->data.pair.acc;
}

int16_t ENC_GetStepDelta(ENC_Handle_t *h)
{
    return h->lastStepDelta;
}

int16_t ENC_GetSignedVelocity(ENC_Handle_t *h)
{
    return h->lastStepDelta;
}

int16_t ENC_GetSignedAcceleration(ENC_Handle_t *h)
{
    return h->lastStepAcc;
}

int16_t ENC_GetScaledSteps(ENC_Handle_t *h)
{
    int16_t steps = h->lastStepDelta;
    int16_t abs_steps = (steps < 0) ? (int16_t)(-steps) : steps;
    int16_t multiplier = 1;

    if (abs_steps >= 4) {
        multiplier = 4;
    } else if (abs_steps >= 2) {
        multiplier = 2;
    }

    if (steps < 0) {
        multiplier = (int16_t)(-multiplier);
    }

    return multiplier;
}
