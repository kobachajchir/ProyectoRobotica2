> **Diagnóstico17 compilado, sin cargar:** usuario confirma puente S1–D13 instalado además de GND/5V/S1 del servo. BuildOnly exit0, sin errores/advertencias, sin COM/avrdude. Flash13424bytes; SRAM3169bytes, margen5023bytes, sin certificación de pila. HEX afr2-i2c-diagnostic.hex SHA256 03cf3a3760f3b6bc5b9b8a7ce128a56388a5bd51b69209a2ea39e895283ca573. Causa del servo inmóvil aún sin medición/prueba física. Producto16 no cargado; placa conserva15. Las notas de compilación/puente pendientes inferiores son históricas.

# AFR2-20261029-17 — Prueba aislada encoder → servo S1/D13

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Pedido: investigar servo inmóvil, usuario conecta GND/+5V/S1 de OSOYOO V2.0;
adaptar el firmware de prueba del encoder y compilarlo. La instrucción previa
«No lo subas aun» sigue vigente: no cargar ni ejecutar pruebas físicas.

## Documentación y diagnóstico por lectura

[Manual OSOYOO Mecanum](https://osoyoo.com/manual/2021006600.pdf#page=19),
página19, paso17: diagrama distingue servo GND/5V/S1 y puente adicional
S1 de Model Y V2.0 a D13 del Mega/shield UART Wi-Fi. Conectar únicamente los
tres cables del servo al distribuidor no acredita esa conexión adicional.
Usuario la está revisando; no se afirma que falte en su montaje. Es una causa
posible concreta de que el PWM no llegue. No se presupone continuidad interna
automática entre placas independientes. El modelo citado como «I» se interpreta
según el Model Y V2.0 del proyecto; identificación física no certificada.

Producto usa D13/PB7 por Timer5 IRQ y arranca con servo deshabilitado; ver
HC-SR04/radar por sí solo no inicia movimiento. Acción angular/centrar/iniciar
barrido habilita PWM. Diagnóstico anterior deshabilitaba servo intencionalmente:
su encoder solo actualizaba contador. No se ha medido señal ni alimentación.

La prueba ahora usa PWM hardware independiente del driver Timer5 de producto.
[Datasheet Microchip ATmega2560](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf):
PB7 permite OC1C y comparte salida con OC0A; se deshabilita OC0A para evitar
modulación. Timer1 Fast PWM14, ICR1=39999, prescaler8 a16MHz: periodo nominal
20ms; OCR1C=2*pulso_us. OC1A/B desconectadas, sin PWM de ruedas D11/D12.
No cambia el firmware del auto ni se prueba su driver Timer5 por esta vía.

## Comportamiento y archivos

- Atmega/service/i2c_uart0_diagnostic.c: arranca sin pulsos, referencia90grados.
  Primer giro habilita90+/-5; cada paso suma/resta5, limitado0..180. Pulsos
  nominales1000..2000us. OLED conserva orientación180 y botón PRESIONADO/SUELTO;
  agrega ángulo ordenado, PWM y contador. No mide posición mecánica.
- USART0 D0/D1 a115200: reportes solo cambios, hasta10/s. C centra90, S deja
  PWM apagado/D13 bajo, R adelanta escaneo. Escaneo10s y único I2C Manager se
  conservan. Reportar pulso programado no demuestra señal física ni movimiento.
- Atmega/tools/flash-i2c-encoder-diagnostic.ps1: BuildOnly compila y retorna
  antes de avrdude; no abre COM. Sin ese switch conserva carga/verificación
  explícita existente, ahora identifica la imagen que puede mover servo.
- Informes/README y guía de servicio registran nueva variante. Es pantalla de
  diagnóstico aislada, no UI del producto ni pantalla de catálogo; pedido se
  limita al software de prueba, sin nueva contraparte Web o wire.

No enlaza producto, sensores, motor, EEPROM ni puente ESP; Board_Init conserva
latches de ruedas apagados. Actúa solamente sobre la señal de servo por entrada
del usuario; SW mantiene su función diagnóstica, sin iniciar acciones del auto.
No se modifica reference/ ni se accede a D:/AutoProyecto. Autorías preservadas.

Estado inicial: fuente adaptada, compilación del diagnóstico autorizada pendiente.
Sin tests/validadores, COM, flasheo ni ensayo físico. Producto16 permanece
compilado/no cargado; placa conserva15. Firmware de prueba tiene HEX separado
afr2-i2c-diagnostic.hex, nunca renombrado como afr2.hex. ESP/Web/Loader sin cambios.
