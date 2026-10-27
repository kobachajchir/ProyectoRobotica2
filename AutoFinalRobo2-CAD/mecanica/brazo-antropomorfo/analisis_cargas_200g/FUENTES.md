# Fuentes y estado de los datos

Consulta del 27/10/2026. Los datos de catálogo no sustituyen el pesaje, la identificación de la variante adquirida ni sus curvas de servicio continuo.

| Dato | Fuente | Uso y límite |
| --- | --- | --- |
| Volumen y centroide de 264 cuerpos | Fusion, maestro `AFR2_Brazo_PVC32_AGV_v06` y `geometria_v06.json` | Propiedades geométricas nativas, cálculo High; materiales físicos originales no usados para masa |
| Posición y dimensiones del brazo | Parámetros y uniones nativas del mismo maestro; copia horizontal de estudio | Paridad de transformaciones cotejada en `resultados_estatica.json`; mm/cm convertidos explícitamente |
| PETG 1,27 g/cm³ | [PrintaLot, PETG 1,75 mm](https://printalot.com.ar/productos/filamentos/pet-g/petg-1-75-mm-1-kg/) | Densidad del filamento macizo; sin propiedades resistentes de la impresión concreta |
| PVC-U 1,42–1,48 g/cm³, flexión E=2,7–3,0 GPa, resistencia típica 52 MPa | [Vinidex, propiedades de PVC](https://www.vinidex.com.au/resources/technical-resources/material-properties/pvc-properties/) | Material de tubería de referencia a temperatura moderada y corta duración; el caño argentino no se identificó por fabricante/formulación |
| MG946R: 55 g, bloqueo 10,5/13 kgf·cm a 4,8/6 V | [TowerPro MG946R](https://towerpro.com.tw/product/mg946r/) | Se usa masa de la especificación técnica, no los 0,08 kg del campo comercial de información adicional |
| MG90S: 13,4 g, bloqueo 1,8 kgf·cm a 4,8 V | [TowerPro MG90S](https://towerpro.com.tw/product/mg90s-3/) | No se interpreta «9g» de la familia como masa del servo metálico; variante/clon adquirido pendiente |
| DS3218: 60 g y bloqueo 18/21,5 kgf·cm a 5/6,8 V | [DSSERVO, datasheet](https://www.dsservo.com/d_file/DS3218%20datasheet.pdf) | Tablas del fabricante recuperadas por su índice web; la descarga directa respondió con timeout. No hay curva de par continuo |
| 608-2Z: 12,5 g y dimensiones 8×22×7 mm | [Ficha de SKF distribuida por RS](https://docs.rs-online.com/8f0c/0900766b8151f2a7.pdf) | Masa de rodamiento completo, no la de aros macizos dibujados; capacidad estática 1,4 kN no certifica el alojamiento PETG |
| 625-2Z: 5 g y dimensiones 5×16×5 mm | [SKF, catálogo propio](https://www.emarketplace.in.skf.com/deep-groove-ball-bearing/625-2z) | Masa completa de catálogo |
| GX3240MGV1, anuncio hasta 40 kgf·cm | [Starware, producto de marca propia](https://tienda.starware.com.ar/producto/servo-motor-digital-starware-gx3240mgv1-40k-5-74v-270g-metal-alto-torque/) y [referencia GXSERVO](https://www.gxservo.com/pd.jsp?id=16) | La página GXSERVO no devolvió ficha accesible. La publicación local indica 0,1 kg junto a dimensiones de bulto: **no se acepta como masa neta medida**. Se adopta 100 g conservador y pendiente de pesaje, y el torque se trata como anuncio, sin validación de variante |
| Eficiencia hombro 0,85, J4 0,90 y pinza 0,80 | Supuestos de cálculo | No medidos. El presupuesto separa par ideal de salida y demanda del servo con pérdidas |
| Acero 7,85; latón 8,5; nylon 1,14; goma 1,10 g/cm³ | Supuestos de materiales genéricos | Sin grado/proveedor identificados; no se asignan límites resistentes a partir de ellos |
| Módulos y cables | Masas de ingeniería supuestas: tres XL4016 de 50 g, Nano 7 g, PCA 12 g, bornera 8 g; escenario adicional 30 g de cables | Componentes fijos fuera del par distal del hombro; cables tratados conservadoramente en el TCP |
| Fricción de agarre 0,2/0,3/0,5 y margen de selección 2 | Hipótesis de sensibilidad/diseño | No son coeficientes medidos ni una prescripción normativa |

La carga máxima solicitada aquí es **200 g**, frente a los 100 g del nombre histórico del componente. No se cambia el volumen del cubo ni se obtiene su peso con la densidad Steel que tenía asignada.

El campo `basis` del CSV por cuerpo identifica el origen de cada masa. Las partes de un motor o un rodamiento reciben fracciones de una única masa de catálogo; no se suma esa masa y además la de su envolvente. Horns y tornillos se presupuestan por separado como accesorios del mecanismo; si el peso de un kit los incluye, se deberá ajustar el presupuesto al pesarlo.
