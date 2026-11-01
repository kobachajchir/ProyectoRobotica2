> **AFR2-20261029-26:** fuentes de batería0x55/EX01v9 y navegación/pill/confirmaciones vinculadas según Contracts/contracts/BATTERY_MEASUREMENT_9.md. Screens0x030039/0x03003A y acción69. Revisión solo por lectura/edición, sin build/generador/pruebas/carga; adaptación por archivo en Contracts/provenance/battery-system-20261029-26.json.

# Carpeta src

[Maestro](../../INFORME_MAESTRO.md) · [Informe del repositorio](../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

Adaptar propietario/catálogo F4 a Mega, flash ESP12S, LittleFS, UART y capacidades reales. No activar MPU/AT ni credenciales históricas. [Bridge/HTTP](../docs/modules/http-ws.md).

## Archivos vecinos

- [main.cpp](<main.cpp>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../AutoFinalRobo2-Contracts/provenance/files.json).


AFR2-20261023-11: main.cpp compone storage.h/.cpp, wifi_service.h/.cpp, mega_link.h/.cpp y broker.h/.cpp. El perfil binario reside en Contracts/contracts/afr2_wire.h. Fuente implementada, sin compilar ni probar; detalles en README e INFORME_ADAPTACION.


## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.
