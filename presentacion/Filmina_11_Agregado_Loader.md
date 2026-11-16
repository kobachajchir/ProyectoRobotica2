# Filmina 11 · Nuestro agregado: AFR2 Loader

**Mensaje clave:** una aplicación de Windows propia que compila, carga y diagnostica ATmega y ESP desde un solo lugar, con
confirmaciones físicas, backups y un intérprete del enlace que lee nuestro catálogo de comandos.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «AFR2 Loader · compilar, cargar y depurar»          │
├──────────────────────┬───────────────────────────────────────────────────┤
│ A  QUÉ RESUELVE      │ B  FOTO-54 (Operación)   c5–c8 · f2–f4            │
│ lista c1–c4 · f2–f5  │    FOTO-55 (Flasher)     c9–c12 · f2–f4           │
│                      ├───────────────────────────────────────────────────┤
│                      │ C  D-09 flujo del Loader  c5–c12 · f5–f6          │
├──────────────────────┤                                                   │
│ D  Datos (chips) f6  │                                                   │
└──────────────────────┴───────────────────────────────────────────────────┘
```
Opcional: FOTO-56 (monitor serie) y FOTO-58 (MCP) como segunda fila reducida, o en notas.

## Contenido por zona

### A · Qué resuelve (5–6 viñetas)
- **Un flujo, dos chips:** destino ATmega (firmware completo o solo comunicación) o ESP (firmware propio, Web/LittleFS o paquete AT).
- **Compila y prepara** con las herramientas configurables (avr-gcc/make, PlatformIO, scripts de FileSystem); sin rutas fijas: `{App}`/módulos,
  perfiles y registros de cada usuario **fuera del repositorio**.
- **Carga segura:** avrdude al Mega; esptool al ESP **a través del Mega** (flasher temporal en software, USB ↔ USART1 ↔ ESP). Hash verificado en la
  misma sesión; backup de flash opcional antes de sobrescribir; confirmaciones físicas (KEY/RST, D18/D19) siempre visibles.
- **KEY/RST manual o automático** (circuito de la filmina 12).
- **Monitor serie** (ASCII/HEX, TX/RX a archivo) + **intérprete USB**: decodifica el enlace ROBO2/EX01 con `wire_catalog.json`; pestañas
  *Arranques ESP/ATmega* y *Reinicios* (causa de reset, excepción, `epc1`).
- **Servidor MCP** para asistentes de IA: solicitudes que el operador **revisa**; sin herramienta de comandos arbitrarios.

### B · FOTO-54 / FOTO-55
Capturas del Loader (pestañas Operación y Flasher). *Aviso: las existentes son de la versión 1.2.0; si se puede, recapturar con la
versión 2.4.0 (fuente) o la 2.0.0 distribuida.* Pie: *Compilación y carga del robot · pestañas Operación, Flasher, Monitor serie,
Historial, Eventos, Conversor HEX/ASCII, Servidor MCP, Configuración.*

### C · D-09 ([D-09_flujo_Loader.svg](diagramas/D-09_flujo_Loader.svg))
Diez pasos: Operación → Perfil → Compilar/preparar → Confirmaciones + backup → Flasher temporal → avrdude/esptool con hash →
restaura producto → Monitor → Intérprete → MCP.

### D · Chips de datos
| Chip | Dato |
| --- | --- |
| **2.4.0** | versión de fuente del Loader (última ficha 08/11/2026); distribución **2.0.0** (ZIP + SHA-256) |
| **C# / Windows** | aplicación propia; ejecutable `AFR2 Loader.exe` + lanzadores |
| **portátil** | rutas relativas a la aplicación; perfil de ejemplo incluido |
| **MANUAL.html** | manual propio del módulo |

## Guion del orador (≈ 60 s)

«Cargar firmware a un Mega y a un ESP desde cero es propenso a errores, y más cuando el ESP se programa a través del Mega. Hicimos el
Loader: eliges el puerto y el destino, compila, y para el ESP instala un puente en el Mega que reenvía el USB a la UART del ESP. Verifica
el hash en la misma sesión y puede guardar un backup antes. Además trae un monitor serie y un intérprete que entiende nuestros
comandos usando el mismo catálogo que la Web, y un servidor MCP para trabajar con un asistente sin dejarle ejecutar nada sin revisión.»

## Fuentes

`AutoFinalRobo2-Loader/README.md`, `MANUAL.html`, `PORTABILIDAD.md`, `MCP.md` · `AutoFinalRobo2-Contracts/docs/PLAN_LOADER_INTERPRETE_Y_TAP.md` ·
`contracts/LOADER_FLASH_MODES.md` · `hardware/PLAN_PRUEBA_FLASHER_MEGA.md`.

## Advertencias

- **El flasheo real con hardware no está comprobado** según el README (2.0.0): presentar el flujo como implementado y compilado.
- Las capturas disponibles son anteriores al intérprete; no mostrarlas como si incluyeran esa función.
- El MCP no demuestra ida y vuelta por sí solo (`boot mode 1` ≠ comunicación completa).
