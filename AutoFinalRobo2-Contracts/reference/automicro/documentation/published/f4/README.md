# Bibliotecas y composición de FirmwareF4

Fotografía de trabajo: 2026-10-20. Revisión documental por lectura, sin ejecución de firmware.

Ver [arquitectura y límites](architecture.md), [metodología](../../METHOD.md) y [contratos entre repositorios](../shared/wire-contracts.md).

| Librería / unidad | Capa | Funciones externas | Internas |
| --- | --- | --- |
| [Contratos neutrales de plataforma y control](libraries/ports.md) | contrato | 0 | 0 |
| [Configuración deseada y aplicación de AP](libraries/ap-configuration.md) | integración | 8 | 3 |
| [Preferencias y persistencia](libraries/preferences.md) | servicio | 67 | 51 |
| [Adaptadores de hardware y servicios](libraries/application-adapters.md) | plataforma | 139 | 0 |
| [Composición, tareas y acciones de aplicación](libraries/application-composition.md) | composición | 160 | 261 |
| [GOTO y calibración de motores](libraries/auto-motion.md) | núcleo | 10 | 13 |
| [Contrato de arranque y mailbox](libraries/boot-mailbox.md) | plataforma | 6 | 0 |
| [Balance de inclinación](libraries/balance.md) | núcleo | 9 | 2 |
| [Encoder rotativo y pulsador integrado](libraries/encoder.md) | núcleo | 13 | 0 |
| [Fuentes, tipos y recursos auxiliares](libraries/fonts-resources.md) | datos | 0 | 0 |
| [I2C Manager](libraries/i2c-manager.md) | servicio | 14 | 5 |
| [Calibración de línea y seguidor](libraries/line-follow.md) | núcleo | 15 | 8 |
| [Favoritos de menú](libraries/favorites.md) | integración | 5 | 2 |
| [Sistema de menús](libraries/menus.md) | núcleo | 26 | 12 |
| [Árbitro de movimiento](libraries/motion.md) | núcleo | 10 | 5 |
| [Estimador de movimiento relativo](libraries/motion-estimator.md) | núcleo | 6 | 4 |
| [Motores y tracción diferencial](libraries/motors.md) | núcleo | 21 | 2 |
| [MPU9250 y estimación de actitud](libraries/mpu9250.md) | núcleo | 37 | 32 |
| [Transferencia transaccional OLED Canvas](libraries/oled-canvas.md) | integración | 8 | 20 |
| [Renderers y cola de notificaciones](libraries/screen-renderers.md) | integración | 123 | 102 |
| [Teclado en pantalla OSK](libraries/osk.md) | núcleo | 11 | 21 |
| [Integración del teclado con UI](libraries/osk-integration.md) | integración | 7 | 16 |
| [Permisos y sesiones PIN](libraries/permissions.md) | integración | 23 | 27 |
| [Controlador PID](libraries/pid.md) | núcleo | 10 | 3 |
| [Preparación y navegación de pantallas](libraries/screen-wrappers.md) | integración | 54 | 9 |
| [Contexto y servicios de pantallas](libraries/screen-context.md) | integración | 6 | 0 |
| [Bloqueo de pantalla por inactividad](libraries/screen-lock.md) | integración | 12 | 3 |
| [OLED SSD1306](libraries/ssd1306.md) | núcleo | 44 | 21 |
| [Sensores IR TCRT5000](libraries/infrared.md) | núcleo | 24 | 4 |
| [Router de eventos de usuario](libraries/ui-router.md) | integración | 1 | 0 |
| [UNER de aplicación y contrato del robot](libraries/uner-application.md) | integración | 31 | 114 |
| [Parser y cola UNER v2](libraries/uner-core.md) | núcleo | 6 | 4 |
| [UNER Handle, rutas y despacho](libraries/uner-router.md) | núcleo | 12 | 12 |
| [Transporte UNER UART1 DMA](libraries/uart-transport.md) | plataforma | 2 | 4 |
| [Transporte UNER USB CDC](libraries/usb-transport.md) | plataforma | 13 | 11 |
| [Botón de usuario](libraries/user-button.md) | núcleo | 2 | 0 |
| [Bridge Wi-Fi y backends UNER/AT](libraries/wifi-bridge.md) | integración | 105 | 49 |
| [Arranque de placa e interrupciones STM32](libraries/board-runtime.md) | plataforma | 62 | 10 |
| [Plataforma USB CDC/MSC](libraries/usb-platform.md) | plataforma | 44 | 15 |

Los tipos auxiliares compartidos y el código de placa figuran en `source-map.json`. Los encabezados de puertos se incluyen en sus fichas consumidoras o en la ficha de adaptadores. Vendor HAL/CMSIS, middleware ST, bootloader separado, binarios y firmware F103 quedan fuera del manual de librerías propias.
