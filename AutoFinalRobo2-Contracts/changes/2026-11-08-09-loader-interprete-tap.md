# AFR2-20261108-09 - Loader como depurador USB: catálogo de tramas, intérprete y fuente TAP

Pedido del usuario (08/11/2026): usar el Loader como depurador USB con el firmware del auto cargado:
comparar lo que llega contra un listado de comandos (nombre, ID, qué hace), activar o desactivar el
intérprete (desactivado por defecto, habilitado solo con el listado JSON), un botón que alterne entre
enviar e intérprete, elegir por software si se ve el RX del ESP o del ATmega (al cambiar se limpia el
log y se conserva el listado; cada trama con hora y origen), detectar los resets del ESP y su razón, y
una regla de skill para que todo ABM de comandos actualice el listado.
Plan: [docs/PLAN_LOADER_INTERPRETE_Y_TAP.md](../docs/PLAN_LOADER_INTERPRETE_Y_TAP.md).

Versión: 1.4.0 (subversión). Loader 2.2.0.

## Cómo llega el USB y cómo se elige la fuente

El USB del Mega es el conversor 16U2 cableado a D0/D1 = USART0 del ATmega2560: el PC ve exactamente lo
que el firmware escribe en TX0. El ESP no tiene USB propio; su UART0 es el enlace con el ATmega por
USART1 (D18/D19). El ATmega recibe lo que manda el ESP y arma lo que el ESP recibe, así que el selector
es un multiplexor dentro de `afr2_debug`: el PC lo cambia con una línea de texto por RX0 (D0).

## Qué cambió

### Contracts
- `contracts/wire_catalog.json` (esquema `afr2-wire-catalog-1`): 53 comandos con id, nombre, familia,
  nombre WS, qué hace, largos y campos de pedido/respuesta, `sensitive` y `retired`; envoltura
  (cabecera, rutas, prefijos de sesión/transacción/resultado) y tablas de nombres (resultados, razón de
  reset del ESP, bits de reset AVR, `exccause` de Xtensa, eventos de red). Largos cotejados con el
  despachador del ATmega, no con documentos históricos.
- Skill `.agents/skills/afr2-wire-commands/SKILL.md` con la regla: toda alta/baja/modificación de un
  CMD actualiza el catálogo en el mismo cambio; las bajas quedan `retired` y su ID no se reutiliza;
  `sensitive` obligatorio para comandos con SSID/claves. Referenciada desde `AGENTS.md` raíz y el README.

### ATmega (fuente, `AFR2_DEBUG_TAP=1` por defecto en el Makefile)
- `afr2_link`: eventos de diagnóstico `FRAME_RX` (trama completa del ESP) y `FRAME_TX` (respuesta
  armada). No cambian el manejo del enlace.
- `afr2_debug`: RX0 con anillo de 16 B y línea de control `#AFR2 TAP <0..4>` / `#AFR2 TAP ?`, respuesta
  `TAP mode=<n> <nombre> baud=<b>` (reintentada hasta que entra). Modos: 0 log (siempre tras reset),
  1 tramas ESP→ATmega, 2 tramas ATmega→ESP, 3 ambas, 4 bytes crudos del ESP en bloques de hasta 32 B.
  Registros binarios TAP1 `0x1E 'T' tipo flags ms:u32 n datos xor` intercalados con el texto, escritos
  enteros o contados como perdidos (`STAT2 tapLost`). En modos TAP el log solo deja `AVR`, `ESP`,
  `LINK lost`, `LINK hello`, `TAP` y `STAT`.
- Redacción en el origen de `WIFI_POLL`, `WIFI_UPDATE` y `NETWORK_STATE`: en modo trama solo
  ruta/CMD/largo; en modo crudo los bytes de payload salen como `00`.
- Resets del ESP sin cambiar el protocolo: `ESP ROM probable` cuando llegan errores de UART tras ≥50 ms
  de silencio del ESP (su ROM imprime a 74880), `n=` en la línea `ESP BOOT` y `STAT2 espRst`.
- Con TAP el log pasa a **250000 baudios** (divisor exacto a 16 MHz) y el anillo TX a 256 B.
  `AFR2_DEBUG_TAP=0` deja el log como antes (115200, sin RX).
- SRAM estimada sin compilar: ~185 B más (RX 16 + línea 24 + bloque crudo 32 + TX +128 + estado).

### Loader 2.2.0 (fuente)
- `TapProtocol.cs`: separa texto y registros TAP1 en el worker de `SerialCapture`; el texto sigue el
  camino anterior y cada registro deja una línea resumen en el log con hora del PC, ms del ATmega y
  origen. Colas propias de registros y líneas, acotadas, independientes de la pausa de la vista.
- `WireCatalog.cs`: carga y valida el catálogo (IDs, largos, suma de campos, enums).
- `WireDecoder.cs`: decodifica envoltura, resultado y campos; buscador de tramas en bloques crudos
  (port de `afr2_wire_push`); rastreador de resets del ESP (línea `ESP BOOT`, volcado `Exception (n)`,
  `epc1`, `excvaddr`, `last failed alloc`, watchdog, `ESP ROM probable`, trama BOOT).
- `MainForm.Interpreter.cs`: botones **Ver intérprete ↔** e **Intérprete: desactivado/activado**
  (habilitado solo con catálogo válido), **Cargar catálogo…**, selector **Fuente USB**. Grilla de
  tramas (virtual, hasta 6000), detalle, catálogo con contadores y filtro por clic, pestaña **Resets
  ESP**, Guardar tramas (TSV en registros). Cambiar de fuente envía la línea de control solo por
  elección del usuario, espera la confirmación 1,5 s, limpia log y tramas y conserva el catálogo.
- Perfil: clave `WireCatalog` (por defecto `{Repo}\AutoFinalRobo2-Contracts\contracts\wire_catalog.json`).
  Baudios 250000 y 500000 en el monitor. AGENTS, README y MANUAL actualizados.

### Web (fuente)
- Protocol Studio: `catalog.ts` sincronizado con el catálogo (faltaban `0x1E`, `0x24`, `0x32`, `0x33`,
  `0x55`; `EXT_STATE` responde 19 B, no 16). `codec.ts` decodifica `EXT_STATE` de 19 B y espera EX01 v13.
- Versión 1.4.0 en `package.json` / `package-lock.json`.

## Pendiente

- Fase 5 del plan, **BT02** (causa de excepción, epc1, uptime y heap previos, etapa en RTC del ESP):
  cambia la trama BOOT y sube EX01 a v14 en los dos firmwares. Requiere decisión del usuario.
- Botón "Traducir epc1" (addr2line con el ELF cargado) en el Loader.
- Fase 6 opcional: capturar la ROM a 74880 cuando el ATmega resetea al ESP (circuito KEY/RST sin armar)
  y canal `Serial1`→D17.
- Medir que el 16U2 de esta placa acepte 250000 y que el modo "enlace completo" no pierda registros.

## Estado

atmega=fuente editada, **sin compilar, cargar ni probar** (SRAM sin medir); esp=sin cambios (solo la
versión compartida 1.4.0); web=fuente editada (catálogo de Protocol Studio y versión), sin build ni
navegador; loader=fuente 2.2.0 sin compilar ni abrir puertos.
