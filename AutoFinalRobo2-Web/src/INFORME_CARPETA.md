> **AFR2-20261029-26:** fuentes de batería0x55/EX01v9 y navegación/pill/confirmaciones vinculadas según Contracts/contracts/BATTERY_MEASUREMENT_9.md. Screens0x030039/0x03003A y acción69. Revisión solo por lectura/edición, sin build/generador/pruebas/carga; adaptación por archivo en Contracts/provenance/battery-system-20261029-26.json.

# Carpeta src

[Maestro](../../INFORME_MAESTRO.md) · [Informe del repositorio](../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

Portar datos/componentes al nuevo contrato con unknown/capabilities, cuatro ruedas y sensores reales. Referencias balance/MPU/GLB anterior quedan fuera del producto. [Guías](../INFORME_ADAPTACION.md).

## Archivos vecinos

- [main.jsx](<main.jsx>)
- [style.css](<style.css>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../AutoFinalRobo2-Contracts/provenance/files.json).


## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.
