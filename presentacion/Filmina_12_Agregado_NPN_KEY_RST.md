# Filmina 12 · Nuestro agregado: KEY y RST automáticos con transistores NPN

**Mensaje clave:** dos NPN en paralelo con los pulsadores del shield permiten que el Mega ponga al ESP en modo carga y lo reinicie
sin tocar nada, sin interferir con otros usos del shield.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «KEY y RST automáticos (NPN)»                       │
├────────────────────────────────────┬─────────────────────────────────────┤
│ A  D-06 CIRCUITO (ambos canales)   │ B  FOTO-61 shield (KEY / RST)       │
│ c1–c7 · f2–f5                      │ c8–c12 · f2–f3                      │
│                                    ├─────────────────────────────────────┤
│                                    │ C  Problema → solución (3 viñetas)  │
│                                    │ c8–c12 · f4–f5                      │
├────────────────────────────────────┴─────────────────────────────────────┤
│ D  FOTO-62 · FOTO-63 · FOTO-64  (circuito real, tester, cables) c1–c12·f6 │
└──────────────────────────────────────────────────────────────────────────┘
```

## Contenido por zona

### A · D-06 ([D-06_circuito_KEY_RST_NPN.svg](diagramas/D-06_circuito_KEY_RST_NPN.svg))
Por canal: **Mega D32/D33 → R 1 kΩ → base**; **R 10 kΩ de base a GND** (lado del transistor); **colector → pata de señal del pulsador**
(KEY = GPIO0, RST = reset); **emisor → GND común**. Tabla de redes (si cabe):

| Red | Une |
| --- | --- |
| `KEY_CTRL` / `RST_CTRL` | D32 / D33 – 1 kΩ |
| `Q1_B` / `Q2_B` | 1 kΩ – base – 10 kΩ |
| `ESP_KEY` / `ESP_RST` | colector – pata de señal del pulsador |
| `GND` | emisores, 10 kΩ, GND del Mega y del shield |

Componentes: 2 NPN (2N3904: E-B-C · BC547: C-B-E; **verificar con la hoja de datos**), 2 × 1 kΩ, 2 × 10 kΩ, cables ≤ 10 cm.

### B · FOTO-61
Vista superior del shield OSOYOO con los pulsadores **KEY** y **RST** a la izquierda. Marcar con flechas carmesí las dos patas donde se
sueldan los colectores. (Imagen del fabricante; no es nuestro montaje.)

### C · Problema → solución
- **Antes:** para cargar el ESP había que apretar KEY y RST a mano (y se usaba el método D0/D1 con RESET del Mega a GND, ya **retirado**).
- **Ahora:** el flasher en software (USART0 ↔ USART1) deja el RESET del Mega libre, así el Mega puede comandar D32/D33.
- **Secuencia de entrada en carga:** KEY alto → 20 ms → RST alto 100 ms → RST bajo → ~200 ms con KEY apretado → puente transparente + esptool →
  se suelta KEY.
- **Convivencia (requisito):** al arrancar D32/D33 son **entradas sin pull-up**; con los cables de control sacados, el circuito queda
  inerte; el pulsador sigue funcionando a mano; el modo automático es *opt-in* en el Loader y primero identifica al flasher AFR2.

### D · Fotos del circuito real (por tomar ✱)
**FOTO-62** circuito armado (vista cenital, componentes rotulados Q1/Q2/R1–R4) · **FOTO-63** medición con tester (reposo ≈ 3,3 V en KEY/RST
y < 0,3 V con el pin en alto) · **FOTO-64** cableado por colores (según el usuario: RST = marrón, KEY = violeta; cadena azul→naranja→marrón y
celeste→amarillo→violeta; **confirmar a qué pin va cada uno antes de rotular**).

## Guion del orador (≈ 60 s)

«El ESP necesita dos botones para entrar en modo de carga: KEY, que baja GPIO0, y RST. Los reemplazamos con dos transistores NPN
puestos en paralelo con los pulsadores del shield, comandados por D32 y D33 del Mega a través de una resistencia de un kilo. Una
resistencia de diez kilos mantiene la base a masa para que el transistor esté apagado aunque el Mega no tenga programa. Así
el Loader puede cargar el ESP sin que nadie toque nada, y si alguien saca los cables de control, el shield se comporta como uno de
fábrica. Hoy está implementado en el firmware y el Loader; el circuito se está armando y falta medirlo.»

## Fuentes

`AutoFinalRobo2-Contracts/hardware/PLAN_KEY_RST_AUTOMATICO.md` · `changes/2026-11-06-11-esp-key-rst-automatico.md` · `hardware/WIFI_SHIELD.md` ·
`hardware/FLASHING.md` · memoria de trabajo (colores de cables, mediciones preliminares: pulsador de 2 patas, una con continuidad a masa y otra 0,125 V en diodo, sin alimentar).

## Advertencias

- **Circuito sin armar ni medido** al 06/11/2026 (estado del plan): no afirmar que funciona. Los criterios de aceptación están definidos, no acreditados.
- Falta medir reposo (~3,3 V) y apretado en KEY y RST antes de soldar; NPN solo sirve si el pulsador lleva la señal a masa (control 2–4 del plan).
- Asignación de cables de colores a D32/D33 y a base/colector: sin confirmar.
