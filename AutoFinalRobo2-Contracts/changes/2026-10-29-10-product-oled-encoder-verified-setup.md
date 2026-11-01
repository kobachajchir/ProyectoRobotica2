# AFR2-20261029-10 — Configuración OLED/encoder confirmada aplicada al producto

> **Compilación posterior autorizada, 29/10/2026:** `make -B firmware`
> terminó con exit0, sin errores ni advertencias. Se generaron ELF/HEX/mapa
> nuevos del producto. Flash73484bytes (text72390+data1094), SRAM estática
> 5931bytes (data1094+bss4837); quedan2261bytes sin asignación estática, sin
> certificar pila/ISR. SHA-256 de `AutoFinalRobo2-Atmega/build/afr2.hex`:
> `41bffabe432b2a2e91bbf4b18ed615e99ee66bd2d33efa41925ffb704c131369`.
> Herramientas AVR GCC7.3.0-atmel3.6.1-arduino7 y GNU Make de Atmel Studio7.
> No se compiló diagnóstico, ESP o Web; sin tests/validadores/COM/flasheos ni
> pruebas físicas del agente. Las menciones de fuente sin compilar inferiores
> corresponden a la entrega anterior a este pedido de compilación.

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

## Evidencia y pedido

El usuario confirma que encoder, OLED y orientaciones del diagnóstico09
funcionan y pide trasladar esos cambios al firmware del proyecto. Evidencia
física reportada por el usuario sobre ese servicio y montaje; no es una prueba
del agente ni certificación del firmware completo, sensores, motores o memoria.

## Adaptaciones por archivo

Rutas Atmega relativas a AutoFinalRobo2-Atmega. Código original AFR2, autorías
y comentarios anteriores conservados. Se mantiene C/avr-libc, sin Arduino/DMA,
OLED único cliente de I2C Manager y navegación por SW largo/corto existente.

| Archivo | Adaptación |
| --- | --- |
| src/platform/afr2_twi.c | Default AFR2_TWI_INTERNAL_PULLUPS=1 como diagnóstico09 confirmado; conserva release/recuperación con pulls débiles, reloj TWI restaurado y100kHz. Macro0 permite otra configuración explícita. |
| include/afr2_twi.h | Documenta default promovido y override. |
| src/display/afr2_oled.c | Default espera500ms como diagnóstico. Conserva0x2E antes de encendido, modo por páginas y manejo de errores. No cambia primitivas, glifos ni layout. |
| src/services/afr2_product.c | Defaults encoder_invert=1, rotation=1 (180grados), encoder_steps=4 ya existente. Dirección0x3C y contraste0xCF conservados. |
| include/afr2_product.h | Versión del journal EEPROM de1a2, independiente de versión wire/firmware. |
| src/platform/afr2_preferences.c | Acepta slotsv1/v2 con CRC/marca/validez anteriores. Migra solo campos de UI en v1: encoder_invert=1, rotation=1, encoder_steps=4. Conserva restantes ajustes de Hall/motor/geometría/IR/PID y secuencia de journal. No escribe al arrancar. Guardar explícito crea v2, que conserva ajustes posteriores del usuario. |
| src/main.c | Reenvío del framebuffer autoritativo cada1s como diagnóstico09. No fuerza redibujo/revisión, no ejecuta acciones ni inventa datos. Fuente4bytes de reloj adicionales, presupuesto total pendiente. |
| Makefile | Aclara que defaults del producto coinciden con el diagnóstico; servicio sigue separado. |
| README.md, INFORME_ADAPTACION.md, docs/COMPOSICION.md | Estado fuente y alcance promovido. |

`AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.sources.json`: fingerprint
de afr2_oled.c actualizado después de revisar diff: solo init/transporte/espera
cambiaron desde la revisión anterior, no algoritmos de dibujo. No se altera
renderer TypeScript ni recursos porque las coordenadas/glifos/framebuffer son
idénticos. La rotación física ya usa el ajuste de controlador existente, no
otro ScreenCode ni una pantalla nueva.

Contracts: esta ficha y guía contractual nueva para journalv2, README/CHANGELOG,
guía de diagnóstico y estado de implementación registran estados reales.

## Qué se conserva del producto

El main del producto ya sirve OLED continuamente sin escáner y USART1 ya es
no bloqueante por colas/IRQ. No se agrega el barrido112direcciones, la cola
debug USART0, el contador de prueba ni sus textos. No se reemplazan menús ni
se elimina la semántica SW largo abre menú/corto confirma y Volver explícito.
Encoder físico sigue D2/D3/SW D42, OLED D20/D21. Ajustes de orientación y
sentido continúan editables y se conservan al guardar como v2.

## Estado real

ATmega: configuración trasladada en fuente y revisada por lectura. No se
ejecutaron tests, compilación, validadores, COM o flasheo. HEX del producto no
regenerado por el agente; requiere recompilar antes de cargar. Funcionamiento
reportado del diagnóstico09 no acredita el producto integrado todavía.

Web: solo fingerprint de fuente revisada; sin build, validación, cambios de
dibujo ni promoción ESP. ESP/Loader/CAD sin cambios por10; wire conservado.
No se autoriza movimiento por haber confirmado la interfaz.

Tras una futura carga, las calibracionesv1 se leen sin borrarlas; la migración
vive en RAM hasta Guardar EEPROM. Firmware viejo v1 no puede leer registrosv2;
no volver a cargarlo suponiendo compatibilidad de preferencias. No se cambia
bootloader/fuses ni se borra EEPROM por crear esta adaptación.
