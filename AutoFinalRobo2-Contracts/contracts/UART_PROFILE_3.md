> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

# Aviso de sustitución — AFR2-20261024-01

Este borrador NO es el catálogo activo. Colisiona con EX01 de la ficha19 en
0x10..0x15. Conservar ROBO2/perfil2 y usar [EX01 v2](EX01_WEB_INTEGRATION.md),
secuencias0x40..0x45. El texto siguiente se conserva como evidencia del diseño21.

---

# ROBO2: perfil de aplicación UART 3 (Diseño coordinado)

AFR2-20261023-21 · 23/10/2026. Especificación de ampliación de ROBO2 para control supervisado (`0x07–0x0C`), servicios OLED remotos (`0x0D–0x0E`) y carga/ejecución de secuencias temporizadas (`0x10–0x15`).
- **Estado por módulo:**
  - `web`: consumidor y editor de secuencias adaptados en fuente (`afr2-ws-1` y `afr2-sequence-1`), sin compilar ni ejecutar pruebas.
  - `esp`: diseñado; validación, arbitraje de dueño y traducción pendientes de integración en `mega_link` / `broker`.
  - `atmega`: diseñado; PWM/dirección, lease de 1500 ms, servicios OLED remotos y buffer estático de secuencias pendientes de integración y medición de SRAM/pila.

## 1. Conservación de framing y correlación ROBO2

Se conserva íntegramente el framing de [UART_PROFILE_2.md](UART_PROFILE_2.md):
- Trama `ROBO2 N 3A A5 01 ROUTE CMD PAYLOAD XOR`, máximo `N = 64` bytes de payload (`76` bytes totales).
- Ruta `0x31` (ESP→Mega) y `0x13` (Mega→ESP).
- Prefijo obligatorio en toda petición (`8` bytes: `session:u32 LE [0..3]`, `transaction:u32 LE [4..7]`) y en toda respuesta (`9` bytes: `session:u32 LE [0..3]`, `transaction:u32 LE [4..7]`, `result:u8 [8]`).
- `0x01..0x06` mantienen sus definiciones de `UART_PROFILE_2.md`, anunciando `profile = 3` en `0x01 hello` cuando el firmware incorpore este catálogo y añadiendo los bits de capacidad:
  - `bit 6 (0x0040)`: `DRIVE_LEASE` (motores + lease de 1500 ms).
  - `bit 7 (0x0080)`: `OLED_REMOTE` (`0x0D` snapshot / `0x0E` acción UI).
  - `bit 8 (0x0100)`: `SEQUENCES` (`0x10..0x15`, hasta 32 segmentos y 120 s).

## 2. Catálogo ampliado (`0x07–0x0E` y `0x10–0x15`)

Offsets contados desde el inicio del payload (después del prefijo de petición `[0..7]` o respuesta `[0..8]`):

| CMD | Nombre WS asociado | Petición ESP→Mega (`N` / campos) | Respuesta OK Mega→ESP (`N` / campos) |
| --- | --- | --- | --- |
| `0x07` | `control.acquire` | `N=10`: `ttlMs:u16 [8..9]` (500..5000, nominal 1500) | `N=12`: `leaseState:u8 [9]`, `ttlMs:u16 [10..11]` |
| `0x08` | `control.release` | `N=9`: `stopFirst:u8 [8]` (0/1) | `N=10`: `leaseState:u8=0 [9]` |
| `0x09` | `control.renew` | `N=10`: `ttlMs:u16 [8..9]` (1500 ms, renovado cada 500 ms) | `N=12`: `leaseState:u8 [9]`, `ttlMs:u16 [10..11]` |
| `0x0A` | `robot.drive` | `N=16`: `vxMmS:i16 [8..9]`, `vyMmS:i16 [10..11]`, `wzMradS:i16 [12..13]`, `ttlMs:u16 [14..15]` | `N=18`: `accepted:u8 [9]`, `fl:i16 [10..11]`, `fr:i16 [12..13]`, `rl:i16 [14..15]`, `rr:i16 [16..17]` |
| `0x0B` | `robot.test.wheel` | `N=13`: `wheel:u8 [8]` (0..3), `dir:u8 [9]`, `pwmPct:u8 [10]` (0..60), `durationMs:u16 [11..12]` | `N=11`: `wheel:u8 [9]`, `status:u8 [10]` |
| `0x0C` | `robot.mode` | `N=10`: `mode:u8 [8]`, `speedLimitPct:u8 [9]` | `N=10`: `mode:u8 [9]` |
| `0x0D` | `robot.screen.get` | `N=8` (sin cuerpo adicional) | `N=23`: `screenCode:u32 [9..12]`, `selection:u8 [13]`, `firstVisible:u8 [14]`, `flags:u8 [15]`, `lineBits:u8 [16]`, `ultrasonicMm:u16 [17..18]`, `oledErrors:u16 [19..20]`, `ageMs:u16 [21..22]` |
| `0x0E` | `robot.ui.action` | `N=10`: `action:u8 [8]` (0=rotate, 1=select, 2=menu), `delta:i8 [9]` (-1/0/+1) | `N=15`: `screenCode:u32 [9..12]`, `selection:u8 [13]`, `firstVisible:u8 [14]` |
| `0x10` | `robot.sequence.begin` | `N=17`: `seqToken:u32 [8..11]`, `segmentCount:u8 [12]` (1..32), `totalDurationMs:u32 [13..16]` (<=120000) | `N=19`: `seqToken:u32 [9..12]`, `state:u8=1 [13]` (preparing), `maxSegments:u8=32 [14]`, `maxDurationMs:u32=120000 [15..18]` |
| `0x11` | `robot.sequence.append` | `N=33`: `seqToken:u32 [8..11]`, `index:u8 [12]` (0..31), `durationMs:u32 [13..16]`, `fl0:i16 [17..18]`, `fr0:i16 [19..20]`, `rl0:i16 [21..22]`, `rr0:i16 [23..24]`, `fl1:i16 [25..26]`, `fr1:i16 [27..28]`, `rl1:i16 [29..30]`, `rr1:i16 [31..32]` | `N=15`: `seqToken:u32 [9..12]`, `index:u8 [13]`, `state:u8=1 [14]` |
| `0x12` | `robot.sequence.commit` | `N=17`: `seqToken:u32 [8..11]`, `segmentCount:u8 [12]`, `totalDurationMs:u32 [13..16]` | `N=19`: `seqToken:u32 [9..12]`, `state:u8=2 [13]` (ready), `segmentCount:u8 [14]`, `totalDurationMs:u32 [15..18]` |
| `0x13` | `robot.sequence.start` | `N=16`: `seqToken:u32 [8..11]`, `execToken:u32 [12..15]` | `N=23`: `seqToken:u32 [9..12]`, `execToken:u32 [13..16]`, `state:u8=3 [17]` (running), `activeIndex:u8=0 [18]`, `elapsedMs:u32=0 [19..22]` |
| `0x14` | `robot.sequence.cancel` | `N=17`: `seqToken:u32 [8..11]`, `execToken:u32 [12..15]`, `reason:u8 [16]` | `N=19`: `seqToken:u32 [9..12]`, `execToken:u32 [13..16]`, `state:u8=5 [17]` (cancelled), `stopped:u8=1 [18]` |
| `0x15` | `robot.sequence.status` | `N=16`: `seqToken:u32 [8..11]`, `execToken:u32 [12..15]` | `N=28`: `seqToken:u32 [9..12]`, `execToken:u32 [13..16]`, `state:u8 [17]`, `activeIndex:u8 [18]`, `segmentCount:u8 [19]`, `elapsedMs:u32 [20..23]`, `totalDurationMs:u32 [24..27]` |

## 3. Reglas de integridad y supervisión

- **Inmutabilidad y atomicidad de carga**: `0x10 begin` limpia el buffer estático y pone el estado en `preparing` (`1`). `0x11 append` transporta exactamente un segmento normalizado (`N = 33 <= 64`) y exige `index` consecutivo. `0x12 commit` verifica que se hayan recibido todos los `segmentCount` segmentos y que la suma de sus `durationMs` coincida exactamente con `totalDurationMs` (`<= 120000 ms`, múltiplos de `10 ms`). Una carga incompleta jamás pasa a `ready` (`2`) ni es ejecutable.
- **Límites físicos estrictos**: máximo **32 segmentos** y **120 000 ms (120 s)**. Todo programa que exceda cualquiera de los dos límites se rechaza con `BAD_REQUEST` sin truncar.
- **Buffer AVR estático**: sin `malloc`/`calloc`/`realloc`. `32 × 20 bytes = 640 bytes` de tabla de segmentos en SRAM; debe verificarse el presupuesto de SRAM y pila de `ATmega2560` antes de activar `AFR2_CAP_SEQUENCES`.
- **Supervisión de lease (1500 ms / 500 ms)**: mientras Control esté visible y supervisado, la Web renueva `control.renew` cada `500 ms` para un lease de `1500 ms`. Si vence el lease, se pierde la sesión, ocurre `STOP` (`0x06`) o `cancel` (`0x14`), el ATmega detiene inmediatamente los cuatro motores (`PWM = 0`) y pasa la secuencia a `cancelled` (`5`). Nunca se reanuda automáticamente.
