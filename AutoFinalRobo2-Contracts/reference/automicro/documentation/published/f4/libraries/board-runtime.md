# Arranque de placa e interrupciones STM32

ID: `f4:module/Core/Src/main.c` · Capa: **plataforma** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Inicializa reloj, periféricos y bucle de aplicación y conecta vectores de interrupción a HAL.

Código generado/de plataforma para STM32F411CEU6; CubeMX/.ioc permanece fuente del pinout. No se confunde con las librerías reutilizables.

<a id="modularity"></a>
## Modularidad observada

Dependencia directa STM32/HAL/CMSIS esperada. Su portabilidad requiere reemplazar inicialización, IRQ y adaptadores.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Handles HAL de periféricos, estado del runtime C y recursos estáticos pertenecen a la placa; los puertos neutrales los reciben como referencias opacas.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- main inicializa plataforma antes de APP_Init y atiende el bucle.
- IRQs llaman HAL y los callbacks de aplicación; evitar trabajo bloqueante arbitrario dentro de IRQ.
- Error_Handler y faults describen rutas de fallo de plataforma, no estados recuperables del protocolo.

## Trabajo en tándem

- app_tasks.c es el puente hacia librerías; los módulos neutrales no deben importar main.h.

## Precauciones y límites

- No se compiló ni cargó firmware; no se modifica configuración CubeMX.
- Vendor y startup/linker fuera de Core/USB_DEVICE están excluidos explícitamente de esta extracción.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/main.h` | `stm32f4xx_hal.h` | `365bac6e7e69041db8055e56adaf758c8cd6c5e7a6bfa75cc2b51f1d5d0de959` |
| `Core/Inc/stm32f4xx_it.h` |  | `c12e6de8c0098590467b665024ab89057561b472ef1b4d1c4951d90295eac890` |
| `Core/Inc/stm32f4xx_hal_conf.h` | `stm32f4xx_hal_rcc.h`, `stm32f4xx_hal_gpio.h`, `stm32f4xx_hal_exti.h`, `stm32f4xx_hal_dma.h`, `stm32f4xx_hal_cortex.h`, `stm32f4xx_hal_adc.h`, `stm32f4xx_hal_can.h`, `stm32f4xx_hal_can_legacy.h`, `stm32f4xx_hal_crc.h`, `stm32f4xx_hal_cryp.h`, `stm32f4xx_hal_dma2d.h`, `stm32f4xx_hal_dac.h`, `stm32f4xx_hal_dcmi.h`, `stm32f4xx_hal_eth.h`, `stm32f4xx_hal_eth_legacy.h`, `stm32f4xx_hal_flash.h`, `stm32f4xx_hal_sram.h`, `stm32f4xx_hal_nor.h`, `stm32f4xx_hal_nand.h`, `stm32f4xx_hal_pccard.h`, `stm32f4xx_hal_sdram.h`, `stm32f4xx_hal_hash.h`, `stm32f4xx_hal_i2c.h`, `stm32f4xx_hal_smbus.h`, `stm32f4xx_hal_i2s.h`, `stm32f4xx_hal_iwdg.h`, `stm32f4xx_hal_ltdc.h`, `stm32f4xx_hal_pwr.h`, `stm32f4xx_hal_rng.h`, `stm32f4xx_hal_rtc.h`, `stm32f4xx_hal_sai.h`, `stm32f4xx_hal_sd.h`, `stm32f4xx_hal_spi.h`, `stm32f4xx_hal_tim.h`, `stm32f4xx_hal_uart.h`, `stm32f4xx_hal_usart.h`, `stm32f4xx_hal_irda.h`, `stm32f4xx_hal_smartcard.h`, `stm32f4xx_hal_wwdg.h`, `stm32f4xx_hal_pcd.h`, `stm32f4xx_hal_hcd.h`, `stm32f4xx_hal_dsi.h`, `stm32f4xx_hal_qspi.h`, `stm32f4xx_hal_cec.h`, `stm32f4xx_hal_fmpi2c.h`, `stm32f4xx_hal_fmpsmbus.h`, `stm32f4xx_hal_spdifrx.h`, `stm32f4xx_hal_dfsdm.h`, `stm32f4xx_hal_lptim.h`, `stm32f4xx_hal_mmc.h` | `3717c97421d96025653534a6652b7fb137e3fa17375345d170fbdd3587c9577b` |
| `Core/Inc/app_version.h` |  | `2d2621b37fce9aefa081e6dcf3a12c35b812e8db1a5b325110c221bfe1b3c5f5` |
| `Core/Src/main.c` | `main.h`, `usb_device.h`, `app_tasks.h`, `app_preferences_binding.h`, `app_preferences_stm32.h`, `tcrt5000.h` | `363b600e2687ea080f5eb56af66a379479e7e2d5359e6a15ca4bf54ca84a969b` |
| `Core/Src/stm32f4xx_it.c` | `main.h`, `stm32f4xx_it.h`, `app_wifi_binding.h` | `b3ace6719bc74b20eddfab88110cc0ae2c2946a6c57c38c2930c9d7c589c47cc` |
| `Core/Src/stm32f4xx_hal_msp.c` | `main.h` | `5614c2508675e4747d3d3c350f3e5e0aee1dd8f78e44751d6a9077d7018e9ab3` |
| `Core/Src/system_stm32f4xx.c` | `stm32f4xx.h` | `a3b81129e52ae083587238d759a5b9d7072e37326af5c7542b73b03936333cba` |
| `Core/Src/syscalls.c` | `sys/stat.h`, `stdlib.h`, `errno.h`, `stdio.h`, `signal.h`, `time.h`, `sys/time.h`, `sys/times.h` | `b993ebb4f882e8be21cec8d95983655a0caf4f0eb11348f20f6371f5a5031717` |
| `Core/Src/sysmem.c` | `errno.h`, `stdint.h` | `c41145817cc20abd8881c38851745d6e509e5f8c44a5179b125d108e51d1d432` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-busfault_handler"></a>`void BusFault_Handler(void)` | Operación Bus Fault Handler de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_it.h:52`; inferred-name |
| <a id="fn-dma1_stream0_irqhandler"></a>`void DMA1_Stream0_IRQHandler(void)` | Adapta la llamada a `HAL_DMA_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:58`; structural |
| <a id="fn-dma1_stream3_irqhandler"></a>`void DMA1_Stream3_IRQHandler(void)` | Adapta la llamada a `HAL_DMA_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:59`; structural |
| <a id="fn-dma1_stream4_irqhandler"></a>`void DMA1_Stream4_IRQHandler(void)` | Adapta la llamada a `HAL_DMA_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:60`; structural |
| <a id="fn-dma1_stream6_irqhandler"></a>`void DMA1_Stream6_IRQHandler(void)` | Adapta la llamada a `HAL_DMA_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:61`; structural |
| <a id="fn-dma2_stream0_irqhandler"></a>`void DMA2_Stream0_IRQHandler(void)` | Adapta la llamada a `HAL_DMA_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:67`; structural |
| <a id="fn-dma2_stream2_irqhandler"></a>`void DMA2_Stream2_IRQHandler(void)` | Adapta la llamada a `HAL_DMA_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:68`; structural |
| <a id="fn-dma2_stream7_irqhandler"></a>`void DMA2_Stream7_IRQHandler(void)` | Adapta la llamada a `HAL_DMA_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:70`; structural |
| <a id="fn-debugmon_handler"></a>`void DebugMon_Handler(void)` | Operación Debug Mon Handler de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_it.h:55`; inferred-name |
| <a id="fn-exti9_5_irqhandler"></a>`void EXTI9_5_IRQHandler(void)` | Adapta la llamada a `HAL_GPIO_EXTI_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:62`; structural |
| <a id="fn-error_handler"></a>`void Error_Handler(void)` | Entra al manejo de fallo no recuperable de plataforma. | `f4:Core/Inc/main.h:55`; editorial |
| <a id="fn-hal_adc_mspdeinit"></a>`void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)` | Coordina `HAL_DMA_DeInit`, `HAL_GPIO_DeInit`, `__HAL_RCC_ADC1_CLK_DISABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:167`; structural |
| <a id="fn-hal_adc_mspinit"></a>`void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)` | Coordina `Error_Handler`, `HAL_DMA_Init`, `HAL_GPIO_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:100`; structural |
| <a id="fn-hal_i2c_mspdeinit"></a>`void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)` | Coordina `HAL_DMA_DeInit`, `HAL_GPIO_DeInit`, `HAL_NVIC_DisableIRQ` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:287`; structural |
| <a id="fn-hal_i2c_mspinit"></a>`void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)` | Coordina `Error_Handler`, `HAL_DMA_Init`, `HAL_GPIO_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:207`; structural |
| <a id="fn-hal_mspinit"></a>`void HAL_MspInit(void)` | Coordina `__HAL_RCC_PWR_CLK_ENABLE`, `__HAL_RCC_SYSCFG_CLK_ENABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:77`; structural |
| <a id="fn-hal_spi_mspdeinit"></a>`void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)` | Coordina `HAL_DMA_DeInit`, `HAL_GPIO_DeInit`, `__HAL_RCC_SPI2_CLK_DISABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:400`; structural |
| <a id="fn-hal_spi_mspinit"></a>`void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)` | Coordina `Error_Handler`, `HAL_DMA_Init`, `HAL_GPIO_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:325`; structural |
| <a id="fn-hal_tim_base_mspdeinit"></a>`void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)` | Coordina `HAL_NVIC_DisableIRQ`, `__HAL_RCC_TIM2_CLK_DISABLE`, `__HAL_RCC_TIM3_CLK_DISABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:543`; structural |
| <a id="fn-hal_tim_base_mspinit"></a>`void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)` | Coordina `HAL_NVIC_EnableIRQ`, `HAL_NVIC_SetPriority`, `__HAL_RCC_TIM2_CLK_ENABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:433`; structural |
| <a id="fn-hal_tim_encoder_mspdeinit"></a>`void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef* htim_encoder)` | Coordina `HAL_GPIO_DeInit`, `__HAL_RCC_TIM4_CLK_DISABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:579`; structural |
| <a id="fn-hal_tim_encoder_mspinit"></a>`void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_encoder)` | Coordina `HAL_GPIO_Init`, `__HAL_RCC_GPIOB_CLK_ENABLE`, `__HAL_RCC_TIM4_CLK_ENABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:469`; structural |
| <a id="fn-hal_tim_msppostinit"></a>`void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim)` | Coordina `HAL_GPIO_Init`, `__HAL_RCC_GPIOA_CLK_ENABLE`, `__HAL_RCC_GPIOB_CLK_ENABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Inc/main.h:52`; structural |
| <a id="fn-hal_uart_mspdeinit"></a>`void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)` | Coordina `HAL_DMA_DeInit`, `HAL_GPIO_DeInit`, `HAL_NVIC_DisableIRQ` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:685`; structural |
| <a id="fn-hal_uart_mspinit"></a>`void HAL_UART_MspInit(UART_HandleTypeDef* huart)` | Coordina `Error_Handler`, `HAL_DMA_Init`, `HAL_GPIO_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/stm32f4xx_hal_msp.c:608`; structural |
| <a id="fn-hardfault_handler"></a>`void HardFault_Handler(void)` | Operación Hard Fault Handler de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_it.h:50`; inferred-name |
| <a id="fn-i2c1_er_irqhandler"></a>`void I2C1_ER_IRQHandler(void)` | Adapta la llamada a `HAL_I2C_ER_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:65`; structural |
| <a id="fn-i2c1_ev_irqhandler"></a>`void I2C1_EV_IRQHandler(void)` | Adapta la llamada a `HAL_I2C_EV_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:64`; structural |
| <a id="fn-memmanage_handler"></a>`void MemManage_Handler(void)` | Operación Mem Manage Handler de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_it.h:51`; inferred-name |
| <a id="fn-nmi_handler"></a>`void NMI_Handler(void)` | Operación NMI Handler de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_it.h:49`; inferred-name |
| <a id="fn-otg_fs_irqhandler"></a>`void OTG_FS_IRQHandler(void)` | Adapta la llamada a `HAL_PCD_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:69`; structural |
| <a id="fn-pendsv_handler"></a>`void PendSV_Handler(void)` | Operación Pend SV Handler de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_it.h:56`; inferred-name |
| <a id="fn-svc_handler"></a>`void SVC_Handler(void)` | Operación SVC Handler de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_it.h:54`; inferred-name |
| <a id="fn-systick_handler"></a>`void SysTick_Handler(void)` | Adapta la llamada a `HAL_IncTick` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:57`; structural |
| <a id="fn-systemclock_config"></a>`void SystemClock_Config(void)` | Coordina `Error_Handler`, `HAL_RCC_ClockConfig`, `HAL_RCC_OscConfig` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:71`; structural |
| <a id="fn-systemcoreclockupdate"></a>`void SystemCoreClockUpdate(void)` | Operación System Core Clock Update de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/system_stm32f4xx.c:220`; inferred-name |
| <a id="fn-systeminit"></a>`void SystemInit(void)` | Coordina `SystemInit_ExtMemCtl`, `defined` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/system_stm32f4xx.c:167`; structural |
| <a id="fn-systeminit_extmemctl"></a>`void SystemInit_ExtMemCtl(void)` | Coordina `READ_BIT`, `defined`, `endif` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/system_stm32f4xx.c:279`; structural |
| <a id="fn-tim3_irqhandler"></a>`void TIM3_IRQHandler(void)` | Adapta la llamada a `HAL_TIM_IRQHandler` con el contexto de esta integración. | `f4:Core/Inc/stm32f4xx_it.h:63`; structural |
| <a id="fn-usart1_irqhandler"></a>`void USART1_IRQHandler(void)` | Coordina `HAL_UART_IRQHandler`, `WifiBridge_OnIdleIrq`, `__HAL_UART_CLEAR_IDLEFLAG` para Arranque de placa e interrupciones STM32. | `f4:Core/Inc/stm32f4xx_it.h:66`; structural |
| <a id="fn-usagefault_handler"></a>`void UsageFault_Handler(void)` | Operación Usage Fault Handler de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_it.h:53`; inferred-name |
| <a id="fn-__io_getchar"></a>`extern int __io_getchar(void) __attribute__((weak))` | Operación io getchar de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Src/syscalls.c:36`; inferred-name |
| <a id="fn-__io_putchar"></a>`extern int __io_putchar(int ch) __attribute__((weak))` | Operación io putchar de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Src/syscalls.c:35`; inferred-name |
| <a id="fn-_close"></a>`int _close(int file)` | Operación close de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:92`; inferred-name |
| <a id="fn-_execve"></a>`int _execve(char *name, char **argv, char **env)` | Operación execve de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:169`; inferred-name |
| <a id="fn-_exit"></a>`void _exit (int status)` | Adapta la llamada a `_kill` con el contexto de esta integración. | `f4:Core/Src/syscalls.c:61`; structural |
| <a id="fn-_fork"></a>`int _fork(void)` | Operación fork de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:163`; inferred-name |
| <a id="fn-_fstat"></a>`int _fstat(int file, struct stat *st)` | Operación fstat de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:99`; inferred-name |
| <a id="fn-_getpid"></a>`int _getpid(void)` | Operación getpid de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:48`; inferred-name |
| <a id="fn-_isatty"></a>`int _isatty(int file)` | Operación isatty de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:106`; inferred-name |
| <a id="fn-_kill"></a>`int _kill(int pid, int sig)` | Operación kill de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:53`; inferred-name |
| <a id="fn-_link"></a>`int _link(char *old, char *new)` | Operación link de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:155`; inferred-name |
| <a id="fn-_lseek"></a>`int _lseek(int file, int ptr, int dir)` | Operación lseek de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:112`; inferred-name |
| <a id="fn-_open"></a>`int _open(char *path, int flags, ...)` | Operación open de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:120`; inferred-name |
| <a id="fn-_sbrk"></a>`void *_sbrk(ptrdiff_t incr)` | Operación sbrk de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/sysmem.c:53`; inferred-name |
| <a id="fn-_stat"></a>`int _stat(char *file, struct stat *st)` | Operación stat de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:148`; inferred-name |
| <a id="fn-_times"></a>`int _times(struct tms *buf)` | Operación times de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:142`; inferred-name |
| <a id="fn-_unlink"></a>`int _unlink(char *name)` | Operación unlink de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:135`; inferred-name |
| <a id="fn-_wait"></a>`int _wait(int *status)` | Operación wait de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:128`; inferred-name |
| <a id="fn-assert_failed"></a>`void assert_failed(uint8_t* file, uint32_t line)` | Operación assert failed de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Inc/stm32f4xx_hal_conf.h:486`; inferred-name |
| <a id="fn-initialise_monitor_handles"></a>`void initialise_monitor_handles()` | Operación initialise monitor handles de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/syscalls.c:44`; inferred-name |
| <a id="fn-main"></a>`int main(void)` | Inicializa plataforma y entra al ciclo principal de la aplicación. | `f4:Core/Src/main.c:94`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-mx_adc1_init"></a>`static void MX_ADC1_Init(void)` | Coordina `Error_Handler`, `HAL_ADC_ConfigChannel`, `HAL_ADC_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:197`; structural |
| <a id="fn-mx_i2c1_init"></a>`static void MX_I2C1_Init(void)` | Coordina `Error_Handler`, `HAL_I2C_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:312`; structural |
| <a id="fn-mx_spi2_init"></a>`static void MX_SPI2_Init(void)` | Coordina `Error_Handler`, `HAL_SPI_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:346`; structural |
| <a id="fn-mx_tim2_init"></a>`static void MX_TIM2_Init(void)` | Coordina `Error_Handler`, `HAL_TIMEx_MasterConfigSynchronization`, `HAL_TIM_Base_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:384`; structural |
| <a id="fn-mx_tim3_init"></a>`static void MX_TIM3_Init(void)` | Coordina `Error_Handler`, `HAL_TIMEx_MasterConfigSynchronization`, `HAL_TIM_Base_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:455`; structural |
| <a id="fn-mx_tim4_init"></a>`static void MX_TIM4_Init(void)` | Coordina `Error_Handler`, `HAL_TIMEx_MasterConfigSynchronization`, `HAL_TIM_Encoder_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:500`; structural |
| <a id="fn-mx_usart1_uart_init"></a>`static void MX_USART1_UART_Init(void)` | Coordina `Error_Handler`, `HAL_UART_Init` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:549`; structural |
| <a id="fn-mx_dma_init"></a>`static void MX_DMA_Init(void)` | Coordina `HAL_NVIC_EnableIRQ`, `HAL_NVIC_SetPriority`, `__HAL_RCC_DMA1_CLK_ENABLE` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:580`; structural |
| <a id="fn-mx_gpio_init"></a>`static void MX_GPIO_Init(void)` | Coordina `AppPreferences_GetExtProfile`, `HAL_GPIO_Init`, `HAL_GPIO_WritePin` para Arranque de placa e interrupciones STM32. | `f4:Core/Src/main.c:617`; structural |
| <a id="fn-systeminit_extmemctl"></a>`static void SystemInit_ExtMemCtl(void)` | Operación System Init Ext Mem Ctl de Arranque de placa e interrupciones STM32; semántica fina pendiente de ampliación. | `f4:Core/Src/system_stm32f4xx.c:149`; inferred-name |
