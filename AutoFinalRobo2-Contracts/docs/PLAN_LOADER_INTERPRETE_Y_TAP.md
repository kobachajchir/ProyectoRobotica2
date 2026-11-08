# Plan: intérprete de tramas en el Loader, selector de fuente USB y resets del ESP

Creado el 08/11/2026 (AFR2-20261108-08). Es un plan de trabajo: **nada de esto está implementado**.
No se compiló, cargó ni probó nada. Las fases que tocan firmware requieren compilar y flashear, y eso
se hace solo con pedido expreso.

Alcance: **monitor de depuración con el firmware del producto cargado y el auto funcionando.** El modo
flasher puente queda fuera: ahí el ATmega no corre la aplicación y no hay enlace real que observar.

Objetivo: usar el Loader como depurador USB. Que reconozca las tramas ROBO2 que llegan, las compare
contra un catálogo JSON de comandos (nombre, ID, qué hace) y las liste con hora y origen; que se pueda
elegir por software si se ve lo que recibe el ATmega o lo que recibe el ESP; y que detecte cada reset
del ESP con su causa, para depurar esos reinicios.

---

## 1. Cómo llega hoy lo que leemos por USB

### Camino físico

```
 PC (Loader, COMx)
   │ USB
   ▼
 ATmega16U2 (conversor USB-serie de la placa Mega)
   │ TX/RX TTL 5 V
   ▼
 D0 (RX0) / D1 (TX0) = USART0 del ATmega2560  ◄── lo único que el PC puede ver
                                                    
 ATmega2560 ── USART1: D18 (TX1) → E_RX │ E_TX → D19 (RX1) ── ESP8266 UART0 (GPIO1/GPIO3)
               (enlace ROBO2, 115200 8N1, ESP pregunta / ATmega responde)
```

- El PC **solo ve USART0 del ATmega**. No hay ningún multiplexor de hardware: lo que aparece en el
  monitor es exactamente lo que el firmware del ATmega decide escribir en TX0 (D1).
- El ESP **no tiene USB propio** en este montaje. Su UART0 *es* el enlace con el ATmega; el PC nunca
  lo ve directamente.
- Los selectores D0/D1 y D4/D5 del shield deben quedar aislados: si el ESP estuviera en D0/D1, su TX
  chocaría con TX0 del ATmega (ver `hardware/WIFI_SHIELD.md`).

### Qué escribe el ATmega en USART0 según el firmware cargado

| Firmware en el ATmega | Qué llega al PC | Baudios |
|---|---|---|
| Producto (`AFR2_DEBUG_LOG=1`, AFR2-20261108-01) | Log de texto `ms ETIQUETA texto`: `AVR BOOT`, `ESP BOOT`, `ESP>`/`ESP+`, `LINK`, `NET`, `CAP`, `ENC`, `LOOP`, `STAT`. **Ninguna trama ROBO2**, a propósito (0x53 lleva la clave del AP). | 115200 |
| Producto con `AFR2_DEBUG_LOG=0` | Nada | — |
| Flasher puente (`service/esp_uart_bridge.c`) — **fuera del alcance** | Copia transparente USART0↔USART1: se ve la UART0 del ESP en crudo. El ATmega no corre la aplicación, así que no hay enlace real. | `BridgeBaud` fijo |

### Dato clave

El ATmega está en la otra punta del enlace, así que **ve las dos direcciones**:

- **RX ATmega** = lo que el ESP transmite (pedidos ruta `0x31`, BT01, texto de crash): llega por RX1.
- **RX ESP** = lo que el ATmega transmite (respuestas ruta `0x13`): lo arma el propio ATmega.

Por eso el selector se puede hacer **por software**, dentro del ATmega: elegir qué se copia a TX0.

---

## 2. Cómo variar la fuente por software

### 2.1 Multiplexor en el ATmega (`afr2_debug`, opción de compilación `AFR2_DEBUG_TAP=1`)

Fuentes que el ATmega ya tiene a mano:

1. Su propio log (lo actual).
2. Bytes que entran por RX1 (lo que manda el ESP): están en el ISR/anillo de `afr2_uart1.c` y pasan
   por `Afr2Link_Push` → parser `afr2_wire_push`.
3. Tramas que el ATmega responde: `link->reply` antes de copiarse a TX1.

Modos propuestos:

| Modo | Nombre en el Loader | Qué se escribe en TX0 |
|---|---|---|
| 0 | ATmega (log) | Log actual. **Modo por defecto y al reiniciar.** |
| 1 | RX ATmega (ESP→ATmega) | Tramas completas recibidas del ESP + líneas de reset/crash del ESP |
| 2 | RX ESP (ATmega→ESP) | Tramas que el ATmega responde |
| 3 | Enlace completo | 1 + 2 intercaladas por orden real |
| 4 | RX ATmega crudo | Bytes de RX1 tal cual llegan (incluye ruido, tramas cortadas, ROM): para resets |

En los modos 1–4 el log de texto se silencia salvo lo crítico: `AVR BOOT`, `ESP BOOT`/`ESP RESET`,
`ESP>` (crash), `LINK lost`, `LINK hello` y `TAP lost=n`. Es lo que interesa para depurar resets.

### 2.2 Canal de control: el PC le dice al ATmega qué modo usar

- Hoy USART0 es solo TX. Se habilita **RX0** en la misma instancia `afr2_usart` (anillo de 16 B,
  `Afr2Usart_Read`), con un parser de línea de hasta 24 caracteres.
- Comandos ASCII, solo estos:
  - `#AFR2 TAP <n>\r\n` → cambia de modo.
  - `#AFR2 TAP ?\r\n` → consulta.
- Respuesta siempre como línea de log: `<ms> TAP mode=<n> <nombre> baud=<b>`. **El Loader solo da el
  cambio por hecho cuando ve esa confirmación.**
- Regla de seguridad: RX0 solo cambia la salida del log. **Nunca** inyecta bytes al enlace, nunca
  ejecuta acciones ni toca motores. Cualquier otra línea se descarta.
- El modo no se guarda en EEPROM: tras un reset del ATmega (incluido el auto-reset al abrir el puerto)
  vuelve a 0. Así nunca queda un auto "mudo" por error.
- No hay conflicto con el bootloader: el stk500v2 solo escucha al arrancar; la línea se manda con la
  aplicación ya corriendo. El Loader ya cierra el monitor antes de usar avrdude.

### 2.3 Formato de registro TAP1 (mezclado con el texto)

```
0x1E  'T'  tipo  flags  ms:u32LE  n:u8  datos[n]  xor
```

- `tipo`: 1 trama ESP→ATmega, 2 trama ATmega→ESP, 3 bloque crudo de RX1, 4 estado/contadores.
- `flags`: bit0 redactado, bit1 truncado, bit2 trama con error de parser (checksum, token, versión).
- `xor`: de todos los bytes desde `'T'`.
- El log de texto solo usa ASCII imprimible + CR/LF, así que `0x1E` nunca aparece en él: el Loader
  separa texto y registros sin ambigüedad.
- Escritura todo-o-nada en el anillo TX (`Afr2Usart_Write` ya devuelve falso si no entra); lo que no
  entra se cuenta y sale como `TAP lost=n` en `STAT`.

### 2.4 Privacidad (redacción en el origen)

- Comandos sensibles: `0x53 NETWORK_STATE` (clave del AP), `0x30/0x31 WIFI_POLL/UPDATE` (SSID/clave
  de borradores). Se marcan `"sensitive": true` en el catálogo.
- En modos de trama, el ATmega manda solo cabecera, CMD y largo, con `flags.redactado`.
- En modo crudo, el parser va en paralelo: el CMD llega (offset 10) antes que el payload, así que los
  bytes del payload de un comando sensible salen como `00` en vivo. El Loader muestra "redactado" en
  vez de "checksum malo".
- La clave nunca sale por USB. Igual que hoy: el Loader no la reconstruye ni la registra.

### 2.5 Baudios

- El enlace va a 115200 **en cada dirección**. El modo 3 (las dos direcciones + cabeceras + log) no
  entra en un USB a 115200.
- Propuesta: en builds con TAP, log a **250000** (divisor exacto a 16 MHz, error 0 %; 115200 tiene
  ~2,1 %). Si se mide que falta, **500000** (también exacto, con U2X).
- Loader: agregar 250000 y 500000 a la lista de baudios. Hay que medir que el 16U2 los acepte en esta
  placa: no se da por probado.
- SRAM estimada (sin compilar): RX0 16 B + línea 24 B + anillo TX 128→256 B (+128) + estado ~16 B ≈
  **185 B** sobre los ~1500 B libres de AFR2-20261108-01. Se controla con `STAT stack>=`.

### 2.6 Alternativas de hardware (no son parte del plan base)

- **Adaptador USB-TTL 3,3 V** en la cabecera directa `ESP UART` (solo RX al TX del ESP), como segundo
  COM: es la única forma de leer el texto de la ROM a 74880 en un reset espontáneo.
- **Canal de depuración propio del ESP**: `Serial1` (TX solo, GPIO2) → RX2 del Mega (D17), con
  `Serial1.setDebugOutput(true)` para que el volcado de crash salga por ahí y no por el enlace.
  Pendiente verificar si GPIO2 está accesible en el shield, el nivel 3,3 V→5 V (VIH mín. 3,0 V) y que
  GPIO2 quede en alto al arrancar (pin de arranque).

---

## 3. Detección de resets del ESP y su causa

### 3.1 Lo que ya existe (verificado en fuente)

| Señal | Dónde nace | Qué da | Límite |
|---|---|---|---|
| Trama BOOT `0x52` "BT01" (14 B, sin sesión) | `mega_link.cpp` al arrancar el ESP | bootId, `rst_info.reason` (0..6), versión, EX | Solo la razón: sin `exccause`, `epc1`, uptime ni heap |
| Línea `ESP BOOT id=… rst=n <nombre>` | `afr2_debug.c` | La anterior, legible | — |
| Líneas `ESP>`/`ESP+` | Volcado del core por UART0 a 115200 | `Exception (n)`, `epc1=…`, `last failed alloc` | 16 líneas cada 2 s; anillo de 128 B: puede perder líneas |
| `LINK lost` / `LINK hello, new session` | `afr2_link` | Corte del enlace y nueva sesión | No distingue reset de cuelgue |
| `LINK rx-fault xN` al arrancar | ROM del ESP a 74880 sobre RX1 a 115200 | Indicio de arranque de ROM | El texto `rst cause:` no se puede leer a 115200 |
| `system.info` / `hello` (Web) | ESP | `resetReason` | No llega al Loader |

Faltan: causa de excepción y direcciones cuando el volcado se pierde, tiempo que llevaba encendido
antes de caerse, memoria libre mínima, qué estaba haciendo, historial y contador de resets.
Un `hw-watchdog` o un corte de alimentación no dejan volcado.

### 3.2 Propuesta: BT02 (ESP → ATmega, mismo CMD `0x52`)

Payload de 44 B (≤ 64), sin sesión ni transacción, como BT01:

| Offset | Campo | Tipo | Origen |
|---|---|---|---|
| 0 | `"BT02"` | 4 B | constante |
| 4 | nodo | u8 | `AFR2_NODE_ESP` |
| 5 | bootId | u32 | actual |
| 9 | reason | u8 | `rst_info.reason` |
| 10..12 | versión | 3×u8 | actual |
| 13 | EX | u8 | actual |
| 14 | exccause | u8 | `rst_info.exccause` |
| 15 | epc1, epc2, epc3, excvaddr, depc | 5×u32 | `rst_info` |
| 35 | bootCount | u16 | memoria RTC (se pierde solo con power-on) |
| 37 | uptime anterior (s) | u32 | RTC, actualizado cada 1 s |
| 41 | heap libre mínimo | u16 | RTC |
| 43 | última etapa | u8 | RTC: 1 Wi-Fi, 2 HTTP/archivo, 3 WS, 4 LittleFS, 5 enlace Mega, … |

- La "última etapa" es una marca que el ESP escribe en RTC al entrar a cada zona. Sirve para casos
  como AFR2-20261107-53 (reinicio al servir el modelo 3D): dice dónde estaba al caerse.
- El ATmega acepta BT01 y BT02. Es un cambio de protocolo de los dos firmwares: **propuesta EX01
  v13→v14**, con su ficha y estados atmega/esp/web.
- El ATmega imprime una línea nueva:
  `ESP RESET n=3 rst=2 exception cause=28 LoadProhibited epc1=0x4020a1b3 up=412s heap>=5120 etapa=HTTP`,
  y suma `espRst=n` (resets desde que arrancó el ATmega) a `STAT`.
- La Web puede mostrar lo mismo en `system.info` (opcional, otra ficha).

### 3.3 Texto de la ROM (`rst cause:x, boot mode:(a,b)`)

- **Reset provocado por el ATmega** (D33 RST, circuito KEY/RST, todavía no armado ni medido): el
  ATmega pasa USART1 a 74880 durante ~300 ms, captura la línea de la ROM, la manda como `ESP ROM …` y
  vuelve a 115200. Depende de que exista ese circuito.
- **Reset espontáneo**: el ATmega no sabe cuándo pasa, y la línea de la ROM dura ~8 ms. Solo se marca
  `ESP ROM probable` cuando llega una ráfaga de `rx-fault` tras silencio. El texto completo requiere el
  adaptador del punto 2.6.

### 3.4 En el Loader: panel "Resets ESP"

- Tabla: hora PC, ms AVR, bootId, n.º de reset, razón, `exccause` con nombre, `epc1`, uptime anterior,
  heap mínimo, etapa, fuente (BT01/BT02/línea de log/ROM/volcado).
- **Funciona con el firmware actual**: en modo log se parsean las líneas `ESP BOOT …` y `ESP> Exception
  (n)` / `epc1=…` que ya existen.
- Botón opcional "Traducir epc1": corre `xtensa-lx106-elf-addr2line -pfiaC -e <ELF> <epc1>` con rutas
  configurables (`Addr2line`, `EspElf`), solo al pulsarlo. Avisa que el ELF tiene que ser el mismo
  que está cargado en el ESP.
- Tablas de nombres (`rst_info.reason` y `exccause` de Xtensa) dentro del catálogo JSON.

---

## 4. Catálogo JSON de comandos

Archivo: `contracts/wire_catalog.json`, esquema `afr2-wire-catalog-1`.

Fuentes que se cruzan al armarlo:

- Autoridad de IDs y largos: `afr2_wire.h`, `afr2_product_wire.h`, `afr2_sensor_wire.h`,
  `afr2_network_wire.h`, `afr2_battery_wire.h`.
- Nombre WebSocket y estado: `commands.json`.
- Descripciones y largos de cuerpo: `AutoFinalRobo2-Web/src/features/protocolStudio/catalog.ts`.

Diferencias ya detectadas en `catalog.ts`: faltan `0x1E DRIVE_PWM`, `0x24 TOUCH_UI`,
`0x32/0x33 DIAG_POLL/RESULT` y `0x55 BATTERY`; dice "EX01 versión vigente 8" y el header está en 13.

Forma:

```json
{
  "schema": "afr2-wire-catalog-1",
  "updated_by": "AFR2-…",
  "envelope": {
    "header": "52 4F 42 4F 32", "tokens": "3A A5", "version": 1,
    "routes": { "0x31": "ESP→ATmega (pedido)", "0x13": "ATmega→ESP (respuesta)" },
    "request_prefix": [{ "name": "session", "type": "u32le" }, { "name": "transaction", "type": "u32le" }],
    "response_prefix": [{ "name": "session", "type": "u32le" }, { "name": "transaction", "type": "u32le" },
                        { "name": "result", "type": "u8", "enum": "result" }],
    "no_prefix": ["0x52"]
  },
  "commands": [
    { "id": "0x13", "name": "EXT_DRIVE", "ws": "robot.drive", "family": "EX01",
      "does": "Consigna de movimiento; reserved:u32 = 0",
      "request": { "bytes": 12, "fields": [ { "name": "reserved", "type": "u32le" }, "…" ] },
      "reply": { "bytes": 0 },
      "sensitive": false, "retired": false, "since": "AFR2-20261023-19" }
  ],
  "enums": {
    "result": ["OK", "BAD_REQUEST", "UNAVAILABLE", "SESSION", "INTERNAL"],
    "esp_reset_reason": { "0": "power-on", "1": "hw-watchdog", "2": "exception", "3": "soft-watchdog",
                          "4": "soft-restart", "5": "deep-sleep", "6": "external-reset" },
    "xtensa_exccause": { "0": "IllegalInstruction", "3": "LoadStoreError", "6": "IntegerDivideByZero",
                         "9": "LoadStoreAlignment", "28": "LoadProhibited", "29": "StoreProhibited" }
  }
}
```

- `bytes` puede ser un número o una lista (`[12, 16]`); los `fields` son opcionales al principio.
  Sin ellos se muestra el HEX del cuerpo.
- Tipos: `u8`, `i8`, `u16le`, `i16le`, `u32le`, `i32le`, `bytes:n`, `ascii:n`, con `unit` y `enum`
  opcionales.
- Los retirados quedan con `"retired": true` (ej. `0x19 HALL_CAL`). Su ID no se reutiliza.

## 5. Regla en una skill

- Skill nueva `.agents/skills/afr2-wire-commands/SKILL.md` en Contracts, referenciada desde el
  `AGENTS.md` raíz y el README de Contracts.
- Regla: **toda alta, baja o modificación de un CMD** (ID, nombre, largo, campos, dirección o
  sensibilidad) actualiza en el mismo cambio `wire_catalog.json`, `commands.json` y `catalog.ts`, y
  sube `updated_by`. Una baja se marca `retired` y nunca se borra ni se reutiliza.
- Incluye la lista de lugares a revisar (headers, `afr2_link`, `mega_link.cpp`, broker ESP, Web) y la
  obligación de marcar `sensitive` si el payload lleva SSID, clave o datos privados.

## 6. Loader: intérprete

### Interfaz (pestaña Monitor serie, zona de abajo)

- Barra nueva sobre el panel de envío:
  - `[Enviar ⇄ Intérprete]`: alterna entre la pestaña de envío actual (sin cambios) y la de intérprete.
  - `[Intérprete: desactivado]`: **apagado por defecto**. Deshabilitado y con explicación mientras no
    haya catálogo válido cargado.
  - `[Cargar catálogo…]` + ruta + estado: "49 comandos, 0 errores" o la lista de errores.
  - Selector **Fuente** (fase 4; ver punto 7).
- Pestaña Intérprete:
  - **Arriba, tramas detectadas**: hora PC (`HH:mm:ss.fff`), ms AVR (si viene en TAP1), origen
    (ESP→ATmega / ATmega→ESP / Loader TX), CMD, nombre, qué hace, sesión/transacción,
    resultado, largo, campos decodificados y estado (OK, checksum malo, desconocido, redactado,
    truncado). Al lado, el detalle en HEX.
  - **Abajo, el catálogo**: ID, nombre, qué hace, veces visto, último visto. Un clic filtra las tramas;
    elegir una trama marca su comando.
  - Pestaña o sección "Resets ESP" (punto 3.4).
- Con el intérprete apagado, los registros TAP1 igual se sacan del texto y se muestran como una línea
  corta `[TAP ESP→ATmega CMD 0x13 23 B]`, para que el monitor no se llene de basura.
- Exportar lo detectado a `LOCALAPPDATA\…\logs\serial-….frames.jsonl` junto a la captura existente.

### Código (todo en `AutoFinalRobo2-Loader/src`)

| Archivo | Contenido |
|---|---|
| `WireCatalog.cs` | Carga y valida el JSON: esquema, IDs únicos en hex, largos ≤ 64, suma de campos = bytes |
| `WireScanner.cs` | Separador TAP1/texto + port C# de `afr2_wire_push` para encontrar tramas dentro de los bloques crudos (modo 4) |
| `WireDecoder.cs` | Envoltura (sesión/transacción/resultado), campos por catálogo, BT01/BT02, nombres de razón |
| `MainForm.Interpreter.cs` | Botones, grillas, filtro, panel de resets |
| `MainForm.SourceSelector.cs` | Selector, línea de control, espera de confirmación |
| `SerialCapture.cs` | Cola propia de bytes crudos para el intérprete (acotada, con contador de descartes), independiente de la pausa y del recorte de la vista |
| `Profile.cs` | Claves `WireCatalog` (por defecto `{Repo}\AutoFinalRobo2-Contracts\contracts\wire_catalog.json`), `Addr2line`, `EspElf` |

Sin rutas fijas. El catálogo se lee del perfil o se elige con el botón.

## 7. Loader: selector de fuente

- Opciones: los cinco modos del punto 2.1.
- Al elegir: se manda `#AFR2 TAP n` por el mismo worker de `SerialCapture` (acción explícita del
  usuario, registrada en `.tx.txt` como TX), y se espera `TAP mode=n` durante 1 s.
  - Con confirmación: **limpia el log y la grilla de tramas, conserva el catálogo** y reinicia los
    contadores "visto". Sigue mostrando con hora y origen.
  - Sin confirmación: vuelve a la opción anterior y avisa "el firmware no responde a TAP (¿build sin
    `AFR2_DEBUG_TAP`?)".
- Si aparece `AVR BOOT`, el ATmega volvió a modo log: el Loader lo marca y **no reenvía solo**.
- Cambio de reglas: hoy `AGENTS.md` del Loader dice "No enviar al abrir, reabrir o desde MCP". Se
  mantiene; se agrega que el selector es un envío explícito más, nunca automático ni desde MCP.

---

## 8. Fases

| Fase | Qué | Módulos | Hardware |
|---|---|---|---|
| 1 | `wire_catalog.json` armado desde headers + `commands.json` + `catalog.ts` | Contracts | No |
| 2 | Skill `afr2-wire-commands` y referencias | Contracts, AGENTS raíz | No |
| 3 | Intérprete, separador TAP1, panel de resets con lo que ya imprime el firmware | Loader | No (usa el log actual) |
| 4 | Multiplexor TAP, RX0 de control, redacción, baudios 250000 | ATmega + Loader | Compilar y flashear el ATmega |
| 5 | BT02 + marcas RTC en el ESP, línea `ESP RESET` y `espRst` en el ATmega | ESP + ATmega (EX01 v14) | Compilar y flashear los dos |
| 6 | Opcionales: ROM a 74880 cuando el ATmega resetea al ESP; canal `Serial1`→D17 | ATmega, ESP, hardware | Requiere circuito KEY/RST o cableado nuevo |
| 7 | Ficha, CHANGELOG, README/MANUAL/AGENTS del Loader, versión del Loader, `catalog.ts` sincronizado | Todos | No |

Las fases 1–3 sirven solas con el firmware actual: el panel de resets ya lee las líneas `ESP BOOT` y
`ESP>` del log. Pero **ver y decodificar tramas del enlace requiere la fase 4**: hoy el firmware no
manda ninguna trama por USB. Por eso, para el objetivo de monitor de depuración, la fase 4 es la que
habilita lo principal.

## 9. Decisiones abiertas

1. Modos del selector: ¿los cinco propuestos, o solo ATmega / RX ATmega / RX ESP?
2. Baudios con TAP: 250000 (propuesto) o 500000.
3. BT02: acepta subir EX01 a v14 y actualizar los dos firmwares juntos.
4. Contadores "visto": se reinician al cambiar de fuente (propuesto) o se conservan.
5. Canal de hardware opcional (adaptador 3,3 V o `Serial1`→D17): ¿se evalúa ahora o queda para después?

## 10. Límites conocidos

- El modo trama solo muestra tramas que el parser del ATmega completó. Lo roto se ve en el modo
  crudo (4) o en los contadores de error del parser.
- La hora "ms AVR" es la del ATmega; la del PC incluye la latencia del USB. Solo la primera ordena
  bien las dos direcciones.
- Un reset por alimentación o `hw-watchdog` no deja volcado: ahí BT02 (uptime anterior, etapa, heap)
  es la única pista.
- Nada de este plan acredita hardware probado. Cada fase con firmware registra su estado real
  atmega/esp/web.
