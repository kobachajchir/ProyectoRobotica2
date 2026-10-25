# AFR2-20261024-08 — Red y reinicio en OLED

Pedido humano: IP obtenida, avisos de arranque ESP/conexiones/Web/IP/AP,
SSID/clave/IP del AP, Sistema → Reinicio → ATMEGA/ESP, iconos F4, Volver y scroll
de opciones largas. Skill utilizada: [afr2-screen-authoring](../.agents/skills/afr2-screen-authoring/SKILL.md).

## Implementación

ATmega añade IP de conexión0x03002d, Datos de red AP0x03002e y Reinicio0x03002f.
El catálogo pasa a48nodos sin renumerar anteriores. Conserva el árbol objetivo,
historial, corto confirma, largo abre principal y Volver por opción.

Etiquetas de menú32bytes, tres filas16px y14iconos XBM de F4 en PROGMEM.
Texto seleccionado recortado, pausa800ms, avance1px/40ms, pausa800ms al final.
Render parcial de bandas; no bloquea ni llama I2C. Se distingue FillRect de
Rect (contorno) y se corrige el relleno de segmentos de secuencias existente.

EX01v5 añade estado paginado0x53 y avisos0x54. ESP produce datos reales y
distingue cliente AP, WebSocket y respuesta indexHTTP200. Avisos con iconos,
progreso temporal y detalle: arranque, servidor, IP, AP, cliente/WS, Web servida
y desconexiones. Colas acotadas/ACK/reintento; no es historial durable.

Datos AP muestran SSID, clave propia e IP; red abierta dice Sin clave. No se
transporta la clave STA a esa pantalla ni se publica la clave AP por HTTP/WS.
La captura remota se bloquea mientras se muestra clave AP; esta vista se abre
localmente. Las lecturas caducadas quedan sin dato y borran la copia de clave.

Reinicio local Mega conserva guardas de salidas/EEPROM y UART drenada. Reinicio
ESP usa job8: acepta solo detenido/sin mando y se difiere tras ACK. La placa
debe emitir su boot posterior; Solicitud aceptada no afirma reinicio completado.

Web conserva su consumidor de framebuffer para los nuevos IDs; solo se amplía
la documentación de producto, organizada en Web/ESP/ATmega. No se crea réplica
JavaScript del renderer ni nuevo mando genérico de reinicio remoto por encoder.

## Fuentes y autorías

Referencia de solo lectura: `D:/AutoProyecto/FirmwareF4/Core/Src/oled_utils.c`
(notificaciones simples, Wi-Fi/AP, ventanas de texto y MenuScroll),
`fonts.c/h`, `menu_definition.c`, `notificationWrappers.c` y documento de
notificaciones numeradas. Autor original kobac conservado. Se adapta la cadena
al snapshot/servicios/Manager propios, sin System/HAL/DMA ni comandos AT.
[Procedencia por archivo y recurso](../provenance/oled-network-restart-20261024.json).

## Estado y presupuesto

| Módulo | Estado real |
| --- | --- |
| ATmega | Producto/comunicación/biblioteca compilados; red/avisos/reinicio/scroll escritos, sin placa |
| ESP | Firmware candidato compilado; producción/ACK de datos y avisos escritos, sin radio/UART/reset real |
| Web | TypeScript/Vite/paquete/promoción completados; visor framebuffer existente, nueva guía de uso |
| CAD | Sin cambios |

AVR final: .text63422+.data810=**64232bytes flash**;
SRAM estática810+3978+9=**4797/8192bytes**. Margen estático3395bytes, no cota
de pila total. `main`237bytes y renderer120bytes dynamic,bounded según .su;
no se suman como peor caso certificado. Recursos enumerados de flash terminan
en0x3494, dentro del direccionamiento PROGMEM usado; símbolos/logs en build08.

ESP: RAM45852/81920, programa423665/1044464bytes del candidato.
Web:1452714bytes gzip,7activos, estimación FS1540096bytes; modelo final y
layout nominal3MiB conservados. La flash física sigue sin identificar.

[Evidencia y hashes](../documentation/builds/2026-10-24-08/README.md).
Compilaciones por el pedido humano previo de compilar/corregir la integración.
Sin tests/validadores independientes, render de navegador, monitor serie,
imagen FS, flasheos, reinicios ni pruebas físicas. La revisión de fuente y
los recursos copiados no acreditan paridad F4 ni funcionamiento en placas.
`AFR2_SEQUENCE_MEMORY_APPROVED=0` y `AFR2_HALL_CONNECTED=0` se conservan.
