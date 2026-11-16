# Filmina 9 · Simulador, misión y ciclo de recolección

**Mensaje clave:** antes de mover el robot real, la Web simula auto, brazo y conjunto con la misma matemática; la misión
«recolectar cubos» está definida paso a paso y cada fallo es un resultado posible.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Simulador conjunto y misión del recolector»        │
├──────────────────────┬───────────────────────────────────────────────────┤
│ A  CÓMO ESTÁ DEFINIDO│ B  GALERÍA DEL SIMULADOR  c5–c12 · f2–f4          │
│ el simulador         │  FOTO-33 · FOTO-34 · FOTO-35 · FOTO-36            │
│ c1–c4 · f2–f4        │  (4 capturas, grilla 2×2 o 4×1)                   │
├──────────────────────┼───────────────────────────────────────────────────┤
│ C  MISIÓN (reglas)   │ D  D-05 ciclo de la misión   c5–c9 · f5–f6        │
│ c1–c4 · f5–f6        │ E  FOTO-37/38/39 secuencia toma  c10–c12 · f5–f6  │
└──────────────────────┴───────────────────────────────────────────────────┘
```

## Contenido por zona

### A · Cómo está definido el simulador (`#/simulacion/auto | brazo | conjunta`)
- Escena 3D en el navegador (Three.js sobre el GLB del CAD); estado **local**: no se serializa como telemetría ni mueve el robot.
- **Auto:** ruedas simuladas con la cinemática Mecanum; arrastrar el auto con *Mover auto*.
- **Brazo:** 6 ejes seleccionables, controles a la derecha, pinza independiente, matrices homogéneas en vivo.
- **Conjunto:** `T_mundo,pinza = T_mundo,chasis · T_chasis,montaje · T_J1 … T_J6 · T_TCP`; sensor y depósito son ramas del chasis.
- Convención: X adelante, Y izquierda, Z arriba (Three: (X,Y,Z) → (−Y, Z, −X)); metros y radianes.
- Se detiene al cambiar de ruta, abrir un modal, perder foco o emitir STOP.
- Alcance como **nube de tomas verticales** muestreada cada 25 mm; trayectorias en pasos articulares ≤ 3°.

### B · Galería (imágenes del simulador conjunto, capturas ⌗)
| Foto | Qué mostrar |
| --- | --- |
| **FOTO-33** | Perspectiva: auto + brazo + 6 cubos (semilla 42) + depósito de 4 celdas |
| **FOTO-34** | Vista superior con alcance (nube de tomas) |
| **FOTO-35** | Vista lateral con el brazo en preagarre sobre un cubo |
| **FOTO-36** | Radar ideal: barrido de 37 direcciones y ecos |

Todas con el mismo encuadre, mismo tema (claro) y 1920×1080 recortadas a 16:10 (ver [FOTOS.md](FOTOS.md)).

### C · Misión (3–5 viñetas)
- Cubos de **50 mm, 100 g**, generador LCG con semilla repetible; 1…12 cubos (hasta 24 manuales), separados ≥ 150 mm; semilla inicial 42, seis cubos.
- **Depósito** de 118 × 118 mm con **4 celdas** en una capa (borde a Z = 190 mm).
- Sensor **virtual** a (103, 0, 45) mm (altura ajustable 20–200 mm): *montaje bajo propuesto*, no la ubicación del sensor real.
- Modelo de eco: rayo horizontal ideal contra cajas, sin ruido ni cono acústico; sin eco = nulo. El autónomo **no** recibe IDs ni centros del generador.
- Fallos explícitos: objetivo perdido · toma inalcanzable · trayectoria bloqueada · agarre fallido · depósito lleno.

### D · D-05 ([D-05_ciclo_mision_recolectora.svg](diagramas/D-05_ciclo_mision_recolectora.svg))
Recoger brazo → barrer → candidato → orientar/centrar → eco frontal reciente → aproximar → detener → medir otra vez → preagarre → tomar
→ elevar → llevar sobre celda → soltar → recoger brazo → ¿quedan cubos? La base **permanece detenida** mientras el brazo manipula.

### E · FOTO-37 / 38 / 39 (secuencia de toma)
Tres capturas consecutivas: preagarre, elevación/transporte, suelta en la celda. Pie común: *Simulación local: toma por IK vertical
(J4 = 0, herramienta hacia abajo; J6 alinea las mordazas con el cubo).* **FOTO-32** (esquema editable de montaje del recolector,
dibujo nominal) puede ir como apoyo.

## Guion del orador (≈ 90 s)

«El simulador usa el mismo modelo 3D y la misma cinemática que el robot. Podemos mover el auto, mover el brazo articulación por
articulación y ver las matrices; el conjunto compone ambas. Para la misión generamos cubos de 50 milímetros con una semilla
repetible. El auto detenido barre con un ultrasonido ideal, detecta un candidato, se orienta, se aproxima y vuelve a medir; luego el
brazo hace una toma vertical con cinemática inversa, levanta el cubo y lo deja en una de cuatro celdas del depósito. Cualquier falla —
objetivo perdido, inalcanzable, agarre fallido, depósito lleno— detiene la misión con su motivo. Es una simulación: sirve para
definir el ciclo y los criterios antes de probar físicamente.»

## Fuentes

`AutoFinalRobo2-Contracts/docs/ROBOT_RECOLECTOR.md` · `AutoFinalRobo2-Web/src/features/collector/*` (`collectorSimulation.ts`, `armKinematics.ts`) ·
`AutoFinalRobo2-CAD/mecanica/robot-recolector/propuesta_montaje.svg` · `changes/2026-11-11-01-pruebas-minimas-recolector.md`.

## Advertencias

- **Simulación, no misión física verificada.** El sensor real puede no ver un cubo desde su ubicación actual; HC-SR04 y servo siguen por validar en el auto.
- La IK es una rama específica de toma vertical, no un solver general; sin autocolisión BRep ni deformación.
- Valores 130/120 mm del texto del recolector son del v02 histórico; el brazo vigente es v17.
