# FirmwareF4: revisión de modularidad y portabilidad

Fecha: 2026-10-03. Revisión por lectura del checkout local; no corresponde a una revisión fijada por commit. Sin tests, compilaciones, validadores ni ensayo físico. No se modificó código de FirmwareF4 ni de los consumidores.

## Conclusión

El código actual no cumple el requisito de que todas las librerías de dispositivos y software sean independientes entre sí, portables y no bloqueantes. Tener handles o usar DMA no elimina una dependencia concreta ni convierte toda la inicialización en asíncrona.

Aquí HAL significa una interfaz de abstracción inyectada. STM32 HAL queda dentro del adaptador de plataforma; las interfaces públicas de las librerías portables no deben necesitar `GPIO_TypeDef`, `TIM_HandleTypeDef`, `I2C_HandleTypeDef` ni `HAL_StatusTypeDef`.

El `.ioc` identifica STM32F411CEU6, ocho conversiones ADC y grupos IR en PC14/PC15. Esa configuración pertenece a la placa, no a los núcleos reutilizables. El firmware seguirá usando STM32F4 HAL/CMSIS en su integración.

## Inventario de hallazgos

Las rutas siguientes son relativas a FirmwareF4. La ausencia de una espera identificada no acredita tiempos de ejecución ni garantiza que un callback externo sea no bloqueante.

| Módulo | Evidencia en fuente | Separación necesaria |
| --- | --- | --- |
| Encoder y su pulsador | `Core/Inc/encoder.h` expone timer y GPIO; `Core/Src/encoder.c` arranca el timer, lee su contador y GPIO con HAL. Incluye `__NOP()`. El muestreo es periódico. | Inyectar lectura/reset/inicio del contador y lectura del pulsador con contexto. Preservar wrap, dirección, pasos y umbrales. |
| Botón de usuario | `Core/Inc/types/button_state.h:14` incluye STM32 HAL; el estado guarda puerto/pin. `Core/Src/user_button.c` lee GPIO directamente y usa macros del proyecto. | Estado propio y entrada de nivel lógico o callback. Mantener semántica de pulsación corta/larga y período de 10 ms. |
| SSD1306 | `Core/Inc/ssd1306.h:25` incluye gestor I²C, además de fuentes y `main.h`. `Core/Src/ssd1306.c` mantiene framebuffer, colas y gestor en variables globales/estáticas; usa HAL y llamadas `I2C_Manager_*`. | Handle por instancia, interfaz de transporte asíncrono y callbacks con contexto. Separar rasterizador/fuentes del transporte. |
| MPU9250 y AK8963 | `Core/Inc/mpu9250.h` incluye STM32 HAL y gestor I²C. En `Core/Src/mpu9250.c:736` se registra directamente en ese gestor; `:1443` y `:1465` invocan DMA HAL. El reloj débil `MPU9250_GetTimeUs` usa HAL por defecto. | Inyectar bus, reloj y notificación de finalización/error. Separar adquisición, calibración y estimación de actitud sin perder magnetómetro. |
| Gestor I²C | `Core/Src/i2c_manager.c:97` consulta estado HAL; `:107` obtiene tick; `:272` sondea dirección de forma síncrona. | Árbitro independiente con reloj/estado/operaciones inyectados, o mantenerlo en el adaptador. Los drivers no deben conocer su tipo concreto. |
| TCRT5000 y emisores IR | `Core/Src/tcrt5000.c:3` incluye `main.h`; `:25`, `:32`, `:53`, `:60` escriben pines de la placa. Tablas fijan orden, polaridad y geometría. | Entradas ADC y configuración por instancia; callback de emisores. Mantener correspondencia de canales y polaridad eléctrica en el adaptador. |
| Motores | `Core/Inc/motor_control.h` expone HAL; `Core/Src/motor_control.c:99` inicia PWM y `:130` escribe compare del timer. | Salida PWM/dirección/freno abstracta con contexto. Preservar límites, mínimos, signos y apagado de ambos canales. |
| PID | `Core/Inc/pid_controller.h:12` incluye STM32 HAL. Cálculo y estado son propios, pero Init/Update devuelven `HAL_StatusTypeDef`. | Resultado propio de la biblioteca; conservar ecuaciones y estado. |
| Balance | `Core/Inc/balance_control.h` contiene dependencia de PID y HAL; `Core/Src/balance_control.c` inicializa y ejecuta PID directamente. | Para el requisito estricto de independencia entre librerías, inyectar servicio de control y contexto, sin alojar un PID concreto dentro de balance. |
| Movimiento | `Core/Inc/motion_control.h` depende de balance, motores y MPU. La implementación consulta frescura con `HAL_GetTick` y usa sus handles concretos. | Entradas de orientación/frescura y servicios de control/salida abstractos. Mantener guardas, enclavamiento y vencimiento de órdenes. |
| Estimación de movimiento | `Core/Inc/motion_estimator.h` depende de MPU y HAL; `Core/Src/motion_estimator.c:199` obtiene snapshot desde el driver. | Recibir muestra neutral, secuencia y tiempo desde la aplicación. |
| Seguimiento de línea | `Core/Inc/line_follow_control.h` depende de movimiento, estimador, PID, TCRT y HAL. La implementación consulta sensores/emisores y envía órdenes a movimiento. | Datos de línea y servicios inyectados. El coordinador conecta las librerías. |
| Núcleo de menús | `Core/Src/menusystem.c:15-19` incluye globals, WiFi, preferencias y favoritos. `:44` lee perfil y `:58/:73` reporta por WiFi. El límite de elementos visibles consulta favoritos concretos. | Políticas de disponibilidad/layout y callbacks de reporte con contexto. Mantener orden de reporte de pantalla antes de selección y reintentos pendientes. |
| Favoritos y definición de menús | `Core/Src/menu_favorites.c` y `menu_definition.c` incluyen preferencias, wrappers, OLED y módulos de aplicación. | Separar modelo reusable de favoritos/menús del catálogo y acciones AutoMicro. |
| Pantallas, notificaciones y OLED utils | `Core/Src/screenWrappers.c`, `notificationWrappers.c`, `oled_utils.c` y `oled_canvas.c` mezclan render, globals, dispositivos, navegación y transporte. | Renderers reciben modelos de vista y operaciones de dibujo; controladores AutoMicro producen datos y ejecutan acciones. |
| OSK | `Core/Inc/osk.h` usa tipos estándar, `OSK_DrawHandle` y callbacks de completar/cancelar con contexto. `Core/Src/osk.c` no incluye drivers de dispositivos. | Base favorable. Dibujo todavía carece de contexto por operación y asume geometría/páginas concretas; no certificar portabilidad ilimitada. `osk_integration.c` debe permanecer como adaptador. |
| Bloqueo, permisos y router de UI | `screen_lock.c`, `permissions_stub.c`, `ui_event_router.c` dependen de estado/UI de aplicación; permisos consulta HAL tick y UNER. | Motor de política/estado separado de reloj, persistencia, presentación y autoridad remota. |
| Preferencias y configuración AP | `app_preferences.c` usa borrado/programación Flash HAL y conoce balance; `app_ap_config.c` conoce globals, placa y bridge. | Modelo/configuración separado de almacenamiento y aplicación de cambios. Persistencia necesita estrategia explícita de ejecución. |
| UNER core/handle | Headers ofrecen callbacks/contexto y `uner_transport.h` ofrece `poll_rx/try_send`. Sus `.c` aún incluyen `stm32f4xx_hal.h`, aunque la búsqueda focalizada no encontró llamadas HAL en ellos. | Eliminar dependencia de compilación innecesaria; conservar relación interna entre capas del mismo protocolo. Adaptadores UART/USB quedan fuera del núcleo. |
| WiFi bridge y backend AT | `wifi_bridge_types.h` expone HAL; `.c` usa UART/DMA y tick. `wifi_bridge_uner.c` incluye `uner_app.h`. | Separar lógica del bridge de puerto UART, tiempo y aplicación UNER mediante interfaces. |
| USB CDC/MSC y modo USB | `USB_DEVICE/` y transportes usan stack ST/periféricos de la placa. `USB_DEVICE/Target/usbd_conf.c:647` contiene HAL delay. | Son adaptadores de plataforma; abstraer el contrato que consumen las librerías, sin exigir portabilidad del stack ST. |
| Composición y utilidades | `app_tasks.c`, `app_globals.c`, `eventManagers.c` conectan módulos. `utils/macros_utils.h` declara flags globales del proyecto. Fuentes, bitmap y tipos simples deben separarse de utilidades que importan esos globals. | La composición puede conocer todas las librerías; ellas no deben depender de la composición. |

## Bloqueos concretos y rutas activas

- OLED: `ssd1306_Init`, llamado desde `app_tasks.c:178`, sondea I²C, espera 100 ms por defecto y envía comandos síncronos (`ssd1306.c:593-679`). El refresco normal usa cola y DMA con los defaults `USE_DMA=1`, `USE_I2C_MANAGER=1`, `CONTUPDATE=0`. Los bucles de espera de `:1506-1507` y `:1568-1597` pertenecen a variantes alternativas; no se atribuyen al refresco normal con esos defaults.
- MPU: `APP_InitMPU` llama a `MPU9250_Configure` (`app_tasks.c:1202`). Configuración y AK8963 usan lecturas/escrituras síncronas y delays de 100 ms y cinco de 10 ms (`mpu9250.c:759-905`). La adquisición posterior usa DMA y callbacks. DMA de adquisición no elimina el bloqueo de configuración.
- Persistencia: `app_preferences.c:1334/:1343` borra/programa Flash de forma síncrona. Encapsularlo en un callback cambia la dependencia, pero no elimina la espera ni garantiza que el procesador siga atendiendo control durante un borrado.
- Gestor: `I2C_Manager_IsAddressReady` usa sondeo síncrono (`i2c_manager.c:269-272`), incluido al iniciar MPU.
- No todo `while` es una espera bloqueante: rasterización, parsing y recorrido de tablas tienen trabajo finito. Esta lectura no establece presupuestos temporales ni duración máxima de callbacks/colas.

## Arquitectura y orden propuestos

1. Definir resultados propios, `void *context`, relojes y operaciones asíncronas con aceptación/BUSY y notificaciones de finalización/error. El buffer debe vivir hasta completar o cancelar la operación.
2. Separar primero botones, encoder, motores y PID: sus interfaces son más pequeñas. Mantener wrappers de la aplicación para conservar sus consumidores actuales.
3. Extraer transporte I²C y mover OLED/MPU a handles propios. Convertir configuración y esperas de arranque en estados con deadlines; conservar secuencia de registros, prioridad del MPU y política de bus.
4. Separar cálculo y adquisición de IR/MPU. Conectar PID, balance, movimiento, estimador y línea mediante datos e interfaces desde la aplicación, no mediante inclusión mutua de drivers.
5. Desacoplar núcleo de menús, favoritos, permisos y bloqueo; después extraer modelos de vista/dibujo de las pantallas. Mantener teclado, notificaciones, navegación y reportes remotos.
6. Separar persistencia, bridge y plataforma USB/UART. Conservar código CubeMX, pinout y adaptadores STM32 dentro de FirmwareF4.

Los callbacks deben definir contexto de ejecución, propiedad de buffers, cancelación, error y reentrada. Un puntero a función que llama a una operación bloqueante sigue siendo bloqueante. Exportar globals de otro módulo tampoco convierte una biblioteca en independiente.

## Contratos y entrega

La ficha vigente AMI-20261003-01 fija frescura MPU, comandos de marcha/TEST y guardas: una refactorización deberá conservarlos junto con IDs/payloads, unidades, endian, ACK/NACK, autenticación, screen codes, orden de eventos y política de transporte. Cambiar tiempos o secuencias compartidas requiere registrar una ficha según la skill de integridad.

Esta entrega es una revisión con propuesta, no una refactorización implementada ni una certificación de compatibilidad entre repositorios. No crea ficha AMI ni cambia CHANGELOG porque no cambió contrato alguno. F4 sigue pendiente de desacoplamiento; ESP, Web y Qt no se modificaron y no se acredita aquí su interoperabilidad. No se creó commit ni se publicó al remoto.


## Evolución posterior — 2026-10-04

Esta auditoría conserva la línea de base anterior al refactor. La implementación
3.0.0 y los límites actuales se describen en el [documento actualizado](../docs/F4_MODULARIZATION_IMPLEMENTATION_PLAN_20261003.md).
FirmwareF4 se modificó y compiló Debug desde cero con salida 0, incluido empaquetado.
No se ejecutaron tests ni pruebas físicas, y no se atribuye compatibilidad probada
con ESP/Web/Qt a esta compilación. Los formatos/comandos compartidos no se rediseñaron.
