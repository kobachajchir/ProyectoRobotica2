# EX01v5 — Red, avisos y reinicio desde OLED

AFR2-20261024-08. Firmware de producto0.7.0, framing ROBO2/perfil2 y
afr2-ws-1 conservados. Extiende [v4](EX01_SYSTEM_SEQUENCE_4.md), sin cambiar
sus comandos. Nuevo bit `AFR2_EXT_F_NETWORK_UI=0x2000`; máscara0x3fff.
Se actualizan ambos firmwares para las funciones nuevas. ESPv5 puede conservar
funciones previas con AVRv4; no le envía el canal de red v5.

## Estado de red — 0x53

ESP envía una imagen acotada de presentación, sin consultas desde el renderer.
El cuerpo de request comienza con bootIdESP LE[0..3], revisión no nula LE[4..5],
página[6], longitud[7] y datos[8..]. Longitud máxima32; cuerpo máximo40bytes.
La respuesta exitosa repite los primeros7bytes (respuesta completa16bytes).

| Página | Datos |
| --- | --- |
| 0 | 15bytes: modo[0] (0off/1STA/2AP/3ambos), STA con IP[1], AP listo[2], clientes AP[3], servidor HTTP iniciado[4], RSSI int16LE[5..6], IP STA[7..10], IP AP[11..14] |
| 1 | SSID STA,0..32bytes |
| 2 | SSID AP,0..32bytes |
| 3 | Primeros0..32bytes de clave AP |
| 4 | Resto0..31bytes de clave AP |

El AVR exige páginas ordenadas y la misma identidad/revisión. Un cambio de
revisión invalida la presentación anterior hasta completar las cinco páginas.
No se anuncia conexión con IP cero ni AP sin dirección; modos/flags coherentes.
Una clave AP no vacía debe tener8..63bytes. No se envía la clave STA por este canal.

ESP observa estado cada750ms entre instantáneas, congela la copia mientras
completa sus cinco ACK y no reinicia esa transferencia por cambios de RSSI o
conteo de clientes. Envía como máximo una petición cada200ms y
refresca la imagen completa3s después del último commit. La imagen OLED caduca
a los5s independientemente del estado Wi-Fi resumido anterior. Al caducar o
revocar enlace se invalida y borra la copia de clave. Estado desconocido se
presenta como Sin datos/Sin IP, no como0.0.0.0 conectada. Todos esos tiempos son
parámetros de fuente compilada, sin mediciones de radio/UART/latencia física.

El snapshot de dibujo contiene punteros prestados a strings preparados por el
servicio en main, estables hasta que termina Render. Ninguna ISR los escribe;
el renderer no consulta red, registros, transporte ni I2C.

## Avisos — 0x54

Cuerpo10bytes: bootIdESP LE[0..3], secuencia no nula LE[4..7], tipo[8], sujeto[9].
Respuesta exitosa: secuencia LE,4bytes (respuesta completa13bytes). Reintentar
una secuencia recibida no repite el aviso. La identidad debe coincidir con la
imagen de red completa. El registro de arranque se deduplica además por bootId.

| Tipo | Significado OLED | Código de pantalla |
| --- | --- | --- |
| 1 | ESP iniciada | 0x040001 |
| 2 | Servidor HTTP disponible | 0x040002 |
| 3 | STA adquirió IP | 0x040003 |
| 4 | AP creado y con IP | 0x040004 |
| 5/6 | Dispositivo asociado/desasociado al AP | 0x040005/0x040006 |
| 7/8 | Transporte WebSocket abierto/cerrado | 0x040007/0x040008 |
| 9 | Respuesta HTTP200 de index iniciada | 0x040009 |
| 10/11 | STA perdió conexión / AP dejó de estar activo | 0x04000a/0x04000b |

Cliente Wi-Fi usa AID del evento SDK cuando existe; no se deriva del número de
clientes. WebSocket y servir index son eventos distintos. Web servida significa
que el servidor encoló la respuesta; no prueba descarga completa ni render del
navegador. Los errores404/406/503 y otros recursos no generan ese aviso.

La cola ESP conserva12eventos, agrupa anuncios repetidos de estado y conserva
el orden de transiciones de cliente distintas. Si se llena, descarta avisos nuevos y cuenta
la pérdida internamente; no es un historial durable. La cola AVR conserva4;
con3ocupados responde ocupado para reintentar después. STOP conserva prioridad.
Los detalles de red son el estado confirmado al recibir el evento, no un registro
histórico de credenciales. Boot puede reenviarse como identidad retenida después
de recuperar enlace; no se anuncia como reinicio nuevo si ese bootId ya se mostró.

## Pantallas y navegación

- Wi-Fi → IP de conexión,0x03002d: SSID y dirección STA o acceso AP disponible.
- Wi-Fi → Datos de red AP,0x03002e: SSID, clave propia del AP e IP. Red abierta
  indica Sin clave. El fallback sigue abierto, sin credenciales embebidas.
- Sistema → Reinicio,0x03002f: Reiniciar ATMEGA, Reiniciar ESP y Volver.

48nodos de navegación y11tipos de aviso; los IDs anteriores no se renumeran.
Todos los submenús conservan Volver. Inicio mantiene su invocador Menu. SW
largo abre el principal, corto confirma; la liberación de largo no confirma.
Los avisos pueden cerrarse con corto, o abandonar su contexto con largo.

14recursos XBM seleccionados de F4 `fonts.c` se conservan en PROGMEM con
acceso AVR. Tres filas16px, iconos con relación de aspecto conservada y texto
recortado a su ventana. Etiquetas de hasta32bytes; solo la seleccionada desplaza
texto: pausa800ms,1px/40ms, pausa final800ms. SSID/clave larga también desplazan
en sus vistas. ASCII7x10; bytes sin glifo se representan con `?`.

No hay delays ni DMA. Actualización parcial del texto seleccionado cada40ms;
avisos/red100ms, vistas de datos250ms y menús estáticos1s. Solo framebuffer y
dirty pages; el servicio único OLED/I2C Manager mantiene snapshot TX por página.
Se agrega FillRect real para borrar bandas y rellenar progreso de secuencias;
Rect conserva contorno, sin reinterpretar su semántica anterior.

## Clave AP y captura

La clave AP se transporta solo por UART interna para dibujo local. No se añade
a wifi.status, system.info, HTTP, WebSocket, logs ni fuentes. La captura remota
se rechaza mientras el aviso/vista muestra una clave AP no vacía; no se puede
abrir Datos AP mediante acción remota. La clave de router/STA sigue oculta.
Una red AP abierta puede capturarse porque no contiene una clave.

## Reinicio físico solicitado desde el menú

Reiniciar ATMEGA requiere salidas detenidas y EEPROM libre; detiene/revoca y
programa watchdog después de250ms y USART1 TX vacía, igual al servicio v4.
Las acciones de reinicio no se admiten por ui.action genérico.

Reiniciar ESP crea job Wi-Fi8. ESP exige v5, salidas detenidas recientes, sin
control ni parada pendientes. Devuelve resultado2 Solicitud aceptada por el
canal de jobs. Solo tras ACK de ese resultado, y sin cambio de generación,
programa `ESP.restart()`300ms después. Espera de aceptación limitada3s;
no se reinicia si falla/interrumpe esa entrega. Nuevos mandos quedan bloqueados
mientras se prepara. ACK de solicitud no es comprobación de arranque.

No se cambia EEPROM, LittleFS, fuses, bootloader ni jumpers por estas acciones.
Una configuración guardada se vuelve a cargar al arrancar; no se promete
persistencia de cambios que el usuario mantuvo solo en RAM.

## Evidencia real

Atmega, ESP y Web compilados; documentación Web empaquetada/promovida.
[Build08](../documentation/builds/2026-10-24-08/README.md). No se ejecutaron
reinicios, pruebas de OLED/radio/UART/scroll, tests, validadores independientes,
imagen FS ni flasheos. Iconos copiados no acreditan paridad de raster F4.
Memoria de secuencias y Hall permanecen con macro0; CAD/pinout sin cambios.
