#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_
#include "encoder_types.h"
#ifdef __cplusplus
extern "C" {
#endif
void ENC_Init(ENC_Handle_t *h, const ENC_Port *port, uint32_t sampleTenMs, uint8_t calibrateCountPerStep);
/**
 * @brief Arranca el encoder y resetea contadores
 */
void ENC_Start(ENC_Handle_t *h);

void ENC_SetDirectionInverted(ENC_Handle_t *h, bool inverted);
bool ENC_IsDirectionInverted(const ENC_Handle_t *h);

/**
 * @brief Se invoca cada 10 ms
 *        Lee PIN y actualiza flags, counter y overflow count (nibble alto).
 * @param btn Puntero a la estructura ButtonState_t
 */
void ENC_Button_Task_10ms(ENC_Handle_t *h);

/**
 * @brief Tarea de muestreo, llamar cada sampleTenMs*10 ms
 *        Realiza debounce de botón e actualiza vel/acc/dir
 */
void ENC_Task_N10ms(ENC_Handle_t *h);

/**
 * @brief Obtiene la dirección de giro
 */
ENC_Direction_t ENC_GetDirection(ENC_Handle_t *h);

/**
 * @brief Obtiene la velocidad (pulsos / intervalo)
 */
uint16_t ENC_GetVelocity(ENC_Handle_t *h);

/**
 * @brief Obtiene la aceleración (delta pulsos / intervalo)
 */
uint16_t ENC_GetAcceleration(ENC_Handle_t *h);

int16_t ENC_GetStepDelta(ENC_Handle_t *h);
int16_t ENC_GetSignedVelocity(ENC_Handle_t *h);
int16_t ENC_GetSignedAcceleration(ENC_Handle_t *h);
int16_t ENC_GetScaledSteps(ENC_Handle_t *h);

#ifdef __cplusplus
}
#endif

#endif /* INC_ENCODER_H_ */
