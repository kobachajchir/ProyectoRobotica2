# Filmina 4 · Firmwares: qué son, qué hacen y de qué se encargan · ciclo principal

**Mensaje clave:** tres programas propios con responsabilidades separadas. El ATmega es la **autoridad de las salidas**; el ESP
es transporte y servidor; la Web es interfaz y simulación. Un único contrato compartido los mantiene de acuerdo.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Firmwares y ciclo principal»                       │
├─────────────┬─────────────┬─────────────┬────────────────────────────────┤
│ A  ATmega   │ B  ESP8266  │ C  Web      │ D  DIAGRAMA D-03a              │
│ c1–c3       │ c4–c6       │ c7–c9       │ ciclo principal (main.c)       │
│ f2–f5       │ f2–f5       │ f2–f5       │ c10–c12 · f2–f6 (vertical)     │
├─────────────┴─────────────┴─────────────┤                                │
│ E  Enlace compartido (franja) c1–c9 · f6│                                │
└─────────────────────────────────────────┴────────────────────────────────┘
```
Alternativa si el diagrama D-03a queda chico: mover A·B·C a una fila de tres tarjetas c1–c8 y dar a D las columnas c9–c12 con altura
completa; **D-03b** (servicio de control) va en la filmina 6 o como filmina de apoyo.

## Contenido por zona

### A · ATmega2560 (tarjeta, ícono de chip)
- **Qué es:** firmware en **C / avr-libc**, sin Arduino y sin DMA. Un solo lazo cooperativo (`main.c`).
- **De qué se encarga:** motores (PWM + sentido) · Hall por interrupción (PCINT2) · odometría · PID por rueda · seguidor de
  línea · GOTO · radar (HC-SR04 + servo) · batería · OLED + encoder (UI local) · EEPROM · watchdog · enlace ROBO2 con el ESP.
- **Garantías de seguridad:** arranca con salidas apagadas · STOP prioritario · TTL de 400 ms por consigna · corte a 1,5 s sin pulso Web ·
  fallos F1–F4 que cortan la marcha.
- **Estructura:** `platform/` (placa, motor, sensores, TWI, UART1, EEPROM, watchdog) · `services/` (app, producto, enlace, secuencias,
  red, depuración, I2C Manager) · `ui/` (pantallas, catálogo de menú, dashboard) · `input/` (encoder) · `display/` (OLED).
- **Cifra de memoria** (build del 06/11/2026): SRAM estática 5.614 B; libres antes de pila 2.578 B. Versión 1.27.0.

### B · ESP8266 (ESP-12S)
- **Qué es:** firmware propio **Arduino ESP8266 + LittleFS**, sin comandos AT.
- **De qué se encarga:** punto de acceso/estación Wi-Fi (AP · STA · AP+STA, escaneo, guardado de credenciales) · servidor **HTTP :80**
  que entrega la Web comprimida (`.gz`, manifiesto) · **WebSocket `/ws`** (`afr2-ws-1`, JSON) · **broker** (2 sesiones, cola común,
  STOP reservado) · puente hacia el Mega por UART (`mega_link`).
- **No decide movimiento:** retransmite; el ATmega valida y ejecuta.
- **Cifra:** RAM estática 46.644 B tras la optimización 1.10.5 (−4.528 B); AP abierto `AutoFinalRobo2-<chipId>` si no hay red guardada.

### C · Web (React 19 + Vite + TypeScript + Tailwind v4)
- **Qué es:** aplicación de una sola página servida por el ESP (HashRouter).
- **Vistas:** Inicio · Conducir · Programas (GOTO, secuencias) · Puesta a punto (calibración) · Sensores · Modelos 3D · Armado ·
  Simulación (Auto / Brazo / Conjunta) · Conexión Wi-Fi · Ayuda (OLED virtual) · Infraestructura · Desarrollador (Protocol Studio).
- **Cliente `afr2-ws-1`:** `system.hello` (5 s), latido `system.alive` (2 s / 6 s), reconexión con *jitter*, un único slot reemplazable para
  `robot.drive`, prioridad máxima para `robot.stop`.
- **Simuladores:** corren en el navegador; no mueven el robot físico.

### D · D-03a ([D-03a_ciclo_principal_ATmega.svg](diagramas/D-03a_ciclo_principal_ATmega.svg))
Diagrama de flujo del ciclo del ATmega, en 8 pasos numerados: ① servicios del producto → ② enlace ESP (≤ 64 B por vuelta) →
③ encoder → ④ control puntual → ⑤ TWI/I2C → ⑥ ¿redibujar? → ⑦ OLED ≤ 16 B por turno → ⑧ watchdog. El detalle de ① está en
**D-03b** ([D-03b_servicio_de_control.svg](diagramas/D-03b_servicio_de_control.svg)): sensores → vencimientos → secuencias → odometría
→ modo → PID por rueda → rampa → PWM.

### E · Enlace compartido (franja inferior, 3 chips)
| Chip | Texto |
| --- | --- |
| **Navegador ↔ ESP** | WebSocket JSON `afr2-ws-1`, mismo origen que el HTTP |
| **ESP ↔ ATmega** | UART **ROBO2** perfil 2 (tokens 0x3A/0xA5, rutas 0x31/0x13, carga útil 64 B) + extensión de producto **EX01 v13** |
| **Contracts** | `wire_catalog.json` (54 comandos) es la fuente única: lo leen el intérprete del Loader y la Web; header, `commands.json` y Protocol Studio se actualizan en el mismo cambio |

## Guion del orador (≈ 75 s)

«Tenemos tres programas. El del ATmega, escrito en C puro, es el que realmente manda a los motores: corre un lazo cooperativo,
lee los encoders por interrupción, calcula la odometría y cierra el PID de cada rueda. Si pasa algo raro —un latido que no llega,
un pulso de más, un signo cruzado— corta las salidas. El ESP8266 es el servidor: aloja la interfaz web, crea el punto de acceso y
pasa los mensajes al Mega por una UART con nuestro propio protocolo ROBO2. La Web es la cara visible: control, calibración,
simuladores y un OLED virtual. Y como los tres comparten un catálogo de comandos, cualquier alta o baja se hace en un solo lugar.
A la derecha se ve el ciclo del ATmega: ocho pasos que se repiten sin bloquear nada.»

## Fuentes

`AutoFinalRobo2-Atmega/src/main.c` · `src/services/afr2_product.c` (`Afr2Product_Task`) · `AutoFinalRobo2-ESP/README.md` ·
`AutoFinalRobo2-Web/README.md` y `src/app/App.tsx` (rutas) · `AutoFinalRobo2-Contracts/contracts/PROTOCOL.md`, `afr2_product_wire.h` ·
`INFORME_MAESTRO.md` (fichas AFR2-20261109-09 y AFR2-20261106-04).

## Advertencias

- «Implementado en código» ≠ «probado en el auto»: el firmware se compiló en varias entregas, pero la aceptación física completa está pendiente.
- La SRAM estática no certifica la pila ni las ISR; decir «medida en compilación».
- La seguridad de movimiento descrita es lógica de firmware, no una certificación.
