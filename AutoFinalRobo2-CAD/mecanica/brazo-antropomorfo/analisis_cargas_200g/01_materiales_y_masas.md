# 1. Materiales, masas y centros de masa

Maestro geométrico v06, 264 cuerpos extraídos con `HighCalculationAccuracy`. Los colores del CAD no eran materiales físicos asignados: muchas piezas figuraban como Steel. Por eso no se usa la masa automática original de Fusion. Se extraen volumen y centroide y se asignan densidades o masas de catálogo fuera del CAD. [Geometría](geometria_v06.json), [cálculo](calcular_estatica.py).

El modelo analítico se cotejó con las transformaciones nativas de la copia horizontal: error máximo menor a 10⁻⁹ mm. Se usan centros geométricos uniformes dentro de cada sólido; en servos/módulos se reparte su masa total entre las envolventes según volumen. Su distribución interna real y la de cables no están identificadas.

## Propiedades y supuestos

- PETG PrintaLot: densidad 1,27 g/cm³, [fabricante](https://printalot.com.ar/productos/filamentos/pet-g/petg-1-75-mm-1-kg/). Se utiliza densidad del polímero sólido; no se inventa un módulo o resistencia de la pieza impresa.
- PVC-U de referencia: densidad 1,42–1,48 g/cm³; se adopta 1,45. Módulo de flexión de corta duración 2,7–3,0 GPa y resistencia última típica 52 MPa, [Vinidex](https://www.vinidex.com.au/resources/technical-resources/material-properties/pvc-properties/). El tubo argentino real, su formulación y su estado deben confirmarse. No se traslada la resistencia a presión a una unión impresa.
- Acero 7,85 g/cm³, latón 8,5, nylon de horns 1,14 y goma 1,10: valores de cálculo supuestos. Sin marca/grado de metal ni polímero de los horns identificados. Tornillos y ejes son sólidos CAD simplificados, sin corrección por hélices de rosca.
- Rodamientos completos: 608-2Z = 12,5 g, [ficha SKF](https://docs.rs-online.com/8f0c/0900766b8151f2a7.pdf); 625-2Z = 5 g, [SKF](https://www.emarketplace.in.skf.com/deep-groove-ball-bearing/625-2z). Se reemplaza la masa de los aros simplificados por la masa de catálogo: 8×608 + 4×625 = 120 g. Las tapas que llevan 608/625 en el nombre siguen siendo PETG, no rodamientos.
- Servos: MG946R 55 g ([TowerPro](https://towerpro.com.tw/product/mg946r/)), MG90S 13,4 g ([TowerPro](https://towerpro.com.tw/product/mg90s-3/)), DS3218 60 g ([DSSERVO](https://www.dsservo.com/d_file/DS3218%20datasheet.pdf)). GX3240: **100 g supuesto conservador**, intervalo de sensibilidad 80–100 g; no se obtuvo una masa neta de fabricante verificable. La publicación local informa 0,1 kg junto a dimensiones de bulto y no se toma como una medición del servo. Esta masa está en la torre J1, no en el eslabón que gira sobre J2.
- Electrónica fija, a pesar: XL4016 50 g cada uno, Nano 7 g, PCA9685 12 g y bornera 8 g. Total 177 g. Se usa el centro geométrico del módulo simplificado. Los disipadores no se computan como acero además de sumar la masa del módulo.
- Carga útil 200 g, fuerza **1,96133 N**. El nombre histórico del componente dice 100g, pero la copia de análisis usa 200 g y la masa en el cálculo se impone directamente. Se conservan las dimensiones 50×50×50 mm.

## Cuánto plástico hay

Las 25 piezas rojas suman **823.13 cm³**. Macizas equivalen a **1045.4 g de PETG**. Es una cota de peso propio conservadora de este CAD, no la predicción del laminador.

Un 40 % de relleno no produce una pieza del 40 % de masa: perímetros, tapas, nervios y regiones de poco espesor pueden quedar casi macizos. Se comparan fracciones **efectivas** de masa 100 %, 70 % y 50 %, conservando el centroide por hipótesis. La masa y el centro reales se deberán reemplazar por los del G-code o por pesaje. Los escenarios de fracción uniforme no acreditan resistencia equivalente ni permiten dividir el límite elástico por ese porcentaje.

| Pieza PETG | Volumen (cm³) | Masa maciza (g) |
| --- | --- | --- |
| A01_Plataforma_giratoria_PETG | 207.00 | 262.9 |
| V03_Tapa_608_A01_Plataforma_giratoria_PETG_-1 | 4.20 | 5.3 |
| V03_Tapa_608_A01_Plataforma_giratoria_PETG_1 | 4.20 | 5.3 |
| A05_Encastre_proximal_brazo_PETG | 65.23 | 82.8 |
| A06_Encastre_distal_brazo_PETG | 80.29 | 102.0 |
| V02_Rueda40_Hombro | 16.40 | 20.8 |
| V03_Tapa_608_A06_Encastre_distal_brazo_PETG_-1 | 4.20 | 5.3 |
| V03_Tapa_608_A06_Encastre_distal_brazo_PETG_1 | 4.20 | 5.3 |
| A11_Encastre_proximal_antebrazo_PETG | 60.86 | 77.3 |
| A12_Encastre_distal_antebrazo_PETG | 40.59 | 51.5 |
| V03_Tapa_608_J4 | 4.20 | 5.3 |
| V02_Pinon20_Hombro | 3.77 | 4.8 |
| V02_A15_Horquilla_J5_PETG | 73.80 | 93.7 |
| V03_Tapa_625_V02_A15_Horquilla_J5_PETG_-1 | 2.86 | 3.6 |
| V03_Tapa_625_V02_A15_Horquilla_J5_PETG_1 | 2.86 | 3.6 |
| V02_A18_Nucleo_J5_y_columna_J6 | 30.57 | 38.8 |
| V03_Tapa_625_J6 | 1.89 | 2.4 |
| V02_A20_Bastidor_pinza | 49.24 | 62.5 |
| V02_Mandibula_-1 | 6.88 | 8.7 |
| V02_Mandibula_1 | 6.88 | 8.7 |
| V02_Pinon24_Pinza | 1.64 | 2.1 |
| V02_Pinon24_J4 | 3.84 | 4.9 |
| V03_Tapa_rodamientos_J1 | 4.69 | 6.0 |
| V05_Montura_baja_PETG | 123.47 | 156.8 |
| V06_Contraplaca_inferior_unica_PETG | 19.37 | 24.6 |

## Presupuesto de masas

| Categoría | Masa (g) |
| --- | --- |
| PETG impreso | 1045.4 |
| Rodamiento | 120.0 |
| Servo | 310.2 |
| Laton | 7.5 |
| Acero | 323.3 |
| Horn polimero | 4.9 |
| PVC | 38.1 |
| Carga util | 200.0 |
| Almohadilla | 2.4 |
| Electronica fija estimada | 177.0 |

Total brazo, montaje, electrónica añadida y carga: **2.229 kg**, peso **21.86 N**. Sin los 200 g: **2.029 kg**. No incluye el auto base ni su batería. Masa giratoria sostenida por J1, incluida carga: **1.686 kg**. Masa a partir del hombro J2, incluida carga: **1.245 kg**.

## Centros de masa por conjunto

Coordenadas globales: X transversal, Y hacia el HC-SR04 frontal, Z vertical; origen del CAD del brazo, cara superior del chasis en Z=0. Hombro en (0, −14, 209) mm. Se define `s = Y + 14` como distancia longitudinal desde el hombro, solo para esta postura de avance frontal.

| Conjunto | Masa total (g) | PETG macizo (g) | CG global X,Y,Z (mm) | s (mm) |
| --- | --- | --- | --- | --- |
| Torre y giro J1 | 432.5 | 273.6 | (-21.05, -10.94, 147.63) | 3.1 |
| Eslabon 1 + motor del codo | 389.5 | 216.3 | (-19.41, 56.32, 206.04) | 70.3 |
| Eslabon 2 + motor J4 | 254.4 | 134.2 | (0.14, 164.71, 204.97) | 178.7 |
| Pinon y horn del hombro | 8.4 | 4.8 | (-41.36, -14.00, 164.00) | -0.0 |
| Carga util 200 g | 200.0 | 0.0 | (0.00, 444.00, 209.00) | 458.0 |
| Giro J4 + horquilla/motor J5 | 193.5 | 101.0 | (-30.58, 277.83, 203.52) | 291.8 |
| Nucleo J5 + motor J6 | 88.3 | 41.2 | (-3.33, 312.36, 207.60) | 326.4 |
| Palma + servo pinza | 88.6 | 62.5 | (0.03, 381.58, 207.72) | 395.6 |
| Mordaza izquierda | 9.9 | 8.7 | (-5.92, 421.74, 234.21) | 435.7 |
| Mordaza derecha | 9.9 | 8.7 | (5.92, 421.74, 183.79) | 435.7 |
| Pinon y horn de pinza | 4.0 | 2.1 | (-0.00, 401.04, 209.00) | 415.0 |
| Pinon y horn de J4 | 6.8 | 4.9 | (-0.00, 243.44, 173.00) | 257.4 |
| Montura/control fijo | 543.0 | 187.4 | (-11.55, -11.74, 35.69) | 2.3 |


Los conjuntos incluyen motores que están fijados a ellos: el DS3218 viaja con el eslabón 1, el MG90S J4 con el antebrazo y el MG946R J5 con la horquilla giratoria J4. No se coloca toda la masa de motor en la punta ni se la suma dos veces. CSV por cada cuerpo, con volumen, masa, CG y fuente: [masas_por_cuerpo.csv](masas_por_cuerpo.csv).

## Ubicación de los motores

| Componente | Masa adoptada (g) | CG global (mm) | s (mm) |
| --- | --- | --- | --- |
| V03_M01_MG946_Base | 55 | (10.33, -14.00, 42.84) | -0.0 |
| V02_M02_GX3240_270 | 100 | (-69.01, -4.03, 164.00) | 10.0 |
| V02_M03_DS3218_Codo | 60 | (-66.01, 116.00, 199.03) | 130.0 |
| V02_M04_MG90S_J4 | 13.4 | (0.00, 222.04, 167.14) | 236.0 |
| V02_M05_MG946_J5 | 55 | (-59.20, 291.00, 198.67) | 305.0 |
| V02_M06_MG90S_J6 | 13.4 | (0.00, 319.54, 203.14) | 333.5 |
| V02_M07_MG90S_Pinza | 13.4 | (0.00, 380.04, 203.14) | 394.0 |


Los CG internos reales de los motores no están publicados; esta primera aproximación usa la carcasa/salida modeladas. Una incertidumbre de ±5 mm en la posición longitudinal del CG de todos los servos transportados produce una variación adicional pequeña frente a la del porcentaje efectivo de plástico, pero debe actualizarse con componentes reales.
