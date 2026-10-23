#ifndef APP_I2C_PORTS_H
#define APP_I2C_PORTS_H
#include "main.h"
#include "i2c_manager.h"
static inline uint32_t App_I2cMs(void *ctx) { (void)ctx; return HAL_GetTick(); }
static inline uint32_t App_I2cUs(void *ctx) { return App_I2cMs(ctx)*1000u; }
static inline void App_I2cDelay(void *ctx,uint32_t ms) { (void)ctx; HAL_Delay(ms); }
static inline bool App_I2cReady(void *ctx) {
    I2C_HandleTypeDef *i2c=ctx;
    return HAL_I2C_GetState(i2c)==HAL_I2C_STATE_READY &&
           __HAL_I2C_GET_FLAG(i2c,I2C_FLAG_BUSY)==RESET;
}
static inline AM_Result App_I2cProbe(void *ctx,uint16_t a,uint32_t n,uint32_t t) { return HAL_I2C_IsDeviceReady(ctx,a,n,t); }
static inline AM_Result App_I2cWrite(void *ctx,uint16_t a,uint16_t r,uint16_t rs,uint8_t *p,uint16_t n,uint32_t t) { return HAL_I2C_Mem_Write(ctx,a,r,rs,p,n,t); }
static inline AM_Result App_I2cRead(void *ctx,uint16_t a,uint16_t r,uint16_t rs,uint8_t *p,uint16_t n,uint32_t t) { return HAL_I2C_Mem_Read(ctx,a,r,rs,p,n,t); }
static inline AM_Result App_I2cWriteDma(void *ctx,uint16_t a,uint16_t r,uint16_t rs,uint8_t *p,uint16_t n) { return HAL_I2C_Mem_Write_DMA(ctx,a,r,rs,p,n); }
static inline AM_Result App_I2cTx(void *ctx,uint16_t a,uint8_t *p,uint16_t n) { return HAL_I2C_Master_Transmit_DMA(ctx,a,p,n); }
static inline AM_Result App_I2cRx(void *ctx,uint16_t a,uint8_t *p,uint16_t n) { return HAL_I2C_Master_Receive_DMA(ctx,a,p,n); }
static const AM_I2C_Hardware appI2cHardware={App_I2cMs,App_I2cUs,App_I2cDelay,App_I2cReady,App_I2cProbe,App_I2cWrite,App_I2cRead,App_I2cWriteDma,App_I2cTx,App_I2cRx};
/* Main context only. Reuse CubeMX MSP setup and the existing I2C init values. */
static inline void App_I2cRecover(void *ctx) {
    I2C_HandleTypeDef *i2c=ctx;
    if (i2c->Instance!=I2C1) return;
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Stream6_IRQn);
    __HAL_I2C_DISABLE_IT(i2c,I2C_IT_EVT|I2C_IT_BUF|I2C_IT_ERR);
    CLEAR_BIT(i2c->Instance->CR2,I2C_CR2_DMAEN);
    __HAL_I2C_DISABLE(i2c);
    if (i2c->hdmatx && HAL_DMA_GetState(i2c->hdmatx)==HAL_DMA_STATE_BUSY)
        (void)HAL_DMA_Abort(i2c->hdmatx);
    if (i2c->hdmarx && HAL_DMA_GetState(i2c->hdmarx)==HAL_DMA_STATE_BUSY)
        (void)HAL_DMA_Abort(i2c->hdmarx);
    (void)HAL_I2C_DeInit(i2c);
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    HAL_NVIC_ClearPendingIRQ(I2C1_EV_IRQn);
    HAL_NVIC_ClearPendingIRQ(I2C1_ER_IRQn);
    HAL_NVIC_ClearPendingIRQ(DMA1_Stream0_IRQn);
    HAL_NVIC_ClearPendingIRQ(DMA1_Stream6_IRQn);
    (void)HAL_I2C_Init(i2c);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}
typedef struct { I2C_ManagerHandle *manager; I2C_DeviceID id; AM_I2C_DevicePort port; } AppI2cBinding;
static inline AM_Result App_I2cBind(void *ctx,uint16_t a,uint8_t p,void *c,I2C_OnGrantedCb g,I2C_OnTxDoneCb tx,I2C_OnRxDoneCb rx,I2C_OnErrorCb e) {
    AppI2cBinding *b=ctx; return I2C_Manager_RegisterDevice(b->manager,b->id,a,p,c,g,tx,rx,e);
}
static inline AM_Result App_I2cRequest(void *ctx,I2C_RequestPrio p) { AppI2cBinding *b=ctx; return I2C_Manager_RequestBusPrio(b->manager,b->id,p); }
static inline AM_Result App_I2cRelease(void *ctx) { AppI2cBinding *b=ctx; return I2C_Manager_ReleaseBus(b->manager,b->id); }
static inline bool App_I2cOwner(void *ctx) { AppI2cBinding *b=ctx; return I2C_Manager_GetOwner(b->manager)==b->id; }
static inline I2C_PolicyMode App_I2cPolicy(void *ctx) { AppI2cBinding *b=ctx; return I2C_Manager_GetPolicy(b->manager); }
static inline void App_I2cTransferFailed(void *ctx) {
    AppI2cBinding *b=ctx;
    if (I2C_Manager_GetOwner(b->manager)==b->id)
        I2C_Manager_OnError(b->manager,b->manager->hi2c);
}
static inline void App_I2cConnect(AppI2cBinding *b,I2C_ManagerHandle *m,I2C_DeviceID id) {
    b->manager=m; b->id=id;
    b->port=(AM_I2C_DevicePort){b,m->hardware,m->hi2c,App_I2cBind,App_I2cRequest,App_I2cRelease,App_I2cOwner,App_I2cPolicy,App_I2cTransferFailed};
}
#endif
