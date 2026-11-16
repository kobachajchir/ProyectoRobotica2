# Filmina 2 · Modelos 3D: auto base, brazo v17 y conjunto

**Mensaje clave:** el diseño mecánico se hizo en CAD paramétrico y se exporta tal cual a la Web; el brazo v17 es el resultado de
varias iteraciones guiadas por masa y par.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Modelos: auto base · brazo v17 · conjunto»        │
├─────────────────────┬─────────────────────┬──────────────────────────────┤
│ A  AUTO BASE        │ B  BRAZO V17        │ C  CONJUNTO                  │
│ c1–c4 · f2–f5       │ c5–c8 · f2–f5       │ c9–c12 · f2–f5               │
│ FOTO-03 (grande)    │ FOTO-07 (grande)    │ FOTO-01 (grande)             │
│ FOTO-05/06 (mini)   │ tabla de cotas      │ FOTO-08 captura Web Modelos  │
├─────────────────────┴─────────────────────┴──────────────────────────────┤
│ D  Datos clave de cada modelo (3 columnas alineadas con A·B·C)  f6       │
└──────────────────────────────────────────────────────────────────────────┘
```

## Contenido por zona

### A · Auto base (OSOYOO Mecanum 4WD)
- **FOTO-03** render Blender frente (principal), **FOTO-05** y **FOTO-06** lateral derecho/izquierdo (Fusion) en miniatura;
  opcional **FOTO-04** posterior.
- Datos (columna D-A): chasis 245 × 200 mm (placa) · rueda Mecanum Ø80 mm · 4 motores GM25-370 con encoder Hall ·
  ATmega2560 + shield Wi-Fi ESP-12S · 2 puentes H (Model Y) · batería 2×18650.
- **FOTO-11** (hoja del fabricante con cotas 245/155/200 mm) y **FOTO-10** (rueda Mecanum) como apoyo en notas.
- Procedencia: modelo armado en Fusion (v16) con CADs descargados/locales e inferencias documentadas, sin cables.
  Exportado a Blender y de ahí a GLB (871.756 bytes) que usa la Web.

### B · Brazo antropomorfo v17
- **FOTO-07** render del brazo. Tabla de cotas (pegar tal cual):

| Tramo | Valor |
| --- | --- |
| Hombro–codo (L1) | 70 mm |
| Codo–J4 | 60 mm |
| J4–J5 / J5–J6 / J6–TCP | 55 / 65 / 78,5 mm |
| Tubos PVC | Ø25 × 1,2 mm y Ø20 × 1 mm |
| Transmisiones | J2 engranaje 18/72 (i=4) · J3 correa HP3M 20/50 (i=2,5) · J4 24/24 |
| Actuadores | 5 × STS3250 (J1–J5) + 2 × STS3032 A0 (J6, pinza) |
| Pinza | V7, dos cremalleras + piñón de 24 dientes, módulo 1; apertura ≈ 4–79 mm |

- Rango de ejes: J1 ±90° · J2 −10…+125° · J3–J6 ±90°. Pose de transporte [0°, 15°, −40°, 0°, 25°, 0°].
- Libro de planos: 66 definiciones fabricadas/adaptadas, 6 conjuntos (A100–A600), PDF A3.

### C · Conjunto
- **FOTO-01** render del robot completo y **FOTO-08** captura de la Web en `#/modelos` (visor 3D con puntos de componentes).
- Ensamble: brazo montado en la plataforma con transformación fija (0,0278; 0; 0,079) m y giro −90° sobre Z.
- Pasada de **FOTO-09** (captura de `#/armado`: ensamblaje animado por scroll) si se quiere mostrar el recorrido de armado.

### D · Datos clave (franja inferior)
| A · Auto | B · Brazo | C · Conjunto |
| --- | --- | --- |
| Base de movilidad en el plano (3 DOF: x, y, ψ) | 6 DOF + pinza | 9 DOF de configuración Q = (x, y, ψ, q1…q6) |
| GLB final 871.756 B | 2.345 kg estimados (CAD, sin carga) | 4.045 kg con auto 1,5 kg y carga 200 g |

## Guion del orador (≈ 60 s)

«Modelamos todo en Fusion. A la izquierda el auto base con ruedas Mecanum; en el centro el brazo en su versión 17, que bajó a
70 y 60 milímetros de eslabón y usa tubos de PVC para aliviar masa; a la derecha el conjunto. Cada transmisión fue elegida mirando
el par: la reducción 4 a 1 en el hombro, la correa 2,5 a 1 que lleva el motor del codo cerca del hombro para bajar el momento.
Ese mismo modelo se exporta a Blender y de ahí a la Web, de modo que el simulador y el CAD coinciden.»

## Fuentes

`AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/README.md`, `SELECCION_ACTUADORES.md`, `ENSAMBLES_Y_MOVIMIENTO.md` ·
`AutoFinalRobo2-CAD/docs/MODELO_FINAL_WEB_20261025.md` · `docs/informe_20261109/AFR2_Informe_Borrador_D1.md` (cap. 3–4, 10) ·
`Documentacion/Dimensiones.png`.

## Advertencias

- Todo es **geometría CAD**: no hay brazo fabricado ni aprobado para fabricación (tolerancias, ajustes y material sin definir).
- Los renders son del mismo CAD; no reemplazan fotos del prototipo (FOTO-02 pendiente).
- La masa 2,345 kg es por volumen y densidad (PLA macizo 1,24 g/cm³); la impresa real será distinta.
- Algunos parámetros heredados del CAD aún dicen `pvc_od32`; el descriptor vigente es PVC25/20.
