# AFR2-20261027-22 — Loader0.6: Web/AT por USB y selección de memoria

27/10/2026. Pedido humano: ampliar el cargador para futuros usos con firmware
ESP de comandos AT, además de Web y ATmega, elegir filesystem/tamaño y mostrar
la reserva de código/datos con reglas distintas. Aclaración expresa: **firmware
AT o Web, carga por USB**. No se incluye actualización HTTP/OTA ni se ejecuta
una carga ahora. Se conserva el trabajo local de Loader0.5 y las demás ediciones.

## Implementación en fuente

- Operación agrega selector de familia **Web propio / Arduino** o **Firmware
  AT / paquete**, casilla filesystem, lista de layouts y resumen B/MiB de
  flash esperada, código, FS y otras reservas/espacio. Destinos y botón de
  preparación reflejan la familia. ATmega conserva HEX/avrdude y su verificación.
- Web obtiene los tamaños de linkers instalados para la capacidad del perfil.
  Seleccionar actualiza linker/offset/bytes/límite. Los layouts sin FS usan el
  linker correspondiente; construir/cargar solo FS requiere casilla activada.
  No se calcula el límite de código como flash menos FS: se lee del linker.
- Los presets Web generan un INI de build en LOCALAPPDATA con el linker
  elegido; PlatformIO recibe project-dir/project-conf. No se reescribe el INI
  de producto. Recibos locales asocian BIN/hash al layout/INI y se exige que
  coincidan para cargar. Si linker/INI cambian durante un build, no se acredita
  su imagen para ese layout. Personalizado conserva imágenes externas con
  linker/entorno declarados, sin asumir su procedencia.
- AT instala BIN precompilados por USB/ROM. El manifiesto
  `afr2-esp-at-package-1` declara ESP8266, flash, SPI y layouts/regiones. Soporta
  imagen factory combinada o varios BIN en regiones no solapadas. Los offsets
  y capacidades se revisan por sector; rutas dentro del paquete, tamaño BIN
  y SHA opcional. FS exige imagen de su tamaño exacto. El dropdown de tamaños
  usa variantes reales del paquete; no mueve particiones compiladas ni recorta
  un BIN. Marcar FS incluye sus imágenes, sin modificar la tabla del proveedor.
- Firmware AT incluye regiones firmware/system; FS solo o ambos tienen sus
  selecciones propias. ROM y tamaño físico deben responder antes de escribir;
  se confirma el mapa y se ejecuta verificación contra todos los BIN elegidos.
  Manifiesto y BIN no pueden cambiar entre revisión y ejecución. Se registra
  mapa/familia/hashes, incluso si falla después. No hay erase global ni reintento.
- En AT, **Revisar paquete AT** revisa archivos aportados y tiene indicador
  propio. No se presenta como compilación Arduino. Compilar todo prepara
  ATmega y ESP según familia; Web/FS se construye solo en Web con FS activo.
- Lanzador con `-FirmwareEsp web|at` y `-Perfil nombre.json` para elegir la
  configuración inicial. No abre COM ni inicia operaciones. Una sesión ya
  abierta no cambia por lanzar otra. Perfil solicitado inexistente/ilegible
  produce error, sin sustituirlo silenciosamente por otra configuración.
- Perfiles schema1 anteriores reciben los nuevos valores Web/custom/FS sin
  alterar sus rutas. MCP informa familia/FS en status, memoria en profile y
  comparte las mismas reglas/acciones y revisión/autoaceptación existentes.

## Contrato, referencias y alcance

[Reglas y esquema completo](../contracts/LOADER_FLASH_MODES.md),
[procedencia por archivo](../provenance/loader-esp-modes-20261027.json),
[README](../../AutoFinalRobo2-Loader/README.md),
[manual portable](../../AutoFinalRobo2-Loader/MANUAL.html#modos).

Se consultaron en solo lectura `D:/AutoProyecto/FirmwareF4/Core/Src/wifi_bridge_at.c`
y `D:/AutoProyecto/AutoMicroQtDesktop/src/protocol/AtProtocol.cpp`. Describen
operación AT/consulta de versión y respuestas. No se ejecutaron manifests,
scripts, builds ni registros de AutoMicro ni se copiaron sus implementaciones.
Se conservaron autorías; el Loader implementa sus propias reglas C#.

Fuentes primarias y linkers locales se detallan en el contrato. AT+FS no se
asume disponible en ESP8266 por ejemplos ESP32. No se descargó ni distribuyó
un paquete AT: la plantilla es deliberadamente incompleta hasta contar con el
mapa real del proveedor. La instalación de AT no implementa ese protocolo en
el Mega. El firmware ESP de producto sigue siendo Arduino/LittleFS sin AT.

| Módulo | Estado real de esta ficha |
| --- | --- |
| Loader | Fuente nominal0.6.0 y documentación; no compilada/ejecutada en esta revisión. |
| Contracts | Reglas, esquema, cambio, procedencia e índices actualizados. |
| atmega | Sin cambios de fuente, build ni carga por esta ficha. |
| esp | Sin cambios de firmware/INI, build, identificación de flash ni carga por esta ficha. |
| web | Sin cambios de fuente, build ni promoción por esta ficha. |
| CAD | Sin cambios. |

Se efectuaron lecturas y edición de fuentes/documentación. No se ejecutaron
tests, compilaciones, validadores, GUI, llamadas al Loader MCP, apertura COM,
flasheos ni pruebas físicas de esta revisión. No se afirma compatibilidad con
paquetes concretos ni hardware probado. El proceso abierto conserva la versión
anterior hasta que se construya y abra la nueva con pedido expreso.
