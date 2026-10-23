# API inicial: propiedad y ciclo de vida

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
| Afr2UiInput Afr2EncoderUi_Poll(e,clk,dt,pressed,now) | Devuelve copia {rotation,click,menu}; llamar cada1ms; largo800ms y debounce20ms |
| void Afr2Ui_Init(ui) | Inicio/selección/redraw; no controla hardware |
| void Afr2Ui_Apply(ui,input) | Transición semántica; menu prioritario; atrás solo ítem |
| void Afr2Ui_Render(ui,oled,snapshot) | Snapshot prestado durante llamada; dibuja y limpia redraw. Sin I2C ni motor |

Firmas completas en include/. No reentrante ni threads/IRQ concurrentes sobre handles. Reloj uint32 monotónico modular, intervalos menores que 2^31ms. No reiniciar instancias con transferencia en vuelo. Callback nunca debe guardar punteros a snapshots de stack; capturar/copiar datos si perduran.

Snapshot de sensores: line_valid/line_bits bit0 IR1..bit4 IR5; ultrasonic_valid/ultrasonic_mm. Validez/frescura deben resolverse en adquisición antes de Render. OLED ready/errors provienen del servicio local. No hay estructuras wire compartidas por sizeof.
