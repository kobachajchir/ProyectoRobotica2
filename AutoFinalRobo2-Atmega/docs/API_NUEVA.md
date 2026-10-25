> AFR2-20261024-11: API IRQ/FIFO y separación giro/pulsador, fuente sin compilar. Ver [contrato IRQ](../../AutoFinalRobo2-Contracts/hardware/INTERRUPCIONES_ENCODER_HALL.md).

> **AFR2-20261024-08:** IP/AP y avisos con iconos F4, scroll seleccionado y reinicio local por Sistema → Reinicio, compilados. [EX01v5](../../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md) · [Estado real](../../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). APIs nuevas FillRect/TextWindow, iconos y snapshot de red separados del transporte/I2C. Sin prueba de placa ni flasheo; estados inferiores históricos.

# API inicial: propiedad y ciclo de vida

AFR2-20261023-19: composición de producto y EX01 ya escritos, sin compilar/probar.
[Guía vigente](PRODUCTO_ATMEGA.md), [detalle por archivo](../../AutoFinalRobo2-Contracts/docs/ATMEGA_IMPLEMENTACION_19.md),
[API/complemento](../../AutoFinalRobo2-Contracts/contracts/ROBO2_PRODUCT_EXTENSION.md).
Las restricciones y estados de las fichas anteriores abajo son históricos.


Estado vigente AFR2-20261023-15: backend/composición integrados en fuente; el texto anterior de migración se conserva debajo como antecedente. Sin compilar/probar.

ID: atmega:module/include/afr2_oled.h; también afr2_i2c_manager.h, afr2_encoder_ui.h, afr2_screens.h. Núcleo C sin HAL/Arduino; backend TWI y composición todavía pendientes. Fuente no compilada.

| API | Uso/propiedad/contexto |
| --- | --- |
| bool Afr2I2c_Init(manager, hardware) | Copia tabla de funciones; ctx prestado. Inicialización sin transferencia activa |
| Afr2I2cResult Afr2I2c_Submit(manager,address7,data,size,now,timeout,done,client) | Retiene data/client hasta callback. OK acepta transferencia; BUSY no acepta. Deadline ms no cero. Solo main |
| void Afr2I2c_Tick(manager,now) | Poll/timeout; abort antes de callback de error; callback solo main |
| start(ctx,address7,data,size) | Backend no bloqueante; OK significa aceptado, no terminado |
| poll(ctx) | BUSY o resultado tras STOP; ISR solo publica resultado |
| abort(ctx) | Impide nuevo uso del buffer por periférico/IRQ antes de retornar; recuperación diferida acotada |
| void Afr2Oled_Init(oled,manager,address7,now) | Posee framebuffer/TX, presta manager. Agenda espera inicial100ms; módulo SSD1306 128x64 supuesto |
| void Afr2Oled_Task(oled,now) | Init/páginas; error marca redibujo/reinit con reintento1000ms; transferencias con timeout40ms |
| Clear / Pixel / Rect | Solo framebuffer. Coordenadas signed; clipping en Pixel. Rect es contorno; INVERT XOR |
| Char / Text / TextFlash | ASCII 7x10; sin fondo automático. Texto prestado durante llamada; TextFlash lee PROGMEM AVR |
| uint8_t Afr2Font7x10_Row(ch,row) | Acceso flash a fila de glifo; desconocido usa '?' |
| void Afr2EncoderUi_Init(e,clk,dt,pressed,now) | Estado propio, inicializa lectura y supresión de gesto si arranca pulsado |
| uint8_t Afr2Board_EncoderRead(ab) | Main:0vacío,1AB FIFO,2overflow+AB actual; resetear paso parcial en2 |
| int8_t Afr2EncoderUi_Transition(e,ab) | Main: decodifica cada transición de la cola, devuelve paso -1/0/+1 |
| void Afr2EncoderUi_ResetRotation(e,ab) | Resincroniza AB/parcial sin afectar pulsador |
| Afr2UiInput Afr2EncoderUi_Button(e,pressed,now) | Main, objetivo1ms; largo800ms exclusivo y debounce20ms |
| Afr2UiInput Afr2EncoderUi_Poll(e,clk,dt,pressed,now) | Compatibilidad de polling; producto usa Transition/Button separados |
| void Afr2Board_HallInit() / HallSnapshot(counts) | Init antes de sei; captura x1 por IRQ y copia atómica FL/FR/RL/RR; macro0 entrega ceros |
| void Afr2Ui_Init(ui) | Inicio/selección/redraw; no controla hardware |
| void Afr2Ui_Apply(ui,input) | Transición semántica; menu prioritario; atrás solo ítem |
| void Afr2Ui_Render(ui,oled,snapshot) | Snapshot prestado durante llamada; dibuja y limpia redraw. Sin I2C ni motor |

Firmas completas en include/. No reentrante ni threads/IRQ concurrentes sobre handles. Reloj uint32 monotónico modular, intervalos menores que 2^31ms. No reiniciar instancias con transferencia en vuelo. Callback nunca debe guardar punteros a snapshots de stack; capturar/copiar datos si perduran.

Snapshot de sensores: line_valid/line_bits bit0 IR1..bit4 IR5; ultrasonic_valid/ultrasonic_mm. Validez/frescura deben resolverse en adquisición antes de Render. OLED ready/errors provienen del servicio local. No hay estructuras wire compartidas por sizeof.


AFR2-20261023-15: TWI AVR por IRQ y composición OLED/encoder/USART1 implementados en fuente, sin DMA ni Arduino. Ver docs/COMPOSICION.md y Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Sin builds/tests/validadores/flasheos; hardware y recursos finales pendientes.
