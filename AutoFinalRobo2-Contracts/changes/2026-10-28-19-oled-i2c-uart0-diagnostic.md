# AFR2-20261028-19 — Diagnóstico I2C OLED por D0/D1

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 28/10/2026.

Pedido: preparar búsqueda de dirección en D20/D21 con depuración USART0
D0/D1. El usuario aclara que el pinout une esos pines con SDA/SCL del shield
y corrige su hipótesis de dirección a 0x78 (0x3C de 7 bits si es byte de escritura).
Continúa la revisión18; no confirma causa del síntoma.

[Guía de uso y límites](../hardware/OLED_I2C_DIAGNOSTICO.md).

## Procedencia y adaptaciones por archivo

Rutas siguientes relativas a AutoFinalRobo2-Atmega. Código nuevo original AFR2;
comentarios de autoría anteriores conservados. Sin copiar reference/ ni usar
Arduino/HAL/DMA. Sin cambios de pantallas, ScreenCodes, recursos o navegación.

| Archivo | Adaptación |
| --- | --- |
| service/i2c_uart0_diagnostic.c | Servicio separado: búsqueda08–77 por Manager, USART0 115200 8N1, ACK/NACK/timeout/error, niveles/estado/recovery, resumen y R para repetir; watchdog y esperas acotadas. No envía comandos OLED ni modifica EEPROM. |
| include/afr2_i2c_manager.h | API Probe, contrato NULL/0 y NACK añadido al final del enum, sin cambiar valores anteriores. |
| src/services/afr2_i2c_manager.c | Propiedad/deadline/callback comunes; Submit sigue rechazando escrituras vacías. NACK de probe completado no dispara recuperación. |
| include/afr2_twi.h | Snapshot atómico de estado retenido, niveles y recuperación. |
| src/platform/afr2_twi.c | Transferencias sin datos y NACK específico solo para probe; escrituras del producto conservan ERROR/recovery. Retiene estado ISR. |
| Makefile | Objetivo i2c-diagnostic-firmware, HEX/ELF/map separados; recompila fuentes compartidas sin reciclar objetos de otra composición. |
| service/README.md, README.md, INFORME_ADAPTACION.md | Estado fuente y enlace al servicio. |

Contracts: esta ficha y la guía son originales de esta revisión; README y
CHANGELOG enlazan el cambio. No cambia wire de producto ni contrato Web/ESP.

## Estado real

ATmega: servicio preparado en fuente y revisión manual por lectura. Sin tests,
compilaciones, validadores, ejecución, COM, flasheo ni prueba I2C/OLED. Flash,
SRAM y pila del servicio no medidos. Los cambios compartidos requieren futura
recompilación autorizada del producto; su HEX existente no se regeneró.

ESP/Web/Loader/CAD: sin cambios por esta ficha, sin promoción ESP. No se altera
dirección del producto ni se integra escáner permanente en el auto. Cambios
previos del Loader en el árbol de trabajo conservados fuera de este alcance.
