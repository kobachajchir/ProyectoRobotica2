# AFR2-20261028-11 — Integrar mejoras locales con origin/main

28/10/2026. Pedido humano: actualizar desde origin/main, mantener las mejoras
locales e incorporar el Loader remoto más actualizado, resolviendo los cambios.

## Origen y resguardo

- Rama local: `codex/monorepo-cad`, base anterior `fba3f79`.
- Remoto incorporado: `origin/main` en `2a72b9a`, incluyendo Loader0.9.0,
  servicio puente AVR y documentos/modelos/estudios CAD de sus commits.
- Estado local previo: Loader0.7.4, monitor HEX, verificación y USB–TTL,
  Protocol Studio, cambios documentales y exclusión de configuración privada.
- Resguardo recuperable conservado: stash `188eba5592901fcac61a50dcb402109c92012be3`,
  con archivos modificados y nuevos; los archivos privados ignorados no se
  agregan al repositorio ni se sobrescriben.

Se hizo fast-forward a origin/main y se reaplicó ese resguardo, resolviendo
los solapamientos por contenido. Los cambios locales no tenían commits propios;
la integración se registra como un commit encima del remoto, sin reescribir su
historial. No se hizo push ni se modificó la rama remota.

## Loader integrado: 0.9.1 en fuente

Se conserva íntegra la funcionalidad remota0.9.0: servicio temporal ATmega,
wrapper al baud fijo y espera tras abrir COM, instalación/restauración,
backups leídos dos veces, hfuse/hashes, último backup, Preferencias y MCP.
Se conservan las mejoras locales0.7.1..0.7.4: Python `-u`, monitor con bytes
originales y vistas ASCII/HEX/texto, reconversión sin reabrir COM, limpieza de
ventana sin borrar logs, KEY sostenido y comprobación nueva antes de verificar.

Resoluciones que requieren combinación, además del merge textual:

1. `EspTransport` admite `mega_reset`, `mega_bridge` y `usb-ttl`, con selector y
   confirmaciones propias. El valor anterior `mega-usb` se acepta y normaliza
   a `mega_reset` al cargar un perfil; no cambia el montaje físico seleccionado.
   USB–TTL conserva su COM, lógica3,3V, VCC sin conectar y UART Mega aislada;
   no se pide RESET-GND del Mega para ese modo.
2. El constructor de argumentos conserva tanto el wrapper remoto como Python
   `-u`. La consulta previa a verificar usa una sola opción de intentos=2,
   `read_mac`, sin escritura/reset ni subida de stub. Plazo15s, o30s con wrapper.
3. Ante falta de respuesta, se pausa la verificación sin volver a escribir.
   USB–TTL/Mega físico usan su escucha74880 y aviso PUERTO LISTO. El puente de
   software conserva RESET Mega libre y baud fijo, con guía manual; no intenta
   escuchar74880 por un puente que no cambia de baud.
4. Los tres modos mantienen KEY hasta terminar consulta/carga/verificación.
   Un aviso manual no se anuncia como detección UART. Las indicaciones finales
   distinguen el flasher aún instalado, el producto ATmega y el adaptador externo.

## Protocol Studio y registros

Se conservan Constructor, formularios guiados, HEX manual, salidas, estructura
visual, Traductor, mapa/stream y ayuda. Fuente Web recuperada del resguardo.
Los IDs locales01/02 de28/10 chocaban con fichas remotas ya publicadas. Se
reasignan **solo los registros locales de Protocol Studio** a
**AFR2-20261028-09** y **AFR2-20261028-10**, con fichas, vínculos, coberturas y
procedencias actualizados. Los IDs remotos01/02 siguen identificando J1 y el
servicio de puente software. Los IDs VIS-045/046 no cambian.

Los archivos CAD y el servicio AVR llegan como contenido de origin/main; no se
ejecutan sus scripts ni se modifica su diseño por esta integración.

## Estado real

| Módulo | Estado |
|---|---|
| Loader | Fuente integrada0.9.1; sin compilar ni ejecutar esta integración |
| web | Protocol Studio conservado en fuente; sin build ni navegador |
| atmega | Servicio remoto incorporado, producto sin edición local de integración |
| esp | Documentación local conservada, firmware/data/dist sin promover |
| CAD | Contenido remoto incorporado; sin modelado ni cálculos locales nuevos |

Revisión manual de fuentes, contratos, diferencias Git y resolución de índices;
sin tests, compilaciones, validadores, servidores, COM, flasheos ni pruebas
físicas. No se declara compatibilidad probada, memoria suficiente ni hardware
aceptado. [Procedencia por archivo](../provenance/integracion-local-remoto-20261028.json).
