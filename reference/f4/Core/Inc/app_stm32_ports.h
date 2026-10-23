#ifndef APP_STM32_PORTS_H
#define APP_STM32_PORTS_H
#include "main.h"
#include "user_button.h"
#include "encoder.h"
#include "motor_control.h"
#include "tcrt5000.h"
typedef struct { GPIO_TypeDef *gpio; uint16_t pin; } AppGpioContext;
typedef struct { TIM_HandleTypeDef *timer; AppGpioContext button; } AppEncoderContext;
static inline bool App_ReadLevel(void *ctx) {
    AppGpioContext *p=ctx; return HAL_GPIO_ReadPin(p->gpio,p->pin)==GPIO_PIN_SET;
}
static inline uint16_t App_EncoderCount(void *ctx) {
    AppEncoderContext *p=ctx; return (uint16_t)__HAL_TIM_GET_COUNTER(p->timer);
}
static inline void App_EncoderReset(void *ctx) {
    AppEncoderContext *p=ctx; __HAL_TIM_SET_COUNTER(p->timer,0);
}
static inline void App_EncoderStart(void *ctx) {
    AppEncoderContext *p=ctx; HAL_TIM_Encoder_Start(p->timer,TIM_CHANNEL_ALL);
}
static inline bool App_EncoderButton(void *ctx) {
    AppEncoderContext *p=ctx; return App_ReadLevel(&p->button);
}
static inline AM_Result App_PwmStart(void *ctx,uint32_t channel) {
    return (AM_Result)HAL_TIM_PWM_Start(ctx,channel);
}
static inline void App_PwmCompare(void *ctx,uint32_t channel,uint16_t value) {
    TIM_HandleTypeDef *timer=ctx; __HAL_TIM_SET_COMPARE(timer,channel,value);
}
static const Motor_Port appMotorPort={App_PwmStart,App_PwmCompare};
typedef struct { AppGpioContext objects, line; ADC_HandleTypeDef *adc; volatile uint16_t *samples; uint32_t count; } AppIrContext;
static inline void App_IrEmitters(void *ctx,uint8_t group,bool enabled) {
    AppIrContext *p=ctx;
    const AppGpioContext *pins=group==5?&p->objects:&p->line;
    HAL_GPIO_WritePin(pins->gpio,pins->pin,enabled?GPIO_PIN_RESET:GPIO_PIN_SET);
}
static inline AM_Result App_IrInitialize(void *ctx) {
    App_IrEmitters(ctx,5,false); App_IrEmitters(ctx,3,false);
    return AM_OK;
}
static inline AM_Result App_IrStartSampling(AppIrContext *p) {
    return HAL_ADC_Start_DMA(p->adc,(uint32_t *)p->samples,p->count);
}

#endif
