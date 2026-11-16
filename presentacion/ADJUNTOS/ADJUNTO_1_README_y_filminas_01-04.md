# ADJUNTO 1 de 4 · contiene: README.md y las fichas de las filminas 1 a 4. Cada archivo original va entre marcas INICIO/FIN DEL ARCHIVO.

===== INICIO DEL ARCHIVO: README.md =====

# Presentación AutoFinalRobo2 · carpeta de documentación

Esta carpeta es **la base documental para armar el PowerPoint** (no contiene el `.pptx`). Cada filmina tiene su archivo con:
layout con zonas, textos listos para pegar, fórmulas, diagramas, fotos numeradas y dónde van, guion del orador, fuentes y
advertencias de veracidad.

Redactada el 11/11/2026 a partir del repositorio (producto 1.27.0). Autor del proyecto: Koba Chajchir.
No se compiló, ejecutó ni flasheó nada; todo número sale de una fuente citada en cada filmina.

## 1. Orden de la presentación (13 filminas)

| N.º | Filmina | Archivo | Bloque |
| --- | --- | --- | --- |
| 1 | Introducción y arquitectura del sistema | [Filmina_01_Introduccion.md](Filmina_01_Introduccion.md) | Proyecto |
| 2 | Modelos 3D: auto base, brazo v17 y conjunto | [Filmina_02_Modelos.md](Filmina_02_Modelos.md) | Proyecto |
| 3 | Pinout y conexionado | [Filmina_03_Pinout_y_conexionado.md](Filmina_03_Pinout_y_conexionado.md) | Proyecto |
| 4 | Firmwares: ATmega, ESP8266 y Web · ciclo principal | [Filmina_04_Firmwares_y_ciclo.md](Filmina_04_Firmwares_y_ciclo.md) | Proyecto |
| 5 | Matemática del auto (Mecanum) y cálculos de odometría | [Filmina_05_Matematica_auto_y_odometria.md](Filmina_05_Matematica_auto_y_odometria.md) | Proyecto |
| 6 | Programa GOTO y funcionamiento de la odometría | [Filmina_06_GOTO_y_odometria.md](Filmina_06_GOTO_y_odometria.md) | Proyecto |
| 7 | Matemática del brazo y del conjunto | [Filmina_07_Matematica_brazo_y_conjunto.md](Filmina_07_Matematica_brazo_y_conjunto.md) | Proyecto |
| 8 | Esfuerzos, tensiones, vuelco y estabilidad | [Filmina_08_Esfuerzos_y_vuelco.md](Filmina_08_Esfuerzos_y_vuelco.md) | Proyecto |
| 9 | Simulador, misión y ciclo de recolección | [Filmina_09_Simulador_y_mision.md](Filmina_09_Simulador_y_mision.md) | Proyecto |
| 10 | Interfaz: pasos y pantallas (OLED + Web) | [Filmina_10_Interfaz.md](Filmina_10_Interfaz.md) | Proyecto |
| 11 | **Agregado:** AFR2 Loader | [Filmina_11_Agregado_Loader.md](Filmina_11_Agregado_Loader.md) | Nuestros agregados |
| 12 | **Agregado:** KEY y RST automáticos con NPN | [Filmina_12_Agregado_NPN_KEY_RST.md](Filmina_12_Agregado_NPN_KEY_RST.md) | Nuestros agregados |
| 13 | **Agregado:** encoder y OLED | [Filmina_13_Agregado_Encoder_y_OLED.md](Filmina_13_Agregado_Encoder_y_OLED.md) | Nuestros agregados |

Informe visual de diagramación (wireframes con imágenes): [INFORME_DIAGRAMACION.html](INFORME_DIAGRAMACION.html) (se regenera con
`generar_informe_layout.py`) · Prompt de armado del PowerPoint: [PROMPT_ARMADO_PPTX.md](PROMPT_ARMADO_PPTX.md).

Material común: [FOTOS.md](FOTOS.md) (catálogo numerado de fotos y capturas) · [DIAGRAMAS.md](DIAGRAMAS.md) (diagramas D-01…D-10) ·
[ANEXO_Formulario.md](ANEXO_Formulario.md) (todas las fórmulas ordenadas, con ejemplos numéricos) ·
[ANEXO_Pendientes_y_alertas.md](ANEXO_Pendientes_y_alertas.md) (qué NO afirmar y qué datos faltan).

## 2. Contenido de la carpeta

```text
presentacion/
├─ README.md                      este índice y las reglas de diseño
├─ Filmina_01 … Filmina_13 .md    una ficha por filmina
├─ FOTOS.md                       catálogo FOTO-01…FOTO-69 (existentes ✔ / por tomar ✱ / capturas ⌗)
├─ DIAGRAMAS.md                   D-01…D-10, en qué filmina va cada uno
├─ ANEXO_Formulario.md            fórmulas + ejemplos numéricos verificados a mano
├─ ANEXO_Pendientes_y_alertas.md  límites de lo que se puede afirmar
├─ fotos_existentes/              imágenes que YA existen en el repo, copiadas con su número FOTO-NN
└─ diagramas/
   ├─ D-xx_*.svg                  diagramas vectoriales (insertar como imagen; editables)
   ├─ png/                        las mismas en PNG ×2 (por si el PowerPoint no abre SVG)
   └─ generar_diagramas.py        regenera los SVG (solo escribe en esa carpeta)
```

## 3. Reglas de diseño (para que las 13 filminas se vean como una sola)

**Formato.** 16:9 (33,867 × 19,05 cm). Márgenes 1,2 cm. Zona útil 31,5 × 16,6 cm. Título arriba a la izquierda;
pie con número de filmina y versión del producto (1.27.0).

**Grilla.** Cada filmina usa una grilla de 12 columnas × 6 filas (columna ≈ 2,6 cm, fila ≈ 2,8 cm). En cada ficha las zonas
están nombradas **A, B, C…** y llevan su rango de columnas/filas (`c1–c6 · f2–f5`). Respetar esas zonas es lo que mantiene el
orden. Sin más de 3 zonas de contenido por filmina, salvo las de fórmulas (hasta 4).

**Paleta** (la del sistema visual del proyecto):

| Uso | Color |
| --- | --- |
| Acento / títulos de zona | carmesí `#B81D33` |
| Texto | `#1F2430` |
| Fondo | `#FFFFFF` |
| Panel claro | `#F3F5F9` |
| Hardware / entradas | violeta `#6A4BB5` sobre `#EFE9FF` |
| Datos / software | azul `#2D6CB5` sobre `#E4F0FF` |
| Alerta / fallo | `#B81D33` sobre `#FDE7EA` |
| Correcto | verde `#2A8A52` sobre `#E3F5EA` |

**Tipografía.** Segoe UI (o Arial). Título 30 pt negrita · subtítulo de zona 16 pt negrita carmesí · texto 14 pt (mínimo 12 pt)
· fórmulas 16 pt en Cambria Math (Insertar → Ecuación; **no** pegar como imagen) · pie 10 pt.

**Fotos.** Cada foto lleva su rótulo `FOTO-NN` en una etiqueta carmesí pequeña (esquina inferior izquierda) y un pie de una línea.
Las capturas de pantalla se muestran con borde fino gris y sombra suave. Las que son **fotos reales por tomar** (✱) están
descritas con encuadre, fondo y qué debe verse en [FOTOS.md](FOTOS.md).

**Diagramas.** Son SVG con las mismas fuentes y colores; en PowerPoint: Insertar → Imágenes → Este dispositivo. Para editarlos:
clic derecho → *Convertir en forma*. Ancho recomendado: el de la zona asignada en cada ficha; no deformar.

**Fórmulas.** Todas están en [ANEXO_Formulario.md](ANEXO_Formulario.md) con símbolos y unidades. En las filminas se muestran
las esenciales; el resto va en las notas del orador.

## 4. Criterio de evidencia (decirlo en la filmina 1 y respetarlo en todas)

El informe del proyecto usa cuatro niveles; conviene mostrar un pequeño ícono por dato cuando haya duda:

1. **Dato nominal** (hoja de datos, contrato).
2. **Geometría CAD** (modelo Fusion v17).
3. **Implementado en código** (fuente leída; no prueba que funcione en la placa).
4. **Medido físicamente** (con instrumento y fecha).

Hoy hay pocas mediciones físicas (CPR 370, avance 1000 mm = 1000 mm, lateral 1000 mm → 775 mm). El resto de resultados son
CAD, estimaciones o código. Ver [ANEXO_Pendientes_y_alertas.md](ANEXO_Pendientes_y_alertas.md).

## 5. Cómo trabajar con esta carpeta

1. Armar la plantilla maestra del PowerPoint con las reglas de la sección 3 (patrón de diapositivas con título, pie y grilla).
2. Por cada filmina: abrir su ficha, crear las zonas, pegar los textos, insertar diagramas (`diagramas/`) y fotos
   (`fotos_existentes/` o las que se tomen con el número indicado en [FOTOS.md](FOTOS.md)).
3. Guardar las fotos nuevas con el nombre `FOTO-NN_descripcion.jpg` en una carpeta `fotos_nuevas/` (no existe aún) para que
   el número coincida con el catálogo.
4. Pasar el guion del orador a las notas de cada filmina.

Los datos de portada (institución, asignatura, docentes, integrantes) **no están en el repositorio**: completarlos.


===== FIN DEL ARCHIVO: README.md =====

===== INICIO DEL ARCHIVO: Filmina_01_Introduccion.md =====

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


===== FIN DEL ARCHIVO: Filmina_01_Introduccion.md =====

===== INICIO DEL ARCHIVO: Filmina_02_Modelos.md =====

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


===== FIN DEL ARCHIVO: Filmina_02_Modelos.md =====

===== INICIO DEL ARCHIVO: Filmina_03_Pinout_y_conexionado.md =====

# Filmina 3 · Pinout y conexionado

**Mensaje clave:** cada señal del auto tiene un pin documentado; el Mega concentra motores, Hall, sensores y panel, y habla con el
ESP por USART1 (D18/D19). Los agregados propios están identificados.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Pinout y conexionado (Mega 2560 + shield OSOYOO)» │
├────────────────────────────────┬─────────────────────────────────────────┤
│ A  TABLA DE PINOUT             │ B  DIAGRAMA D-02 (conexionado)          │
│ c1–c6 · f2–f6                  │ c7–c12 · f2–f4                          │
│ 3 bloques: Potencia/Motores,   ├───────────────────┬─────────────────────┤
│ Hall+Sensores, Panel+ESP+NPN   │ C  FOTO-12        │ D  FOTO-14 / FOTO-15│
│                                │ pinout shield     │ foto real cableado  │
│                                │ c7–c9 · f5–f6     │ c10–c12 · f5–f6     │
└────────────────────────────────┴───────────────────┴─────────────────────┘
```

## Contenido por zona

### A · Tabla de pinout (3 bloques; resaltar en carmesí lo agregado por nosotros)

**Bloque 1 · Motores (IN1 · IN2 · PWM)**

| Rueda | Salida del puente H | IN1 | IN2 | PWM |
| --- | --- | --- | --- | --- |
| DEL DER (FR) | M_B · BK1-BK2 | D22 | D24 | D9 |
| DEL IZQ (FL) | M_B · BK3-BK4 | D26 | D28 | D10 |
| TRAS DER (RR) | M_A · AK1-AK2 | D5 | D6 | D11 |
| TRAS IZQ (RL) | M_A · AK3-AK4 | D7 | D8 | D12 |

**Bloque 2 · Hall (x1: flanco ↑ de A, B da el sentido) y sensores**

| Elemento | Pines |
| --- | --- |
| Hall A (S1, naranja) DER-DEL / IZQ-DEL / DER-TRAS / IZQ-TRAS | A8 / A9 / A10 / A11 |
| Hall B (S2, verde) en el mismo orden | A12 / A13 / A14 / A15 |
| Barra IR ×5 (IR1…IR5) | A4 / A3 / A2 / A1 / A0 |
| HC-SR04 TRIG / ECHO | D30 / D31 |
| Servo del radar (señal) | D13 |
| Batería (divisor 10 kΩ/10 kΩ + 100 nF) ⭐ | A5 |

**Bloque 3 · Enlace y agregados propios** ⭐

| Elemento | Pines |
| --- | --- |
| UART al ESP (a E_RX / E_TX del selector) | D18 (TX1) · D19 (RX1) |
| OLED I2C ⭐ | SDA D20 · SCL D21 |
| Encoder ⭐ | CLK D2 (INT4) · DT D3 (INT5) · SW D42 |
| KEY / RST automáticos (vía NPN) ⭐ | D32 → KEY · D33 → RST |
| USB (carga/depuración) | D0 / D1 (no es el enlace del ESP) |

Leyenda: ⭐ = agregado propio sobre el kit OSOYOO. Nota al pie: *DEL/TRAS e IZQ/DER mirando en el sentido de avance.
Orden físico de los headers Hall: FR, FL, RR, RL; orden lógico del firmware: FL, FR, RL, RR.*

### B · D-02 ([D-02_conexionado.svg](diagramas/D-02_conexionado.svg))
Mega al centro; a la izquierda enlaces con el ESP, la batería y los NPN; a la derecha motores, Hall, IR, ultrasonido, OLED y
encoder. Sobre el diagrama, tres notas breves:
1. Alimentación: Hall a **+5 V regulados** (nunca la batería) · GND común.
2. Potencia: portabaterías → Model Y VIN → VOUT → POWER del shield.
3. ESP: usar E_RX/E_TX del selector (el puerto ESP UART directo es de 3,3 V).

### C · FOTO-12 / FOTO-13
**FOTO-12** pinout oficial del shield OSOYOO V1.3 (arriba/abajo). Pie: *Fuente: OSOYOO; el mapa ilustrado es de UNO, no usar A4/A5
como I2C del Mega.* **FOTO-13** (mapa de pines Wi-Fi) como alternativa o apoyo.

### D · FOTO-14 / FOTO-15 (por tomar ✱)
Foto real del Mega + shield visto desde arriba con cables identificados; foto de motores y Hall cableados. Detalle en [FOTOS.md](FOTOS.md).
**FOTO-16**: exportar la primera página de `Documentacion/PINOUT_DEL_AUTO.pdf` si se prefiere mostrar la hoja oficial.

## Guion del orador (≈ 60 s)

«El Mega gobierna cuatro motores con dos pines de dirección y uno de PWM cada uno; los cuatro encoders Hall entran por el
puerto K y generan interrupción por cambio de A, mientras B da el sentido. A eso sumamos la barra de cinco IR, el ultrasonido con
su servo de radar y el divisor de batería en A5. El shield Wi-Fi se une por D18 y D19. Los agregados propios son el panel OLED con
encoder, la medición de batería y los dos transistores que simulan los botones KEY y RESET del ESP. Todo con masa común y los Hall
a cinco volts regulados.»

## Fuentes

`AutoFinalRobo2-Contracts/hardware/PINOUT.md` · `PINOUT_DEL_AUTO.json` · `INFORME_PINOUT_ROBOT_20261106.md` ·
`WIFI_SHIELD.md` · `INTERRUPCIONES_ENCODER_HALL.md` · `AutoFinalRobo2-Atmega/include/board_pinmap.h`.

## Advertencias

- Pines = **implementación en código y montaje comunicado**; no hay verificación eléctrica documentada de todos.
- El CAD de la PCB del panel conserva +3V3; el montaje real se informó a **5 V** (ver filmina 13).
- Calibración de Hall y signos: ver filmina 5; no afirmar que está «calibrado» sin medición.


===== FIN DEL ARCHIVO: Filmina_03_Pinout_y_conexionado.md =====

===== INICIO DEL ARCHIVO: Filmina_04_Firmwares_y_ciclo.md =====

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


===== FIN DEL ARCHIVO: Filmina_04_Firmwares_y_ciclo.md =====