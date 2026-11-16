# Filmina 1 · Introducción y arquitectura del sistema

**Mensaje clave:** un auto Mecanum de 4 ruedas lleva un brazo de 6 ejes; ambos se comandan desde un celular o desde un panel
propio (OLED + encoder), con un software que nosotros escribimos de punta a punta.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «AutoFinalRobo2 · manipulador móvil Mecanum»       │
├───────────────────┬──────────────────────────────────────────────────────┤
│ A  FOTO-01        │ B  DIAGRAMA D-01  (arquitectura)                     │
│ robot completo    │ c5–c12 · f2–f4                                       │
│ c1–c4 · f2–f6     ├────────────────────────┬─────────────────────────────┤
│ (hero, vertical)  │ C  Problema y objetivo │ D  Seis módulos (tabla)     │
│                   │ c5–c8 · f5–f6          │ c9–c12 · f5–f6              │
├───────────────────┴────────────────────────┴─────────────────────────────┤
│ E  Franja de cifras  c1–c12 · f6 (pie)  5–6 chips                        │
└──────────────────────────────────────────────────────────────────────────┘
```

## Contenido por zona

**Título:** `AutoFinalRobo2` · subtítulo: *Manipulador antropomorfo de 6 ejes sobre plataforma Mecanum 4WD*.
Línea de autoría: Koba Chajchir · [institución, asignatura, docentes e integrantes: **completar**].

**A · FOTO-01** (render del robot completo, fondo transparente: ponerle fondo `#F3F5F9`). Pie: *Conjunto CAD v17: auto
OSOYOO Mecanum + brazo antropomorfo + pinza V7. Render de geometría, no foto del prototipo.* Si se toma **FOTO-02** (foto real),
va en una miniatura inferior dentro de la misma zona.

**B · D-01** ([D-01_arquitectura_general.svg](diagramas/D-01_arquitectura_general.svg)). Leer de izquierda a derecha:
Operador → Web (React) → ESP8266 → ATmega2560 → motores y sensores. Debajo: panel local OLED + encoder y sensores entran al Mega.
Arriba: el Loader carga ambos micros por USB.

**C · Problema y objetivo** (3 viñetas, 14 pt):
- *Problema:* convertir una consigna (ir a X,Y · tomar un cubo) en movimientos coordinados de base y brazo, sabiendo en qué
  referencia y unidad está cada número.
- *Objetivo:* plataforma móvil con manipulación, documentada con modelo matemático, CAD, firmware y una interfaz reproducible.
- *Criterio:* separar **dato nominal · CAD · código · medición física** (ver README §4).

**D · Seis módulos bajo un único Git** (tabla de 2 columnas):

| Módulo | Qué es |
| --- | --- |
| Atmega | C / avr-libc, sin Arduino; motores, sensores, UI local, odometría, GOTO |
| ESP | Arduino ESP8266 propio (sin comandos AT); Wi-Fi, HTTP, WebSocket, LittleFS |
| Web | React 19 + Vite + TypeScript; control, simuladores, OLED virtual |
| Contracts | contratos del enlace, pinout, evidencias, versionado |
| CAD | Fusion/Blender (auto, brazo v17), KiCad (PCB del panel) |
| Loader | aplicación Windows (C#) que compila y carga el firmware |

**E · Cifras** (chips con número grande y rótulo pequeño; todas con fuente):

| Chip | Dato | Fuente |
| --- | --- | --- |
| **4 + 6** | ruedas Mecanum Ø80 mm + articulaciones del brazo (más pinza) | `afr2_geometry.h`, descriptor v17 |
| **7** | servos: 5 STS3250 (J1–J5) + 2 STS3032 (J6 y pinza) | `revision_v17/README.md` |
| **54** | comandos del enlace ESP↔ATmega en `wire_catalog.json` | `contracts/wire_catalog.json` |
| **101** | pantallas OLED catalogadas en `screens.json` | `documentation/screens.json` |
| **1.27.0** | versión de producto única (ATmega = ESP = Web) | `afr2_build_info.h` |
| **≈7.300 / 2.200** | líneas de C del ATmega (`src/`) / de C++ del ESP (`src/`) | `wc -l` del 11/11/2026 |

## Guion del orador (≈ 60 s)

«AutoFinalRobo2 es un robot móvil con brazo. La base es un auto Mecanum de cuatro ruedas que se mueve en cualquier dirección
sin girar; encima lleva un brazo antropomorfo de seis articulaciones y una pinza. Lo comandamos desde el celular, por Wi-Fi,
o desde un panel propio con pantalla OLED y un encoder. Todo el software es nuestro: el ATmega2560 maneja motores y sensores,
el ESP8266 hace de servidor web y puente, y la interfaz web incluye simuladores. Un cargador para Windows compila y carga los
dos micros. Un detalle de método: separamos siempre lo que es dato de catálogo, lo que sale del CAD, lo que está en el código
y lo que medimos con instrumentos.»

## Fuentes

`INFORME_MAESTRO.md` · `AutoFinalRobo2-Contracts/README.md` · `docs/informe_20261109/AFR2_Informe_Borrador_D1.md` (§1–2) ·
`AutoFinalRobo2-Contracts/contracts/afr2_build_info.h` · `contracts/wire_catalog.json` · `documentation/screens.json`.

## Advertencias

- No decir «el robot funciona completo»: hay mediciones físicas parciales (ver [ANEXO_Pendientes_y_alertas.md](ANEXO_Pendientes_y_alertas.md)).
- FOTO-01 es un render CAD, no una foto del prototipo.
- Datos académicos de portada pendientes.
