# Carpeta include

Estado vigente AFR2-20261023-15: backend/composición integrados en fuente; el texto anterior de migración se conserva debajo como antecedente. Sin compilar/probar.

[Maestro](../../INFORME_MAESTRO.md) · [Informe del repositorio](../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

## Utilidad y reformas específicas

### [Nuevo driver HC-SR04](../docs/modules/hcsr04.md)

No se localizó un driver reusable directo en la fuente F4. El pinout recibido asigna TRIG D30 y ECHO D31.

Reformas: Implementar idle, trigger, wait-rise, wait-fall, publish y timeout con reloj fino; no delay/pulseIn. D31 requiere muestreo periódico de resolución suficiente o recableado a captura con ficha. Publicar distancia/validez/edad/error; timeout no significa camino libre. Si se usa servo D13, medir solo después de estabilización y marcar orientación. Cobertura frontal no protege lateral/retroceso.

## Archivos vecinos

- [afr2_encoder_ui.h](<afr2_encoder_ui.h>)
- [afr2_i2c_manager.h](<afr2_i2c_manager.h>)
- [afr2_oled.h](<afr2_oled.h>)
- [afr2_screens.h](<afr2_screens.h>)
- [board_pinmap.h](<board_pinmap.h>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../AutoFinalRobo2-Contracts/provenance/files.json).


AFR2-20261023-15: TWI AVR por IRQ y composición OLED/encoder/USART1 implementados en fuente, sin DMA ni Arduino. Ver docs/COMPOSICION.md y Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Sin builds/tests/validadores/flasheos; hardware y recursos finales pendientes.
