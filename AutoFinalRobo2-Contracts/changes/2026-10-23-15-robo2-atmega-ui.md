# AFR2-20261023-15 — ROBO2 y composición ATmega con OLED/encoder

23/10/2026. Pedido directo del usuario: revisar documentación/código interno
Mega y plan maestro antes de implementar; aprovechar UNER F4, cambiar cabecera
a ROBO2, integrar OLED/encoder sin DMA y documentar emisor/receptor/validación
en ambos extremos, incluyendo un documento para retomar desde firmware ESP.

## Antes y después

Antes: endpoint/USART1 y composición opcional de comunicaciones, perfil1 sobre
UNER; biblioteca parcial OLED/encoder/renderers, sin backend TWI/composición.
Después: ROBO2/perfil2 en header único C/C++, validación compartida, ruta por
nibbles como F4, controles semánticos/correlación y composición AVR integrada.
TWI nuevo por IRQ para el Manager; no se usa la rama bloqueante sin DMA F4.
Se conservan las cuatro vistas, su raster/IDs, la fuente PROGMEM y Volver por
ítem. SW largo abre menú sin corto posterior, también al soltar en el umbral.

## Wire, productor y consumidores

Norma: [UART_PROFILE_2.md](../contracts/UART_PROFILE_2.md), criterios comunes:
[ROBO2_VALIDACION.md](../contracts/ROBO2_VALIDACION.md).

Frame: ASCII ROBO2 (cinco bytes), N:u8<=64, tokens3A/A5, versión01, ruta:u8,
CMD:u8, payload[N], XOR de todo lo anterior. Total12+N<=76. Emisor nibble alto,
receptor bajo: ESP3→Mega1=31; respuesta Mega1→ESP3=13. Ambos validan framing con
el mismo header, luego su rol/destino y longitud/comando/correlación. Perfil2
hello; no compatible con UNER/perfil1. IDs01..06/cuerpos LE conservan significado;
no se agrega UI remota ni se habilita movimiento. WS afr2-ws-1 sigue igual.

ESP inicia pedidos, Mega responde con CMD/sesión/transacción iguales. No hay
eventos/peticiones espontáneas Mega→ESP. Sesión/transacción no cero; Mega rechaza
repetidos/anteriores de sesión actual y ESP descarta respuesta que no coincida
con transacción activa. Error siempre N9; sin ACK genérico. Caps y rangos
comprobados antes de aceptar respuesta. Formato/ruta/checksum mal no ejecutan
handlers ni renuevan supervisión. Contadores de framing saturados locales.

Timeouts conservados: gap>50 ms, pedido800 ms, alive500 ms, reintento hello1000 ms,
supervisión Mega>1500 ms. STOP prioritario reservado sin intercalar frames; sin
callback/capability responde unavailable. RX fault revoca negociación, descarta
entrada incierta y callback STOP si existe. No se confirma parada física por
estado de transporte ni por GPIO inactivos en la composición.

## Composición, recursos y procedencia

`Atmega/src/main.c` integra Timer0/USART1/TWI/Manager/OLED/encoder/UI; board.c
comparte GPIO/tick con la opción UART sola, sin duplicar vectores en un target.
TWI retiene buffer, espera STOP completo, informa NACK/error, abort desactiva
IRQ antes de soltar puntero y recovery en main pulsa hasta9 veces open drain
con deadline50 ms. Sin spin, HAL, Arduino ni DMA. Manager sigue siendo la única
entrada al bus. Fuente0x3C/SSD1306/100kHz y encoder2/4pasos son candidatos.

Framebuffer1024 + TX129 + comandos4, parser88 por campos, reply76 y UART256 de
arrays. Suma de estructuras AVR estáticas alrededor1,7 KiB más constantes,
pila/runtime/temporales; no es mapa ni memoria libre probada. Fuente/textos OLED
en PROGMEM. Medir latencia de main/IRQ y muestras encoder al autorizar pruebas.
VCC/pull-ups/controlador/dirección/conexión real siguen pendientes.

Lecturas: maestro/Contracts, documentación por subsistema y código activo AVR,
ESP mega_link/broker, UNER core/v2/handle/transporte DMA F4, SSD1306/puerto I2C,
encoder y wrappers. Originales/autores preservados. Adaptación por archivo en
[provenance/atmega-robo2.json](../provenance/atmega-robo2.json). Referencia técnica
de registros TWI: [Microchip, ATmega2560 sección24](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf).

## Estado real por módulo

| Módulo | Estado de esta ficha | Pendientes |
| --- | --- | --- |
| atmega | Fuente integrada UART/TWI/Manager/OLED/encoder; cuatro vistas locales; potencia inactiva | Compilar/mapa/latencia; confirmar hardware; sensores/STOP/potencia/lease/Hall/GOTO; menús de producto/UI remota |
| esp | Parser compartido ROBO2 y recepción semántica/correlación adaptados; documento de continuación guardado | Compilar/interop/memoria, flash/layout; UI remota y servicios nuevos |
| web | Sin cambios de fuente/paquete en esta ficha; afr2-ws-1 se conserva | Reconstrucción por cambios previos; servicios remotos pendientes |
| shared | Contrato vigente, validación de ambos roles, estados/capabilities, overlays/hallazgos/procedencia | Comprobación bajo pedido; sin publicación de commits |

## Documentos para retomar

- ESP: **[docs/PROTOCOLO_ROBO2.md](../../AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md)**:
  qué enviar, recibir, validar, correlacionar y esperar; estado fuente y faltantes.
- Mega: [docs/COMUNICACION_ESP.md](../../AutoFinalRobo2-Atmega/docs/COMUNICACION_ESP.md)
  y [docs/COMPOSICION.md](../../AutoFinalRobo2-Atmega/docs/COMPOSICION.md).

No se ejecutaron tests, compilaciones, validadores, flasheos, imágenes LittleFS,
puertos serie ni pruebas físicas. Lectura de código/diff no acredita funcionamiento.
No se editaron AutoMicro/reference/Documentacion, no hay Git anidados ni publicación.
Actualizar ambos firmwares conjuntamente antes de una futura verificación autorizada.
