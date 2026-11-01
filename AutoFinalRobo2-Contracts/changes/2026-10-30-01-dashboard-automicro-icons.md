# AFR2-20261030-01 — Dashboard e iconos AutoMicro adaptados

Pedido: recuperar dashboard AutoMicro con Wi-Fi, AP, encoder, batería y niveles; accesos corto Menú y largo Rápidas.

## Implementación

- Misma identidad010001 y navegación29: corto010002; largo >=800ms al soltar010003. No cambia el debounce ni la excepción Sí/No.
- Diseño inspirado en `Atmega/reference/f4/Core/Src/oled_utils.c:OledUtils_RenderDashboard`: conectividad arriba, estado en el centro y accesos abajo. Se reemplazan backend F4/USB/RF y botón físico independiente por modo/motores reales, batería y un único encoder. No se inventan periféricos.
- Wi-Fi, AP y encoder ya portados de `fonts.c`, autor kobac. Wi-Fi desconectado copiado literalmente del mismo origen; batería y encoder largo son recursos AFR2 originales, no había batería en esa fuente. IDs0..15 conservados, nuevos16..23.
- Recursos PROGMEM, sin segundo framebuffer. Se añaden ocho iconos (332bytes de bitmap y tabla de ocho entradas flash) y campos declarados de batería uint16+uint8 en snapshot; margen real de SRAM/flash/pila pendiente de build, sin extrapolar1107bytes del build anterior.
- Encabezado: icono STA o AP con prioridad STA si ambos activos; SSID e IP con ventanas70px y scroll según reloj. Red inválida muestra Red: --, distinta de Sin Wi-Fi confirmado. No se presenta RSSI inexistente.
- Batería toma `Afr2Battery_Status == VALID` y milivoltios de la adquisición existente A5. Vacía0V;1barra(0,6V),2[6,7),3[7,8),4>=8V, mismas bandas que pillWeb. Son bandas visuales de voltaje, sin inferir carga, química ni alarma. Inválida/off/caducada: ? y --V. No enciende automáticamente medición, se inicia en Sistema → Batería.
- Centro: modo real y fallo o estado de motores; desconocido explícito en biblioteca sin snapshot. Footer Menu con icono corto, Rapidas con icono encoder y barra inferior de duración.
- Renderer puro C portado literalmente a `Web/src/features/oled/firmwareRenderer.ts`, recursos copiados mediante `--copy-only` sin validadores. LiveOLED conserva captura AVR1024bytes. Texto y recursos estáticos conservan transferencia diferencial/cooperativa existente. Sin nuevo wire ni cambios ESP, EEPROM o versiones.

## Estado por módulo

| Módulo | Estado |
|---|---|
| ATmega | Fuentes implementadas; sin compilación/carga/ensayo |
| ESP | Sin cambios; transporte de framebuffer existente; paquete sin promover |
| Web | Renderer raster y recursos vinculados; sin compilación/navegador/despliegue |

Solo lectura y edición de fuentes/documentos, y copia de recursos `sync-oled-library.mjs --copy-only`; no tests, validadores, compilaciones, puertos, flasheos ni pruebas físicas. Revisión visual en dispositivo y equivalencia de raster pendientes de autorización y ejecución. Procedencia por archivo en `provenance/dashboard-automicro-20261030-01.json`.
