# Catálogo, codecs y herramientas UNER

ID: `web:module/src/features/protocolStudio/payloadCodec.ts` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Representa comandos y traduce bytes/campos para inspección y generación de solicitudes.

UNER, manifest, valores de ejemplo y decoders; el transporte Web de producción es JSON/ESP.

<a id="modularity"></a>
## Modularidad observada

Codecs puros separados de UI y sesión, con esquemas del robot.

<a id="state"></a>
## Handle, estado y atributos externos

Catálogos y buffers pertenecen a la herramienta; ningún payload de ejemplo acredita un efecto físico.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Elegir comando, validar forma de entrada y construir/decodificar representación.
- Conservar endian, ancho, unidades y política de finalización de cada comando.

<a id="interactions"></a>
## Interacciones

- Registro ESP, tabla/handler F4 y manifest pueden divergir; se conservan por separado en documentación.

<a id="limitations"></a>
## Precauciones y límites

- Comando declarado no equivale a implementado.
- No usar tamaños de structs ni bitfields como contrato portable.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/api/UnerFrameV2.ts` | typescript-ast-static | `6b84e29c83e0119e2574544f7d04fac63b08f941ba7d22def9bbcd0ebb1c3ee9` |
| `src/api/UnerProtocol.ts` | typescript-ast-static | `15a374dfdf943a1ede9db9c973235e352567e3cf4f3760b316a7d8f3fb1af2ea` |
| `src/api/UnerProtocolUtils.ts` | typescript-ast-static | `eabd47b0effbed580f2fa8403892eb6cfa63a4137d1b35f48f7ff682936b669a` |
| `src/features/protocolStudio/catalog.ts` | typescript-ast-static | `0d54bc4c8b263ce987e5a64e43bff079ec33b132f3732b7ba84aebdbeff9f674` |
| `src/features/protocolStudio/commandManifest.json` | support-inventory | `67f266678b0efb2499bac9b20e693af114f65d0f50bac2bc0067bf333f77c5e3` |
| `src/features/protocolStudio/payloadCodec.ts` | typescript-ast-static | `70f250306a3a5c414f0511bb1457d5c88b4c3496826b05102010f0ddc4eb6ffb` |
| `src/features/protocolStudio/types.ts` | typescript-ast-static | `104305b3dc37a6960bdacc6654b1ccba7d596e567415034ddd36a9b72e1e91b9` |
| `src/features/protocolStudio/utils.ts` | typescript-ast-static | `fd1cb41012c1c9426d787b3ece3ffb890ea2e257c69a4929c1cd6f808153057f` |
| `src/protocol/f4Payloads.ts` | typescript-ast-static | `17b378754ffb395a5795def1635386c2c1e034814d1d6c8f795e37005e24becc` |
| `src/types/UnerProtocolCMDTypes.ts` | typescript-ast-static | `b78052505448a894222721b13f8de5dd7afd44f8298d4419ffe87623632ee7a4` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `IrSensorKey`

`web:src/api/UnerFrameV2.ts:55`

Tipo: `(typeof IR_SENSOR_ORDER)[number]`.

### `IrSnapshot`

`web:src/api/UnerFrameV2.ts:57`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `flags: number` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `sampleSeq: number` | Dato de estado/configuración `sampleSeq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `periodMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `tickMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `raw: Record<IrSensorKey, number>` | Dato de estado/configuración `raw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `norm: Record<IrSensorKey, number>` | Dato de estado/configuración `norm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `linePattern: number` | Dato de estado/configuración `linePattern`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineAlignment: number` | Dato de estado/configuración `lineAlignment`; declaración estructural, interpretación específica pendiente de ampliar. |
| `confidence: number` | Dato de estado/configuración `confidence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ambiguous: boolean` | Dato de estado/configuración `ambiguous`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineWidthMm: number` | Dato de estado/configuración `lineWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lateralErrorMm: number` | Dato de estado/configuración `lateralErrorMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lateralErrorNorm: number` | Dato de estado/configuración `lateralErrorNorm`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EspRebootMode`

`web:src/api/UnerFrameV2.ts:79`

Tipo: `"normal" \| "ap"`.

### `UnerV2FrameOptions`

`web:src/api/UnerFrameV2.ts:81`

| Atributo / firma | Propósito |
| --- | --- |
| `cmd: number` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `payload?: Uint8Array \| number[]` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `source?: number` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `destination?: number` | Dato de estado/configuración `destination`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UnerV2ParsedFrame`

`web:src/api/UnerFrameV2.ts:88`

| Atributo / firma | Propósito |
| --- | --- |
| `cmd: number` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `payload: Uint8Array` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `payloadLength: number` | Dato de estado/configuración `payloadLength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `route: number` | Dato de estado/configuración `route`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source: number` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `destination: number` | Dato de estado/configuración `destination`; declaración estructural, interpretación específica pendiente de ampliar. |
| `checksum: number` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `offset: number` | Dato de estado/configuración `offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `frame: Uint8Array` | Dato de estado/configuración `frame`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UnerV2StreamParser`

`web:src/api/UnerFrameV2.ts:100`

| Atributo / firma | Propósito |
| --- | --- |
| `push: (chunk: Uint8Array) => UnerV2ParsedFrame[]` | Operación push de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `reset: () => void` | Operación reset de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |

### `UNERPacket`

`web:src/api/UnerProtocol.ts:35`

| Atributo / firma | Propósito |
| --- | --- |
| `cmd: number` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `payload: Uint8Array` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `payloadLen: number` | Dato de estado/configuración `payloadLen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `chk: number` | Dato de estado/configuración `chk`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNERTransport`

`web:src/api/UnerProtocol.ts:42`

| Atributo / firma | Propósito |
| --- | --- |
| `send: (data: Uint8Array) => void` | Operación send de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `onBytes: (handler: (data: Uint8Array) => void) => () => void` | Operación on Bytes de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |

### `UNERStatus`

`web:src/api/UnerProtocol.ts:47`

| Atributo / firma | Propósito |
| --- | --- |
| `OK: inferred-by-TypeScript` | Dato de estado/configuración `OK`; declaración estructural, interpretación específica pendiente de ampliar. |
| `IN_PROGRESS: inferred-by-TypeScript` | Dato de estado/configuración `IN_PROGRESS`; declaración estructural, interpretación específica pendiente de ampliar. |
| `NO_PACKET: inferred-by-TypeScript` | Dato de estado/configuración `NO_PACKET`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_NULL_PTR: inferred-by-TypeScript` | Dato de estado/configuración `ERR_NULL_PTR`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_PAYLOAD_TOO_LONG: inferred-by-TypeScript` | Dato de estado/configuración `ERR_PAYLOAD_TOO_LONG`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_BUFFER_TOO_SMALL: inferred-by-TypeScript` | Dato de estado/configuración `ERR_BUFFER_TOO_SMALL`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_INVALID_LENGTH: inferred-by-TypeScript` | Dato de estado/configuración `ERR_INVALID_LENGTH`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_HEADER_MISMATCH: inferred-by-TypeScript` | Dato de estado/configuración `ERR_HEADER_MISMATCH`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_TOKEN_MISMATCH: inferred-by-TypeScript` | Dato de estado/configuración `ERR_TOKEN_MISMATCH`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_CHECKSUM_MISMATCH: inferred-by-TypeScript` | Dato de estado/configuración `ERR_CHECKSUM_MISMATCH`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_RX_OVERFLOW: inferred-by-TypeScript` | Dato de estado/configuración `ERR_RX_OVERFLOW`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_TX_FAIL: inferred-by-TypeScript` | Dato de estado/configuración `ERR_TX_FAIL`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ERR_ARG: inferred-by-TypeScript` | Dato de estado/configuración `ERR_ARG`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ParserState`

`web:src/api/UnerProtocol.ts:64`

| Atributo / firma | Propósito |
| --- | --- |
| `WAIT_HEADER: inferred-by-TypeScript` | Dato de estado/configuración `WAIT_HEADER`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WAIT_LEN: inferred-by-TypeScript` | Dato de estado/configuración `WAIT_LEN`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WAIT_TOKEN: inferred-by-TypeScript` | Dato de estado/configuración `WAIT_TOKEN`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WAIT_CMD: inferred-by-TypeScript` | Dato de estado/configuración `WAIT_CMD`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WAIT_PAYLOAD: inferred-by-TypeScript` | Dato de estado/configuración `WAIT_PAYLOAD`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WAIT_CHK: inferred-by-TypeScript` | Dato de estado/configuración `WAIT_CHK`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OnPacketFn`

`web:src/api/UnerProtocol.ts:73`

Tipo: `(p: UNERPacket) => void`.

### `HeartbeatSetParams`

`web:src/api/UnerProtocol.ts:76`

| Atributo / firma | Propósito |
| --- | --- |
| `periodMs: U16` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `WifiSetAPParams`

`web:src/api/UnerProtocol.ts:77`

| Atributo / firma | Propósito |
| --- | --- |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `password: string` | Dato de estado/configuración `password`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip: [U8, U8, U8, U8]` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiSetSTAParams`

`web:src/api/UnerProtocol.ts:82`

| Atributo / firma | Propósito |
| --- | --- |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `password: string` | Dato de estado/configuración `password`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fixedIp: boolean` | Dato de estado/configuración `fixedIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip?: [U8, U8, U8, U8]` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TelemetrySetRateParams`

`web:src/api/UnerProtocol.ts:88`

| Atributo / firma | Propósito |
| --- | --- |
| `periodMs: U16` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `AppPinConfigParams`

`web:src/api/UnerProtocol.ts:89`

| Atributo / firma | Propósito |
| --- | --- |
| `action: U8` | Dato de estado/configuración `action`; declaración estructural, interpretación específica pendiente de ampliar. |
| `currentPin: string` | Dato de estado/configuración `currentPin`; declaración estructural, interpretación específica pendiente de ampliar. |
| `newPin?: string` | Dato de estado/configuración `newPin`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppThemeConfigParams`

`web:src/api/UnerProtocol.ts:94`

| Atributo / firma | Propósito |
| --- | --- |
| `base: [U8, U8, U8]` | Dato de estado/configuración `base`; declaración estructural, interpretación específica pendiente de ampliar. |
| `accent: [U8, U8, U8]` | Dato de estado/configuración `accent`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNERProtocol`

`web:src/api/UnerProtocol.ts:100`

| Atributo / firma | Propósito |
| --- | --- |
| `rxRing: Uint8Array` | Dato de estado/configuración `rxRing`; declaración estructural, interpretación específica pendiente de ampliar. |
| `head: inferred-by-TypeScript` | Dato de estado/configuración `head`; declaración estructural, interpretación específica pendiente de ampliar. |
| `tail: inferred-by-TypeScript` | Dato de estado/configuración `tail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `state: ParserState` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `headerProgress: inferred-by-TypeScript` | Dato de estado/configuración `headerProgress`; declaración estructural, interpretación específica pendiente de ampliar. |
| `cksRunning: inferred-by-TypeScript` | Dato de estado/configuración `cksRunning`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lenField: inferred-by-TypeScript` | Dato de estado/configuración `lenField`; declaración estructural, interpretación específica pendiente de ampliar. |
| `expectedPayload: inferred-by-TypeScript` | Dato de estado/configuración `expectedPayload`; declaración estructural, interpretación específica pendiente de ampliar. |
| `cmdByte: inferred-by-TypeScript` | Dato de estado/configuración `cmdByte`; declaración estructural, interpretación específica pendiente de ampliar. |
| `payloadIdx: inferred-by-TypeScript` | Dato de estado/configuración `payloadIdx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `_payloadBuf: Uint8Array` | Dato de estado/configuración `_payloadBuf`; declaración estructural, interpretación específica pendiente de ampliar. |
| `_packet: UNERPacket` | Dato de estado/configuración `_packet`; declaración estructural, interpretación específica pendiente de ampliar. |
| `_packetReady: inferred-by-TypeScript` | Dato de estado/configuración `_packetReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `packetsOK: inferred-by-TypeScript` | Dato de estado/configuración `packetsOK`; declaración estructural, interpretación específica pendiente de ampliar. |
| `errorsChecksum: inferred-by-TypeScript` | Dato de estado/configuración `errorsChecksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `errorsLength: inferred-by-TypeScript` | Dato de estado/configuración `errorsLength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `errorsHeader: inferred-by-TypeScript` | Dato de estado/configuración `errorsHeader`; declaración estructural, interpretación específica pendiente de ampliar. |
| `errorsToken: inferred-by-TypeScript` | Dato de estado/configuración `errorsToken`; declaración estructural, interpretación específica pendiente de ampliar. |
| `errorsOverflow: inferred-by-TypeScript` | Dato de estado/configuración `errorsOverflow`; declaración estructural, interpretación específica pendiente de ampliar. |
| `resetParser(): void` | Operación reset Parser de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `pushByte(b: number): UNERStatus` | Operación push Byte de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `parse(): UNERStatus` | Operación parse de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `packetReady(): boolean` | Operación packet Ready de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `getLastPacket(): UNERPacket` | Operación get Last Packet de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `clearFlag(): void` | Operación clear Flag de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildPacket(cmd: number, payload?: Uint8Array): Uint8Array` | Operación build Packet de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildHeartbeatSet(params: HeartbeatSetParams): Uint8Array` | Operación build Heartbeat Set de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildWifiGetMode(): Uint8Array` | Operación build Wifi Get Mode de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildWifiGetScan(): Uint8Array` | Operación build Wifi Get Scan de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildWifiSetAP(params: WifiSetAPParams): Uint8Array` | Operación build Wifi Set AP de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildWifiSetSTA(params: WifiSetSTAParams): Uint8Array` | Operación build Wifi Set STA de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildTelemetrySetRate(params: TelemetrySetRateParams): Uint8Array` | Operación build Telemetry Set Rate de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildAppPinConfig(params: AppPinConfigParams): Uint8Array` | Operación build App Pin Config de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `buildAppThemeConfig(params: AppThemeConfigParams): Uint8Array` | Operación build App Theme Config de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |
| `calcChecksum(data: Uint8Array): number` | Operación calc Checksum de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. |

### `CommandKind`

`web:src/features/protocolStudio/types.ts:1`

Tipo: `"request" \| "mixed" \| "event" \| "ack"`.

### `FieldType`

`web:src/features/protocolStudio/types.ts:2`

Tipo: `"select" \| "str" \| "u8" \| "hex1" \| "hex4" \| "u16" \| "optionalU8" \| "optionalU16" \| "f32" \| "bytes" \| "digits"`.

### `ValidationTone`

`web:src/features/protocolStudio/types.ts:3`

Tipo: `"ok" \| "warn" \| "bad"`.

### `OverallState`

`web:src/features/protocolStudio/types.ts:4`

Tipo: `"ok" \| "warn" \| "bad"`.

### `TranslatorViewMode`

`web:src/features/protocolStudio/types.ts:5`

Tipo: `"idle" \| "singleFrame" \| "singleCommand" \| "block" \| "error"`.

### `CommandField`

`web:src/features/protocolStudio/types.ts:7`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `type: FieldType` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `placeholder?: string` | Dato de estado/configuración `placeholder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `options?: Array<{ value: string; label: string }>` | Dato de estado/configuración `options`; declaración estructural, interpretación específica pendiente de ampliar. |

### `CommandDefinition`

`web:src/features/protocolStudio/types.ts:15`

| Atributo / firma | Propósito |
| --- | --- |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `desc: string` | Dato de estado/configuración `desc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fields: CommandField[]` | Dato de estado/configuración `fields`; declaración estructural, interpretación específica pendiente de ampliar. |
| `minPayload: number` | Dato de estado/configuración `minPayload`; declaración estructural, interpretación específica pendiente de ampliar. |
| `maxPayload: number` | Dato de estado/configuración `maxPayload`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kind: CommandKind` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `owner?: string` | Dato de estado/configuración `owner`; declaración estructural, interpretación específica pendiente de ampliar. |
| `implemented?: boolean` | Dato de estado/configuración `implemented`; declaración estructural, interpretación específica pendiente de ampliar. |
| `responseMin?: number` | Dato de estado/configuración `responseMin`; declaración estructural, interpretación específica pendiente de ampliar. |
| `responseMax?: number` | Dato de estado/configuración `responseMax`; declaración estructural, interpretación específica pendiente de ampliar. |
| `completion?: string` | Dato de estado/configuración `completion`; declaración estructural, interpretación específica pendiente de ampliar. |

### `CommandGroup`

`web:src/features/protocolStudio/types.ts:29`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `commands: string[]` | Dato de estado/configuración `commands`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ValidationItem`

`web:src/features/protocolStudio/types.ts:34`

| Atributo / firma | Propósito |
| --- | --- |
| `tone: ValidationTone` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `message: string` | Dato de estado/configuración `message`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TranslationResult`

`web:src/features/protocolStudio/types.ts:39`

| Atributo / firma | Propósito |
| --- | --- |
| `overall: OverallState` | Dato de estado/configuración `overall`; declaración estructural, interpretación específica pendiente de ampliar. |
| `summary: string` | Dato de estado/configuración `summary`; declaración estructural, interpretación específica pendiente de ampliar. |
| `typeDetected: string` | Dato de estado/configuración `typeDetected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `cmdHex: string` | Dato de estado/configuración `cmdHex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `meaning: string` | Dato de estado/configuración `meaning`; declaración estructural, interpretación específica pendiente de ampliar. |
| `route: string` | Dato de estado/configuración `route`; declaración estructural, interpretación específica pendiente de ampliar. |
| `nodes: string` | Dato de estado/configuración `nodes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `len: string` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `payload: string` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `validations: ValidationItem[]` | Dato de estado/configuración `validations`; declaración estructural, interpretación específica pendiente de ampliar. |
| `frameBytes: number[]` | Dato de estado/configuración `frameBytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `hasFrame: boolean` | Dato de estado/configuración `hasFrame`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BlockGap`

`web:src/features/protocolStudio/types.ts:55`

| Atributo / firma | Propósito |
| --- | --- |
| `start: number` | Dato de estado/configuración `start`; declaración estructural, interpretación específica pendiente de ampliar. |
| `end: number` | Dato de estado/configuración `end`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bytes: number[]` | Dato de estado/configuración `bytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kind: "out_of_frame" \| "boot_reset"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `note: string` | Dato de estado/configuración `note`; declaración estructural, interpretación específica pendiente de ampliar. |

### `InvalidCandidate`

`web:src/features/protocolStudio/types.ts:64`

| Atributo / firma | Propósito |
| --- | --- |
| `offset: number` | Dato de estado/configuración `offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reason: string` | Dato de estado/configuración `reason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `preview: number[]` | Memoria de la observación anterior usada por la máquina de estados. |

### `ValidFrameItem`

`web:src/features/protocolStudio/types.ts:70`

| Atributo / firma | Propósito |
| --- | --- |
| `offset: number` | Dato de estado/configuración `offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `frame: number[]` | Dato de estado/configuración `frame`; declaración estructural, interpretación específica pendiente de ampliar. |
| `analysis: TranslationResult` | Dato de estado/configuración `analysis`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ScanBlockResult`

`web:src/features/protocolStudio/types.ts:76`

| Atributo / firma | Propósito |
| --- | --- |
| `sourceLabel: string` | Dato de estado/configuración `sourceLabel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `totalBytes: number` | Dato de estado/configuración `totalBytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `validFrames: ValidFrameItem[]` | Dato de estado/configuración `validFrames`; declaración estructural, interpretación específica pendiente de ampliar. |
| `invalidCandidates: InvalidCandidate[]` | Dato de estado/configuración `invalidCandidates`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gaps: BlockGap[]` | Dato de estado/configuración `gaps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `knownCount: number` | Dato de estado/configuración `knownCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `unknownCount: number` | Dato de estado/configuración `unknownCount`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DetectInputResult`

`web:src/features/protocolStudio/types.ts:86`

Tipo: `\| { mode: "frame"; bytes: number[]; sourceLabel: string; forceBlockScan: boolean; } \| { mode: "cmd"; cmd: number; sourceLabel: string; }`.

### `BuilderData`

`web:src/features/protocolStudio/types.ts:99`

| Atributo / firma | Propósito |
| --- | --- |
| `frame: number[]` | Dato de estado/configuración `frame`; declaración estructural, interpretación específica pendiente de ampliar. |
| `payload: number[]` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `cmd: number` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `route: number` | Dato de estado/configuración `route`; declaración estructural, interpretación específica pendiente de ampliar. |
| `chk: number` | Dato de estado/configuración `chk`; declaración estructural, interpretación específica pendiente de ampliar. |
| `len: number` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Vector3`

`web:src/protocol/f4Payloads.ts:16`

| Atributo / firma | Propósito |
| --- | --- |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `z: number` | Dato de estado/configuración `z`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MpuSnapshot`

`web:src/protocol/f4Payloads.ts:18`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `flags: number` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `sampleValid: boolean` | Dato de estado/configuración `sampleValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `calibrated: boolean` | Dato de estado/configuración `calibrated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `magValid: boolean` | Dato de estado/configuración `magValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stationary: boolean` | Dato de estado/configuración `stationary`; declaración estructural, interpretación específica pendiente de ampliar. |
| `calibrating: boolean` | Dato de estado/configuración `calibrating`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sensorError: boolean` | Dato de estado/configuración `sensorError`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sampleSeq: number` | Dato de estado/configuración `sampleSeq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sampleDtUs: number` | Dato de estado/configuración `sampleDtUs`; declaración estructural, interpretación específica pendiente de ampliar. |
| `eulerDeg: Vector3 & { roll: number; pitch: number; yaw: number }` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `accelG: Vector3` | Dato de estado/configuración `accelG`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gyroDps: Vector3` | Dato de estado/configuración `gyroDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `valid: boolean` | Dato de estado/configuración `valid`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrSensorKey`

`web:src/protocol/f4Payloads.ts:46`

Tipo: `(typeof IR_SENSOR_ORDER)[number]`.

### `IrSnapshot`

`web:src/protocol/f4Payloads.ts:48`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `flags: number` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `sampleSeq: number` | Dato de estado/configuración `sampleSeq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `periodMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `tickMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `raw: Record<IrSensorKey, number>` | Dato de estado/configuración `raw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `norm: Record<IrSensorKey, number>` | Dato de estado/configuración `norm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `linePattern: number` | Dato de estado/configuración `linePattern`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineAlignment: number` | Dato de estado/configuración `lineAlignment`; declaración estructural, interpretación específica pendiente de ampliar. |
| `confidence: number` | Dato de estado/configuración `confidence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ambiguous: boolean` | Dato de estado/configuración `ambiguous`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineWidthMm: number` | Dato de estado/configuración `lineWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lateralErrorMm: number` | Dato de estado/configuración `lateralErrorMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lateralErrorNorm: number` | Dato de estado/configuración `lateralErrorNorm`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StreamSetAck`

`web:src/protocol/f4Payloads.ts:65`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `active: boolean` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `periodMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `StreamStopAck`

`web:src/protocol/f4Payloads.ts:71`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |

### `TelemetrySnapshot`

`web:src/protocol/f4Payloads.ts:75`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `backend: number` | Dato de estado/configuración `backend`; declaración estructural, interpretación específica pendiente de ampliar. |
| `wifiMode: number` | Dato de estado/configuración `wifiMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bridgeConnected: boolean` | Dato de estado/configuración `bridgeConnected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `capabilityMask: number` | Dato de estado/configuración `capabilityMask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `i2cPolicy: number` | Dato de estado/configuración `i2cPolicy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `motionArmed: boolean` | Dato de estado/configuración `motionArmed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `balanceActive: boolean` | Dato de estado/configuración `balanceActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `commandActive: boolean` | Dato de estado/configuración `commandActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `safetyStop: boolean` | Dato de estado/configuración `safetyStop`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pitchMeasuredDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `pitchSetpointDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `balanceOutputPct: number` | Dato de estado/configuración `balanceOutputPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `leftOutputPct: number` | Dato de estado/configuración `leftOutputPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rightOutputPct: number` | Dato de estado/configuración `rightOutputPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kp: number` | Dato de estado/configuración `kp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ki: number` | Dato de estado/configuración `ki`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kd: number` | Dato de estado/configuración `kd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rollDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `pitchDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `yawMagneticDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `mpuStreamActive: boolean` | Dato de estado/configuración `mpuStreamActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `mpuStreamPeriodMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `leftMinPwm: number` | Dato de estado/configuración `leftMinPwm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rightMinPwm: number` | Dato de estado/configuración `rightMinPwm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rawAdc: number[]` | Dato de estado/configuración `rawAdc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pidErrorDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `pidIntegralDegS: number` | Dato de estado/configuración `pidIntegralDegS`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pidDerivativeDegS: number` | Dato de estado/configuración `pidDerivativeDegS`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4BootHandoff`

`web:src/protocol/f4Payloads.ts:114`

Tipo: `0 \| 1 \| 2 \| 3`.

### `F4ExtensionProfileId`

`web:src/protocol/f4Payloads.ts:115`

Tipo: `0 \| 1`.

### `F4BootReport`

`web:src/protocol/f4Payloads.ts:117`

| Atributo / firma | Propósito |
| --- | --- |
| `schema: number` | Dato de estado/configuración `schema`; declaración estructural, interpretación específica pendiente de ampliar. |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `mailboxValid: boolean` | Dato de estado/configuración `mailboxValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `handoff: F4BootHandoff` | Dato de estado/configuración `handoff`; declaración estructural, interpretación específica pendiente de ampliar. |
| `resetFlags: number` | Dato de estado/configuración `resetFlags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `appValidationFlags: number` | Dato de estado/configuración `appValidationFlags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `extensionProfileId: F4ExtensionProfileId` | Dato de estado/configuración `extensionProfileId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bootloaderVersion: number` | Dato de estado/configuración `bootloaderVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `appVersion: number` | Dato de estado/configuración `appVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `appSize: number` | Dato de estado/configuración `appSize`; declaración estructural, interpretación específica pendiente de ampliar. |
| `appCrc32: number` | Dato de estado/configuración `appCrc32`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ControlSnapshot`

`web:src/protocol/f4Payloads.ts:131`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `carMode: number` | Dato de estado/configuración `carMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineFsm: number` | Dato de estado/configuración `lineFsm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `linePattern: number` | Dato de estado/configuración `linePattern`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineConfidencePct: number` | Dato de estado/configuración `lineConfidencePct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineAmbiguous: boolean` | Dato de estado/configuración `lineAmbiguous`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineRejectReason: number` | Dato de estado/configuración `lineRejectReason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineWidthMm: number` | Dato de estado/configuración `lineWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lateralErrorMm: number` | Dato de estado/configuración `lateralErrorMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lateralErrorNorm: number` | Dato de estado/configuración `lateralErrorNorm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `linearAccelerationMps2: number` | Dato de estado/configuración `linearAccelerationMps2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `estimatedVelocityMps: number` | Dato de estado/configuración `estimatedVelocityMps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `yawRateDps: number` | Dato de estado/configuración `yawRateDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `estimatorConfidencePct: number` | Dato de estado/configuración `estimatorConfidencePct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `estimatorFlags: number` | Dato de estado/configuración `estimatorFlags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `commonOutputPct: number` | Dato de estado/configuración `commonOutputPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `differentialOutputPct: number` | Dato de estado/configuración `differentialOutputPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `turnLimitPct: number` | Dato de estado/configuración `turnLimitPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `saturationLimited: boolean` | Dato de estado/configuración `saturationLimited`; declaración estructural, interpretación específica pendiente de ampliar. |

### `U8`

`web:src/types/UnerProtocolCMDTypes.ts:176`

Tipo: `number`.

### `U16`

`web:src/types/UnerProtocolCMDTypes.ts:177`

Tipo: `number`.

### `I8`

`web:src/types/UnerProtocolCMDTypes.ts:178`

Tipo: `number`.

### `I16`

`web:src/types/UnerProtocolCMDTypes.ts:179`

Tipo: `number`.

### `APCreds`

`web:src/types/UnerProtocolCMDTypes.ts:236`

| Atributo / firma | Propósito |
| --- | --- |
| `ssidLen: U8` | Dato de estado/configuración `ssidLen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssid: Uint8Array` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `passLen: U8` | Dato de estado/configuración `passLen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pass: Uint8Array` | Dato de estado/configuración `pass`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip: Uint8Array` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |

### `STACreds`

`web:src/types/UnerProtocolCMDTypes.ts:244`

| Atributo / firma | Propósito |
| --- | --- |
| `ssidLen: U8` | Dato de estado/configuración `ssidLen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssid: Uint8Array` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `passLen: U8` | Dato de estado/configuración `passLen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pass: Uint8Array` | Dato de estado/configuración `pass`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fixedIp: U8` | Dato de estado/configuración `fixedIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip: Uint8Array` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WiFiNetwork`

`web:src/types/UnerProtocolCMDTypes.ts:253`

| Atributo / firma | Propósito |
| --- | --- |
| `ssidLen: U8` | Dato de estado/configuración `ssidLen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rssi_dBm: I8` | Dato de estado/configuración `rssi_dBm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `security: U8` | Dato de estado/configuración `security`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WiFiNetworkDetail`

`web:src/types/UnerProtocolCMDTypes.ts:260`

| Atributo / firma | Propósito |
| --- | --- |
| `ssidLen: U8` | Dato de estado/configuración `ssidLen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `signalStrength: I8` | Dato de estado/configuración `signalStrength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `encryptionType: U8` | Dato de estado/configuración `encryptionType`; declaración estructural, interpretación específica pendiente de ampliar. |
| `channel: U8` | Dato de estado/configuración `channel`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TelemetryDataV1`

`web:src/types/UnerProtocolCMDTypes.ts:268`

| Atributo / firma | Propósito |
| --- | --- |
| `schema: U8` | Dato de estado/configuración `schema`; declaración estructural, interpretación específica pendiente de ampliar. |
| `seq: U16` | Dato de estado/configuración `seq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `accX: I16` | Dato de estado/configuración `accX`; declaración estructural, interpretación específica pendiente de ampliar. |
| `accY: I16` | Dato de estado/configuración `accY`; declaración estructural, interpretación específica pendiente de ampliar. |
| `accZ: I16` | Dato de estado/configuración `accZ`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gyroX: I16` | Dato de estado/configuración `gyroX`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gyroY: I16` | Dato de estado/configuración `gyroY`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gyroZ: I16` | Dato de estado/configuración `gyroZ`; declaración estructural, interpretación específica pendiente de ampliar. |
| `tempRaw: I16` | Dato de estado/configuración `tempRaw`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-analyzecommandonly"></a>`analyzeCommandOnly(cmd: number, sourceLabel: string): TranslationResult` | Coordina `badLine`, `normalizeCmdKey`, `okLine` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:403`; structural |
| <a id="fn-analyzeframe"></a>`analyzeFrame(frame: number[], sourceLabel: string): TranslationResult` | Coordina `String.fromCharCode`, `["0x15", "0x48", "0x4B"].includes`, `badLine` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:482`; structural |
| <a id="fn-apppinconfig"></a>`appPinConfig(action: U8, currentPin: string, newPin?: string): Uint8Array` | Coordina `buf.set`, `encodePin4` para Catálogo, codecs y herramientas UNER. | `web:src/types/UnerProtocolCMDTypes.ts:126`; structural |
| <a id="fn-appthemeconfig"></a>`appThemeConfig(base: [U8, U8, U8], accent: [U8, U8, U8]): Uint8Array` | Operación app Theme Config de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/types/UnerProtocolCMDTypes.ts:164`; inferred-name |
| <a id="fn-asciipreview"></a>`asciiPreview(bytes: number[], maxLength: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `bytes .map`, `bytes .map((byte) => { if (byte === 0x0d) { return "\\r"; } if (byte === 0x0a) { return "\\n"; } if (byte >= 32 && byte <= 126) { return String.fromCharCode(byte); } return "."; }) .join`, `preview.slice` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:92`; structural |
| <a id="fn-asciisearchtext"></a>`asciiSearchText(bytes: number[]): inferred-by-TypeScript` | Coordina `bytes .map`, `bytes .map((byte) => { if (byte === 0x0d) { return "\r"; } if (byte === 0x0a) { return "\n"; } if (byte >= 32 && byte <= 126) { return String.fromCharCode(byte); } return " "; }) .join` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:111`; structural |
| <a id="fn-badline"></a>`badLine(message: string): ValidationItem` | Operación bad Line de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/utils.ts:291`; inferred-name |
| <a id="fn-builddisablempustreamframe"></a>`buildDisableMpuStreamFrame(): Uint8Array` | Adapta la llamada a `buildUnerV2Frame` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:190`; structural |
| <a id="fn-buildesprebootrequestframe"></a>`buildEspRebootRequestFrame(mode: EspRebootMode): Uint8Array` | Adapta la llamada a `buildUnerV2Frame` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:132`; structural |
| <a id="fn-buildformpayload"></a>`buildFormPayload(command: CommandDefinition \| undefined, values: Record<string, string>): inferred-by-TypeScript` | Adapta la llamada a `encodeCommandFields` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:156`; structural |
| <a id="fn-buildframedata"></a>`buildFrameData(source: string, destination: string, commandKey: string, manualPayload: string, fieldValues: Record<string, string>): BuilderData` | Coordina `/^0x[0-9a-f]{2}$/i.test`, `/^\d+$/.test`, `Number.parseInt` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:160`; structural |
| <a id="fn-buildgetcarmodeframe"></a>`buildGetCarModeFrame(): Uint8Array` | Adapta la llamada a `buildUnerV2Frame` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:141`; structural |
| <a id="fn-buildgetirsnapshotframe"></a>`buildGetIrSnapshotFrame(): Uint8Array` | Adapta la llamada a `buildUnerV2Frame` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:163`; structural |
| <a id="fn-buildsetirstreamframe"></a>`buildSetIrStreamFrame(periodMs: number, enable: inferred-by-TypeScript): Uint8Array` | Coordina `buildUnerV2Frame`, `normalizeIrStreamPeriodMs` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerFrameV2.ts:167`; structural |
| <a id="fn-buildsetmpustreamframe"></a>`buildSetMpuStreamFrame(periodMs: number): Uint8Array` | Coordina `Math.max`, `Math.min`, `Math.round` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerFrameV2.ts:145`; structural |
| <a id="fn-buildstopirstreamframe"></a>`buildStopIrStreamFrame(): Uint8Array` | Adapta la llamada a `buildUnerV2Frame` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:186`; structural |
| <a id="fn-buildstopmpustreamframe"></a>`buildStopMpuStreamFrame(): Uint8Array` | Adapta la llamada a `buildUnerV2Frame` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:159`; structural |
| <a id="fn-buildsummary"></a>`buildSummary(params: { overall: OverallState; definition?: CommandDefinition; key: string; frameRole: string; sourceLabel: string; payload: number[]; src: number \| null; dst: number \| null; }): inferred-by-TypeScript` | Coordina `formatNetworkIpPayload`, `frameRole.toLowerCase`, `isNetworkIpDefinition` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:444`; structural |
| <a id="fn-buildunerv2frame"></a>`buildUnerV2Frame({   cmd,   payload = [],   source = UNER_V2_NODE.WEB_APP,   destination = UNER_V2_NODE.MCU, }: UnerV2FrameOptions): Uint8Array` | Coordina `Array.from`, `xorChecksum` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerFrameV2.ts:105`; structural |
| <a id="fn-bytestohex"></a>`bytesToHex(bytes: number[]): inferred-by-TypeScript` | Coordina `bytes.map`, `bytes.map(h2).join` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:25`; structural |
| <a id="fn-bytestorealterm"></a>`bytesToRealTerm(bytes: number[]): inferred-by-TypeScript` | Coordina `bytes.map`, `bytes.map(hx).join` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:29`; structural |
| <a id="fn-carmodelabel"></a>`carModeLabel(value: number): inferred-by-TypeScript` | Operación car Mode Label de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/utils.ts:692`; inferred-name |
| <a id="fn-classifygap"></a>`classifyGap(bytes: number[]): inferred-by-TypeScript` | Coordina `asciiSearchText`, `bootMarkers.some` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:324`; structural |
| <a id="fn-convertacceleration"></a>`convertAcceleration(raw: I16): number` | Operación convert Acceleration de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/types/UnerProtocolCMDTypes.ts:293`; inferred-name |
| <a id="fn-convertgyroscope"></a>`convertGyroscope(raw: I16): number` | Operación convert Gyroscope de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/types/UnerProtocolCMDTypes.ts:295`; inferred-name |
| <a id="fn-converttemperature"></a>`convertTemperature(raw: I16): number` | Operación convert Temperature de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/types/UnerProtocolCMDTypes.ts:297`; inferred-name |
| <a id="fn-countheaders"></a>`countHeaders(bytes: number[]): inferred-by-TypeScript` | Operación count Headers de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/utils.ts:209`; inferred-name |
| <a id="fn-createdefaultfieldvalues"></a>`createDefaultFieldValues(command: CommandDefinition \| undefined): inferred-by-TypeScript` | Adapta la llamada a `defaultValueForField` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:143`; structural |
| <a id="fn-createemptyblocktranslation"></a>`createEmptyBlockTranslation(): TranslationResult` | Adapta la llamada a `warnLine` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:385`; structural |
| <a id="fn-createerrortranslation"></a>`createErrorTranslation(message: string): TranslationResult` | Adapta la llamada a `badLine` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:367`; structural |
| <a id="fn-createidletranslation"></a>`createIdleTranslation(): TranslationResult` | Operación create Idle Translation de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/utils.ts:349`; inferred-name |
| <a id="fn-createunerv2streamparser"></a>`createUnerV2StreamParser(maxBufferedBytes: inferred-by-TypeScript): UnerV2StreamParser` | Coordina `Math.max`, `Uint8Array.from`, `buffer.push` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerFrameV2.ts:295`; structural |
| <a id="fn-decodebootreport"></a>`decodeBootReport(payload: Uint8Array): F4BootReport` | Coordina `exactView`, `validateBootReport`, `view.getUint32` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:256`; structural |
| <a id="fn-decodecontrolsnapshot"></a>`decodeControlSnapshot(payload: Uint8Array): ControlSnapshot` | Coordina `[9, 13, 17, 21, 25, 31, 35, 39].map`, `ensureFinite`, `exactView` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:240`; structural |
| <a id="fn-decodeirsnapshot"></a>`decodeIrSnapshot(payload: Uint8Array): IrSnapshot` | Coordina `ensureFinite`, `exactView`, `readSensorArray` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:181`; structural |
| <a id="fn-decodempusnapshot"></a>`decodeMpuSnapshot(payload: Uint8Array): MpuSnapshot` | Coordina `[6, 10, 14, 18, 22, 26, 30, 34, 38].map`, `ensureFinite`, `exactView` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:153`; structural |
| <a id="fn-decodestreamsetack"></a>`decodeStreamSetAck(payload: Uint8Array): StreamSetAck` | Coordina `exactView`, `view.getUint16`, `view.getUint8` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:207`; structural |
| <a id="fn-decodestreamstopack"></a>`decodeStreamStopAck(payload: Uint8Array): StreamStopAck` | Coordina `exactView`, `exactView(payload, 1, "STOP stream ACK").getUint8` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:216`; structural |
| <a id="fn-decodestudiopayload"></a>`decodeStudioPayload(id: string, payload: number[], response: boolean): string \| null` | Coordina `JSON.stringify`, `Uint8Array.from`, `["0x52", "0x95", "0x96"].includes` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/payloadCodec.ts:98`; structural |
| <a id="fn-decodetelemetrysnapshot"></a>`decodeTelemetrySnapshot(payload: Uint8Array): TelemetrySnapshot` | Coordina `Array.from`, `[13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 80, 84, 88].map`, `ensureFinite` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:220`; structural |
| <a id="fn-defaultvalueforfield"></a>`defaultValueForField(field: CommandField): inferred-by-TypeScript` | Operación default Value For Field de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/utils.ts:133`; inferred-name |
| <a id="fn-describenetworkinterface"></a>`describeNetworkInterface(value: number): inferred-by-TypeScript` | Adapta la llamada a `hx` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:295`; structural |
| <a id="fn-detectinput"></a>`detectInput(rawInput: string): DetectInputResult` | Coordina `/^(0x)?[0-9a-fA-F]{1,2}$/.test`, `Number.parseInt`, `Object.entries` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:237`; structural |
| <a id="fn-encode"></a>`encode(field: CommandField, raw: string): number[]` | Coordina `/^\d{4}$/.test`, `Number`, `Number.isFinite` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/payloadCodec.ts:21`; structural |
| <a id="fn-encodecommandfields"></a>`encodeCommandFields(command: CommandDefinition, values: Record<string, string>): number[]` | Coordina `["kp", "ki", "kd"].filter`, `command.fields.flatMap`, `emit` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/payloadCodec.ts:50`; structural |
| <a id="fn-encodepin4"></a>`encodePin4(pin: string): Uint8Array` | Coordina `/^\d{4}$/.test`, `new TextEncoder().encode` para Catálogo, codecs y herramientas UNER. | `web:src/types/UnerProtocolCMDTypes.ts:168`; structural |
| <a id="fn-ensurefinite"></a>`ensureFinite(values: number[], label: string): void` | Adapta la llamada a `values.some` con el contexto de esta integración. | `web:src/protocol/f4Payloads.ts:404`; structural |
| <a id="fn-exactview"></a>`exactView(payload: Uint8Array, length: number, label: string): DataView` | Operación exact View de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/protocol/f4Payloads.ts:399`; inferred-name |
| <a id="fn-extracthextokens"></a>`extractHexTokens(input: string): inferred-by-TypeScript` | Adapta la llamada a `input.match` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:87`; structural |
| <a id="fn-findheaderindex"></a>`findHeaderIndex(bytes: number[]): number` | Adapta la llamada a `UNER_V2.HEADER.every` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:391`; structural |
| <a id="fn-formatf4boothandoff"></a>`formatF4BootHandoff(handoff: F4BootHandoff): string` | Operación format F4 Boot Handoff de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/protocol/f4Payloads.ts:309`; inferred-name |
| <a id="fn-formatf4extensionprofile"></a>`formatF4ExtensionProfile(profile: F4ExtensionProfileId): string` | Operación format F4 Extension Profile de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/protocol/f4Payloads.ts:313`; inferred-name |
| <a id="fn-formatf4packedversion"></a>`formatF4PackedVersion(version: number): string` | Operación format F4 Packed Version de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/protocol/f4Payloads.ts:304`; inferred-name |
| <a id="fn-formatnetworkippayload"></a>`formatNetworkIpPayload(payload: number[]): inferred-by-TypeScript` | Coordina `bytesToHex`, `describeNetworkInterface` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:309`; structural |
| <a id="fn-formatunerframehex"></a>`formatUnerFrameHex(frame: Uint8Array): string` | Coordina `Array.from`, `Array.from(frame, (byte) => `00${byte.toString(16).toUpperCase()}`.slice(-2)).join` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerFrameV2.ts:239`; structural |
| <a id="fn-h2"></a>`h2(value: number): inferred-by-TypeScript` | Coordina `(value & 0xff).toString`, `(value & 0xff).toString(16).toUpperCase`, ``00${(value & 0xff).toString(16).toUpperCase()}`.slice` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:17`; structural |
| <a id="fn-hasheaderat"></a>`hasHeaderAt(bytes: Uint8Array, offset: number): boolean` | Adapta la llamada a `UNER_V2.HEADER.every` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:387`; structural |
| <a id="fn-heartbeatset"></a>`heartbeatSet(periodMs: U16): Uint8Array` | Operación heartbeat Set de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/types/UnerProtocolCMDTypes.ts:49`; inferred-name |
| <a id="fn-hx"></a>`hx(value: number): inferred-by-TypeScript` | Adapta la llamada a `h2` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:21`; structural |
| <a id="fn-integer"></a>`integer(raw: string, maximum: number, hex: inferred-by-TypeScript): number` | Coordina `(hex ? /^[0-9a-f]+$/i : /^\d+$/).test`, `Number.isSafeInteger`, `Number.parseInt` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/payloadCodec.ts:11`; structural |
| <a id="fn-interpretloosearray"></a>`interpretLooseArray(bytes: number[], sourceLabel: string): DetectInputResult` | Coordina `bytes.some`, `countHeaders` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:219`; structural |
| <a id="fn-isnetworkipdefinition"></a>`isNetworkIpDefinition(definition?: CommandDefinition): inferred-by-TypeScript` | Adapta la llamada a `["BOOT_COMPLETE", "NETWORK_IP", "EVT_NETWORK_IP", "EVT_BOOT_COMPLETE"].includes` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:305`; structural |
| <a id="fn-isprintableascii"></a>`isPrintableAscii(bytes: number[]): inferred-by-TypeScript` | Adapta la llamada a `bytes.every` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:45`; structural |
| <a id="fn-le"></a>`le(value: number, width: number): number[]` | Adapta la llamada a `Array.from` con el contexto de esta integración. | `web:src/features/protocolStudio/payloadCodec.ts:18`; structural |
| <a id="fn-le16"></a>`le16(n: number): Uint8Array` | Operación le16 de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/api/UnerProtocolUtils.ts:2`; inferred-name |
| <a id="fn-nodename"></a>`nodeName(node: number): inferred-by-TypeScript` | Coordina `node.toString`, `node.toString(16).toUpperCase` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:37`; structural |
| <a id="fn-normalizecmdkey"></a>`normalizeCmdKey(cmd: number): inferred-by-TypeScript` | Adapta la llamada a `h2` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:41`; structural |
| <a id="fn-normalizeirstreamperiodms"></a>`normalizeIrStreamPeriodMs(value: number): number` | Coordina `Math.max`, `Math.min`, `Math.round` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerFrameV2.ts:197`; structural |
| <a id="fn-normalizestreamperiodms"></a>`normalizeStreamPeriodMs(value: number): number` | Coordina `Math.max`, `Math.min`, `Math.round` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:344`; structural |
| <a id="fn-number"></a>`number(value: unknown, label: string): number` | Adapta la llamada a `Number.isFinite` con el contexto de esta integración. | `web:src/protocol/f4Payloads.ts:420`; structural |
| <a id="fn-numericarray"></a>`numericArray(value: unknown, length: number, label: string): number[]` | Coordina `Array.isArray`, `value.some` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:425`; structural |
| <a id="fn-okline"></a>`okLine(message: string): ValidationItem` | Operación ok Line de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/utils.ts:283`; inferred-name |
| <a id="fn-parsebootreportdata"></a>`parseBootReportData(value: unknown): F4BootReport` | Coordina `Array.isArray`, `Boolean`, `Uint8Array.from` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:275`; structural |
| <a id="fn-parsehexstring"></a>`parseHexString(input: string): inferred-by-TypeScript` | Coordina `input .trim`, `input .trim() .split`, `input .trim() .split(/\s+/) .filter` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:53`; structural |
| <a id="fn-parseirdata"></a>`parseIrData(value: unknown): IrSnapshot` | Coordina `Boolean`, `IR_SENSOR_ORDER.map`, `Object.fromEntries` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:380`; structural |
| <a id="fn-parselooselist"></a>`parseLooseList(input: string): inferred-by-TypeScript` | Coordina `input .split`, `input .split(/[,\s]+/) .map`, `input .split(/[,\s]+/) .map((chunk) => chunk.trim()) .filter` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:68`; structural |
| <a id="fn-parsempudata"></a>`parseMpuData(value: unknown): MpuSnapshot` | Coordina `flagValue`, `number`, `record` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:349`; structural |
| <a id="fn-parseunerv2frames"></a>`parseUnerV2Frames(bytes: Uint8Array): UnerV2ParsedFrame[]` | Coordina `Array.from`, `bytes.slice`, `frames.push` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerFrameV2.ts:243`; structural |
| <a id="fn-readirsensorarray"></a>`readIrSensorArray(view: DataView, offset: number): Record<IrSensorKey, number>` | Adapta la llamada a `IR_SENSOR_ORDER.reduce` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:407`; structural |
| <a id="fn-readle16"></a>`readLe16(buf: Uint8Array, off: inferred-by-TypeScript): number` | Operación read Le16 de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/api/UnerProtocolUtils.ts:7`; inferred-name |
| <a id="fn-readsensorarray"></a>`readSensorArray(view: DataView, offset: number): Record<IrSensorKey, number>` | Adapta la llamada a `IR_SENSOR_ORDER.reduce` con el contexto de esta integración. | `web:src/protocol/f4Payloads.ts:408`; structural |
| <a id="fn-record"></a>`record(value: unknown, label: string): Record<string, unknown>` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/protocol/f4Payloads.ts:415`; structural |
| <a id="fn-scanblock"></a>`scanBlock(bytes: number[], sourceLabel: string): ScanBlockResult` | Coordina `Math.min`, `analyzeFrame`, `bytes.slice` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:700`; structural |
| <a id="fn-shorthex"></a>`shortHex(bytes: number[], maxBytes: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `bytes.slice`, `bytesToHex` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:128`; structural |
| <a id="fn-strtobytes"></a>`strToBytes(input: string): inferred-by-TypeScript` | Coordina `Array.from`, `new TextEncoder().encode` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/utils.ts:49`; structural |
| <a id="fn-stricthexbytes"></a>`strictHexBytes(value: string): number[]` | Coordina `value.trim`, `value.trim().split`, `value.trim().split(/\s+/).map` para Catálogo, codecs y herramientas UNER. | `web:src/features/protocolStudio/payloadCodec.ts:4`; structural |
| <a id="fn-telemetrysetrate"></a>`telemetrySetRate(periodMs: U16): Uint8Array` | Operación telemetry Set Rate de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/types/UnerProtocolCMDTypes.ts:119`; inferred-name |
| <a id="fn-validatebootreport"></a>`validateBootReport(report: F4BootReport): void` | Coordina `Number.isInteger`, `Object.entries` para Catálogo, codecs y herramientas UNER. | `web:src/protocol/f4Payloads.ts:317`; structural |
| <a id="fn-warnline"></a>`warnLine(message: string): ValidationItem` | Operación warn Line de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/utils.ts:287`; inferred-name |
| <a id="fn-wifigetdetail"></a>`wifiGetDetail(ssid: string): Uint8Array` | Coordina `buf.set`, `new TextEncoder().encode` para Catálogo, codecs y herramientas UNER. | `web:src/types/UnerProtocolCMDTypes.ts:110`; structural |
| <a id="fn-wifisetap"></a>`wifiSetAP(ssid: string, password: string, ip: [U8, U8, U8, U8]): Uint8Array` | Coordina `buf.set`, `new TextEncoder().encode` para Catálogo, codecs y herramientas UNER. | `web:src/types/UnerProtocolCMDTypes.ts:57`; structural |
| <a id="fn-wifisetsta"></a>`wifiSetSTA(ssid: string, password: string, fixedIp: boolean, ip?: [U8, U8, U8, U8]): Uint8Array` | Coordina `buf.set`, `new TextEncoder().encode` para Catálogo, codecs y herramientas UNER. | `web:src/types/UnerProtocolCMDTypes.ts:83`; structural |
| <a id="fn-xorchecksum"></a>`xorChecksum(bytes: number[]): inferred-by-TypeScript` | Adapta la llamada a `bytes.reduce` con el contexto de esta integración. | `web:src/features/protocolStudio/utils.ts:33`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-unerprotocol.buildapppinconfig"></a>`UNERProtocol.buildAppPinConfig(params: AppPinConfigParams): Uint8Array` | Coordina `PayloadBuilder.appPinConfig`, `this.buildPacket` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerProtocol.ts:420`; structural |
| <a id="fn-unerprotocol.buildappthemeconfig"></a>`UNERProtocol.buildAppThemeConfig(params: AppThemeConfigParams): Uint8Array` | Coordina `PayloadBuilder.appThemeConfig`, `this.buildPacket` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerProtocol.ts:430`; structural |
| <a id="fn-unerprotocol.buildheartbeatset"></a>`UNERProtocol.buildHeartbeatSet(params: HeartbeatSetParams): Uint8Array` | Coordina `PayloadBuilder.heartbeatSet`, `this.buildPacket` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerProtocol.ts:377`; structural |
| <a id="fn-unerprotocol.buildpacket"></a>`UNERProtocol.buildPacket(cmd: number, payload?: Uint8Array): Uint8Array` | Coordina `UNERProtocol.calcChecksum`, `buf.set`, `buf.subarray` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerProtocol.ts:333`; structural |
| <a id="fn-unerprotocol.buildtelemetrysetrate"></a>`UNERProtocol.buildTelemetrySetRate(params: TelemetrySetRateParams): Uint8Array` | Coordina `PayloadBuilder.telemetrySetRate`, `this.buildPacket` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerProtocol.ts:414`; structural |
| <a id="fn-unerprotocol.buildwifigetmode"></a>`UNERProtocol.buildWifiGetMode(): Uint8Array` | Adapta la llamada a `this.buildPacket` con el contexto de esta integración. | `web:src/api/UnerProtocol.ts:383`; structural |
| <a id="fn-unerprotocol.buildwifigetscan"></a>`UNERProtocol.buildWifiGetScan(): Uint8Array` | Adapta la llamada a `this.buildPacket` con el contexto de esta integración. | `web:src/api/UnerProtocol.ts:388`; structural |
| <a id="fn-unerprotocol.buildwifisetap"></a>`UNERProtocol.buildWifiSetAP(params: WifiSetAPParams): Uint8Array` | Coordina `PayloadBuilder.wifiSetAP`, `this.buildPacket` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerProtocol.ts:393`; structural |
| <a id="fn-unerprotocol.buildwifisetsta"></a>`UNERProtocol.buildWifiSetSTA(params: WifiSetSTAParams): Uint8Array` | Coordina `PayloadBuilder.wifiSetSTA`, `this.buildPacket` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerProtocol.ts:403`; structural |
| <a id="fn-unerprotocol.calcchecksum"></a>`UNERProtocol.calcChecksum(data: Uint8Array): number` | Operación calc Checksum de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/api/UnerProtocol.ts:439`; inferred-name |
| <a id="fn-unerprotocol.clearflag"></a>`UNERProtocol.clearFlag(): void` | Operación clear Flag de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/api/UnerProtocol.ts:326`; inferred-name |
| <a id="fn-unerprotocol.getlastpacket"></a>`UNERProtocol.getLastPacket(): UNERPacket` | Adapta la llamada a `this._packet.payload.slice` con el contexto de esta integración. | `web:src/api/UnerProtocol.ts:316`; structural |
| <a id="fn-unerprotocol.packetready"></a>`UNERProtocol.packetReady(): boolean` | Operación packet Ready de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/api/UnerProtocol.ts:311`; inferred-name |
| <a id="fn-unerprotocol.parse"></a>`UNERProtocol.parse(): UNERStatus` | Coordina `this._payloadBuf.slice`, `this.onPacket`, `this.resetParser` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerProtocol.ts:173`; structural |
| <a id="fn-unerprotocol.pushbyte"></a>`UNERProtocol.pushByte(b: number): UNERStatus` | Operación push Byte de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/api/UnerProtocol.ts:160`; inferred-name |
| <a id="fn-unerprotocol.resetparser"></a>`UNERProtocol.resetParser(): void` | Operación reset Parser de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/api/UnerProtocol.ts:146`; inferred-name |
| <a id="fn-createunerv2streamparser.discard"></a>`createUnerV2StreamParser.discard(count: number): inferred-by-TypeScript` | Adapta la llamada a `buffer.slice` con el contexto de esta integración. | `web:src/api/UnerFrameV2.ts:301`; structural |
| <a id="fn-createunerv2streamparser.push"></a>`createUnerV2StreamParser.push(chunk: Uint8Array): inferred-by-TypeScript` | Coordina `Math.max`, `Uint8Array.from`, `buffer.push` para Catálogo, codecs y herramientas UNER. | `web:src/api/UnerFrameV2.ts:311`; structural |
| <a id="fn-createunerv2streamparser.reset"></a>`createUnerV2StreamParser.reset(): inferred-by-TypeScript` | Operación reset de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/api/UnerFrameV2.ts:380`; inferred-name |
| <a id="fn-decodecontrolsnapshot.f"></a>`decodeControlSnapshot.f(offset: number): inferred-by-TypeScript` | Adapta la llamada a `view.getFloat32` con el contexto de esta integración. | `web:src/protocol/f4Payloads.ts:242`; structural |
| <a id="fn-decodetelemetrysnapshot.f"></a>`decodeTelemetrySnapshot.f(offset: number): inferred-by-TypeScript` | Adapta la llamada a `view.getFloat32` con el contexto de esta integración. | `web:src/protocol/f4Payloads.ts:222`; structural |
| <a id="fn-encodecommandfields.emit"></a>`encodeCommandFields.emit(ids: string[]): inferred-by-TypeScript` | Adapta la llamada a `ids.flatMap` con el contexto de esta integración. | `web:src/features/protocolStudio/payloadCodec.ts:51`; structural |
| <a id="fn-parsempudata.flagvalue"></a>`parseMpuData.flagValue(key: string, mask: number): boolean` | Operación flag Value de Catálogo, codecs y herramientas UNER; semántica fina pendiente de ampliación. | `web:src/protocol/f4Payloads.ts:358`; inferred-name |
