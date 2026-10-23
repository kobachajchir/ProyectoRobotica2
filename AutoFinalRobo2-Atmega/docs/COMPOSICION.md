# Integración prevista

[Informe](../INFORME_ADAPTACION.md). Declarar handles estáticos: Afr2I2cManager, Afr2Oled, Afr2EncoderUi, Afr2Ui. Antes de habilitar interrupciones, configurar salidas de potencia inactivas y reloj/GPIO reales. No hay firmware main integrado en esta entrega.

1. Construir backend TWI y llamar Afr2I2c_Init. Conservar ctx/funciones durante toda su vida.
2. Afr2Oled_Init con 0x3C solo si se confirma módulo SSD1306 128x64/dirección. No reinicializar handle mientras Manager lo usa.
3. Inicializar encoder con CLK/DT/SW reales; botón ya pulsado al arrancar no produce gesto hasta soltar. Configurar polaridad y 2/4 transiciones por detent según el encoder.
4. En main: atender UART/supervisión, Afr2I2c_Tick(now), Afr2Oled_Task(now), adquisición corta de encoder cada 1ms y Afr2Ui_Apply. ISR solo publica datos.
5. Crear snapshot coherente de sensores/estado; render si redraw o cambio de snapshot al ritmo UI. Afr2Ui_Render no consulta hardware y no transmite. Afr2Oled_Task envía dirty pages independientemente.

Para snapshot compartido con IRQ usar sección atómica breve, no copiar framebuffer deshabilitando interrupciones. Datos desconocidos conservan valid=false. Largo de SW abre menú desde cualquier pantalla, sin regresar automáticamente ni parar/reanudar motores por navegación. La política de seguridad motriz es otro módulo.

La tipografía es ASCII 7x10; glifos especiales/íconos/OSK/renderers completos quedan en reference/f4 para port incremental. INVERT hace XOR explícito en la API nueva; es una decisión local y no una afirmación de paridad con la inconsistencia histórica F4.
