#ifndef AFR2_I2C_MANAGER_H
#define AFR2_I2C_MANAGER_H
#include <stdbool.h>
#include <stdint.h>
/* Adaptacion conceptual de I2C_Manager F4: propiedad, deadline y callback en main.
 * API reducida para un solo OLED. No es sustituto ABI del manager original.
 * Todos los metodos se llaman en main. ISR del backend solo publica estado.
 * El backend conserva data hasta DONE/ERROR o abort; nunca guarda stack temporal.
 */
typedef enum { AFR2_I2C_OK, AFR2_I2C_BUSY, AFR2_I2C_ERROR, AFR2_I2C_TIMEOUT } Afr2I2cResult;
typedef struct {
    void *ctx;
    Afr2I2cResult (*start)(void *ctx,uint8_t address7,const uint8_t *data,uint16_t size);
    Afr2I2cResult (*poll)(void *ctx); /* BUSY o resultado, solo tras STOP completado */
    void (*abort)(void *ctx); /* desactiva IRQ/transferencia antes de retornar; sin espera */
} Afr2I2cHardware;
typedef void (*Afr2I2cDone)(void *ctx,Afr2I2cResult result);
typedef struct {
    Afr2I2cHardware hardware;
    Afr2I2cDone done;
    void *client;
    uint32_t started_ms;
    uint16_t timeout_ms;
    bool busy, configured;
} Afr2I2cManager;
bool Afr2I2c_Init(Afr2I2cManager *m,const Afr2I2cHardware *hw);
Afr2I2cResult Afr2I2c_Submit(Afr2I2cManager *m,uint8_t address7,const uint8_t *data,
    uint16_t size,uint32_t now,uint16_t timeout_ms,Afr2I2cDone done,void *client);
void Afr2I2c_Tick(Afr2I2cManager *m,uint32_t now);
#endif
