# Carpeta src/platform

Estado vigente AFR2-20261023-15: backend/composición integrados en fuente; el texto anterior de migración se conserva debajo como antecedente. Sin compilar/probar.

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

Revisar dependencia de HAL/DMA, memoria, puertos y contexto de callbacks. La biblioteca nueva en src/ separa renderer, wrapper, Manager y backend; el backend AVR queda pendiente. [Guía OLED](../../docs/modules/i2c-oled.md) y [navegación](../../../AutoFinalRobo2-Contracts/documentation/NAVIGATION.md).

## Archivos vecinos

- [INFORME_BACKEND_TWI.md](<INFORME_BACKEND_TWI.md>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../AutoFinalRobo2-Contracts/provenance/files.json).


AFR2-20261023-11 agrega afr2_uart1.c, RX/TX por IRQ USART1 para ESP; src/communication_main.c contiene Timer0 y main opcional sin activar potencia. TWI permanece pendiente. Ver docs/COMUNICACION_ESP.md; fuente sin compilar.


AFR2-20261023-15: TWI AVR por IRQ y composición OLED/encoder/USART1 implementados en fuente, sin DMA ni Arduino. Ver docs/COMPOSICION.md y Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Sin builds/tests/validadores/flasheos; hardware y recursos finales pendientes.
