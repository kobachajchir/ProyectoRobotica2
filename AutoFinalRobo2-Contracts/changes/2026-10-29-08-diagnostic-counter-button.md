# AFR2-20261029-08 — Contador y pulsador en OLED del firmware de prueba

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Pedido: mostrar contador que sube/baja con el encoder y debajo PRESIONADO o
SUELTO. Aclaración explícita posterior: **solo en el sketch de prueba**. Se
modifica la composición C/avr-libc de diagnóstico de07, no el sketch externo
Adafruit, las pantallas del auto ni su copia Web. La restricción explícita de
alcance prevalece sobre la regla general de crear también contraparte Web.

## Adaptaciones por archivo

- `AutoFinalRobo2-Atmega/service/i2c_uart0_diagnostic.c`: sustituye las barras
  periódicas por renderer puro privado del servicio. CONTADOR centrado en
  y4; número firmado centrado en y23; estado en y49. Fuente7x10 existente,
  fondo limpiado para borrar dígitos/texto anteriores; orientación180 conservada.
  Valor int32 inicia0, A suma1 y B resta1 por paso de4transiciones. Saturación
  evita overflow firmado; redibujo solo al cambiar valor o estado estable SW.
  Snapshot de entrada separado de dibujo. Botón no guarda/resetea ni ejecuta
  navegación. Reutiliza antirrebote existente20ms, captura IRQ, depuración solo
  ante actividad y barrido10s; la pausa de envío OLED durante barrido se conserva.
- `AutoFinalRobo2-Atmega/tools/flash-i2c-encoder-diagnostic.ps1`: banner08 y
  descripción de contador/botón. Mismo objetivo y nombre HEX, recompila antes
  de cargar cuando lo ejecute el usuario.
- `AutoFinalRobo2-Atmega/service/README.md` y
  `AutoFinalRobo2-Contracts/hardware/OLED_I2C_DIAGNOSTICO.md`: alcance vigente.
- Esta ficha, README y CHANGELOG Contracts: registro de procedencia/estado.

Código añadido original AFR2, autorías previas conservadas. No cambia recursos
de fuente, ScreenCodes, catálogo, navegación de producto ni wire ESP/Web.
El contador del servicio no tiene ScreenCode de producto ni captura remota.

## Estado real

ATmega: fuente y revisión manual por lectura. Sin compilación, tests,
validadores, COM, flasheo o prueba física. No se generó nuevo HEX. Se añade
int32 de contador, flag y buffer local12bytes para formato; SRAM/pila/flash
totales pendientes de build autorizado. ESP/Web/Loader/CAD sin cambios por08.
La prueba funcional reportada corresponde al sketch externo Arduino/Adafruit,
no confirma07 ni08. La modificación no asegura que se hayan resuelto los NACK.
