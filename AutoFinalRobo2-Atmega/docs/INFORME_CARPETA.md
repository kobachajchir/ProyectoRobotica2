# Carpeta docs

AFR2-20261023-19: composición de producto y EX01 ya escritos, sin compilar/probar.
[Guía vigente](PRODUCTO_ATMEGA.md), [detalle por archivo](../../AutoFinalRobo2-Contracts/docs/ATMEGA_IMPLEMENTACION_19.md),
[API/complemento](../../AutoFinalRobo2-Contracts/contracts/ROBO2_PRODUCT_EXTENSION.md).
Las restricciones y estados de las fichas anteriores abajo son históricos.


[Maestro](../../INFORME_MAESTRO.md) · [Informe del repositorio](../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

Revisar dependencia de HAL/DMA, memoria, puertos y contexto de callbacks. La biblioteca nueva en src/ separa renderer, wrapper, Manager y backend; el backend AVR queda pendiente. [Guía OLED](modules/i2c-oled.md) y [navegación](../../AutoFinalRobo2-Contracts/documentation/NAVIGATION.md).

## Archivos vecinos

- [API_NUEVA.md](<API_NUEVA.md>)
- [COMPOSICION.md](<COMPOSICION.md>)

## Subcarpetas

- [modules](modules/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../AutoFinalRobo2-Contracts/provenance/files.json).
