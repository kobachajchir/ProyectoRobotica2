# AFR2-20261030-10 — Fuentes OLED revisadas para predev Web

30/10/2026. Usuario aporta fallo de `npm run dev`: predev aborta en sync-oled-library.mjs85 por hash de afr2_screens.c distinto del registro revisado. No es un error de Node22 ni de comunicación con el auto.

## Causa y cambio

El registro firmwareRenderer.sources.json conservaba seis hashes anteriores a las fuentes actuales. Las contrapartes TypeScript ya contienen las adaptaciones07/09. Se revisaron por lectura antes de actualizar esos seis hashes SHA256 de texto UTF8 normalizado a LF. No se desactiva ni omite el guard, no se usa --copy-only ni se alteran scripts npm. Cambios futuros de AVR siguen exigiendo revisión.

## Correspondencias revisadas

| AVR | Contraparte Web existente / revisión |
| --- | --- |
| src/ui/afr2_screens.c | firmwareRenderer.ts y firmwareHints.ts: selección/recuento dinámico Wi-Fi, candado, red/datos/modo, hints corto/largo, dashboard y scroll, wizard Hall y Sí/No, rápidas7, encoder, capacidades6 y potencia; control de navegación permanece en AVR |
| src/ui/afr2_network_screen.c | network/networkNotice: conexión2páginas, textos/posiciones/scroll, datos STA/AP, avisos y hints de cerrar/volver |
| src/ui/afr2_icons.c | menuIcon/icon, reformResources.ts y copia de recursos preparada por generador: icono refresh al escanear, candado8x12 y giro16x16, mismos bytes y escalado |
| include/afr2_screens.h | FirmwareUiState/FirmwareSnapshot: datos preparados de seguridad por fila, red, estado de prueba, potencia y selección; estado de operaciones/navegación local no se simula en Web |
| src/ui/afr2_menu_catalog.c | firmwareNode/firmwareMenuItem y reformCatalogue.ts/wifiCatalogue.ts: tablas reform/wifiReform, identidades, resolución y filas; sync-oled-library.mjs ya copia ambas tablas y nodos actuales |
| src/ui/afr2_sensor_screens.c | firmwareSensorRenderer.ts: proyección entera, radar/rastro, formato numérico y hints vía firmwareHints.ts |

La revisión de fuente habilita el guard del snapshot actual; no acredita igualdad diferencial de framebuffer, ejecución de dev, render visual ni funcionamiento físico. El visor conectado continúa dependiendo de captura real, sin sustituirla por la copia local. No se añaden ScreenCodes, campos ni comandos.

## Estado real

Web: registro de fuentes revisadas actualizado; TS y generador existentes conservados. ATmega/ESP: sin modificaciones por esta ficha. No se ejecutaron npm, predev, generador, Vite, compilaciones, tests, validadores, navegador, promoción, COM ni carga. Al ejecutar el usuario nuevamente `npm run dev`, su predev regenerará los recursos con el guard activo. Otros errores de ejecución no quedan descartados por esta lectura.

Autorías conservadas. Adaptaciones por archivo en provenance/web-predev-20261030-10.json.
