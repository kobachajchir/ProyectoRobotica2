# Carpeta docs/modules

Estado19: [producto integrado en fuente](../PRODUCTO_ATMEGA.md), sin compilar/probar.
Backend TWI y módulos nuevos ya escritos; informes iniciales siguientes son históricos.


[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

Revisar dependencia de HAL/DMA, memoria, puertos y contexto de callbacks. La biblioteca nueva en src/ separa renderer, wrapper, Manager y backend; el backend AVR queda pendiente. [Guía OLED](i2c-oled.md) y [navegación](../../../AutoFinalRobo2-Contracts/documentation/NAVIGATION.md).

## Archivos vecinos

- [encoder-ui.json](<encoder-ui.json>)
- [encoder-ui.md](<encoder-ui.md>)
- [hcsr04.json](<hcsr04.json>)
- [hcsr04.md](<hcsr04.md>)
- [i2c-oled.json](<i2c-oled.json>)
- [i2c-oled.md](<i2c-oled.md>)
- [line-ir.json](<line-ir.json>)
- [line-ir.md](<line-ir.md>)
- [motion.json](<motion.json>)
- [motion.md](<motion.md>)
- [protocol.json](<protocol.json>)
- [protocol.md](<protocol.md>)
- [screens.json](<screens.json>)
- [screens.md](<screens.md>)
- [wifi-composition.json](<wifi-composition.json>)
- [wifi-composition.md](<wifi-composition.md>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../AutoFinalRobo2-Contracts/provenance/files.json).
