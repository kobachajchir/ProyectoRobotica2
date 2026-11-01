# Masa de las piezas impresas: PLA PrintALot blanco

AFR2-20261031-03. Solo masa geometrica de los impresos del brazo abierto, sin modificarlo.

**Actual: 788.9 g**, en 636.22 cm3 de material modelado; 32 piezas impresas.

| Referencia | Masa PLA maciza | Ahorro actual |
|---|---:|---:|
| Ultima medicion guardada, antes del cierre J1/J4/J5 | 879.7 g | 90.8 g (10.3 %) |
| 3D terminado antes de las optimizaciones manuales, archivo6041204 | 906.8 g | 117.9 g (13.0 %) |
| Modelo abierto actual | 788.9 g | — |

La segunda referencia se mide ahora desde el F3D terminado anterior: no era la ultima medicion guardada. Permite separar los cambios de los nuevos housings de las optimizaciones manuales posteriores. Se abrio una copia temporal y se restituyo el documento actual sin guardarlo ni cambiar su geometria.

## Por pieza: antes de optimizar frente a actual

| Componente | Anterior terminado (g) | Actual (g) | Ahorro (g) |
|---|---:|---:|---:|
| A01_Plataforma_giratoria_PETG | 213.66 | 161.90 | 51.76 |
| V02_A20_Bastidor_pinza | 60.96 | 41.52 | 19.43 |
| A12_Encastre_distal_antebrazo_PETG | 58.52 | 45.41 | 13.11 |
| V05_Montura_baja_PETG | 136.77 | 125.38 | 11.39 |
| V17_Housing_STS3250_y_apoyo625 | 55.00 | 48.24 | 6.76 |
| V17_Tapa_housing_STS3250 | 15.22 | 8.82 | 6.40 |
| V17_Tapa_STS3250_J5 | 10.34 | 6.16 | 4.18 |
| V06_Contraplaca_inferior_unica_PETG | 24.02 | 20.00 | 4.01 |
| V17_Tapa_STS3250_J4 | 7.01 | 5.14 | 1.88 |
| V09_Cartucho6000_desmontable_PETG | 32.34 | 32.34 | 0.00 |
| V02_Pinon24_Pinza | 2.04 | 2.04 | 0.00 |
| V03_Tapa_608_A06_Encastre_distal_brazo_PETG_1 | 5.21 | 5.21 | 0.00 |
| V03_Tapa_625_J6 | 2.34 | 2.34 | 0.00 |
| A11_Encastre_proximal_antebrazo_PETG | 30.63 | 30.63 | 0.00 |
| V03_Tapa_625_V02_A15_Horquilla_J5_PETG_1 | 3.54 | 3.54 | 0.00 |
| V03_Tapa_608_A01_Plataforma_giratoria_PETG_1 | 5.21 | 5.21 | 0.00 |
| V03_Tapa_608_A06_Encastre_distal_brazo_PETG_-1 | 5.21 | 5.21 | 0.00 |
| A05_Encastre_proximal_brazo_PETG | 53.51 | 53.51 | 0.00 |
| V03_Tapa_625_V02_A15_Horquilla_J5_PETG_-1 | 3.54 | 3.54 | 0.00 |
| A06_Encastre_distal_brazo_PETG | 30.19 | 30.19 | 0.00 |
| V03_Tapa_608_A01_Plataforma_giratoria_PETG_-1 | 5.21 | 5.21 | 0.00 |
| V17_Tapa_STS3032_J6 | 2.51 | 2.51 | 0.00 |
| V03_Tapa_608_J4 | 5.21 | 5.21 | 0.00 |
| V03_Tapa_rodamientos_J1 | 2.20 | 2.20 | 0.00 |
| V02_Pinon24_J4 | 4.56 | 4.56 | 0.00 |
| V02_A18_Nucleo_J5_y_columna_J6 | 40.18 | 40.18 | 0.00 |
| V17_Tapa_STS3032_J7 | 2.51 | 2.51 | 0.00 |
| V02_Mandibula_-1 | 8.53 | 8.53 | 0.00 |
| V02_Mandibula_1 | 8.53 | 8.53 | 0.00 |
| V02_A15_Horquilla_J5_PETG | 72.06 | 73.13 | -1.07 |

## Metodo y limites

Masa = volumen neto del solido CAD en cm3 x1.24g/cm3. Se cuentan las ocurrencias, no solo definiciones compartidas. Se incluyen los vaciados, ventanas y agujeros modelados. La apariencia blanca no altera la densidad ni convierte otras piezas en PLA.

Densidad tomada de PrintALot_PLA_TDS.pdf, tabla Densidad/ASTMD792:1.24g/cm3. Copia local verificada por SHA256 80aa99ebd931b2533735f68aade3b886916bf8f4a4b5975b61df2f484a618e08; procedencia en revision_v09/README.md.

Es una estimacion maciza: no es el consumo de filamento del laminador ni un pesaje fisico. Paredes, relleno, tapas, soportes, brim y desperdicio aun sin definir. No calcular la masa real multiplicando este total por el porcentaje de relleno: las paredes y tapas tambien consumen material.

No incluye caños PVC, metal, motores, rodamientos, electronica ni pieza transportada. No se recalculan esfuerzos, pares, vuelco ni resistencia. No se usan propiedades fisicas nativas que quedaron pendientes de sincronizar; se aplica la misma densidad externamente a las tres referencias.

La reduccion del GLB a596.484bytes es independiente de esta reduccion fisica de material. Se mide BRep deFusion; no el volumen de las mallas simplificadas deBlender.

Evidencia: pesos_impresion_20261031.json, pesos_impresion_pre_optimizacion_6041204.json, comparacion_pesos_impresion.json y masas_impresos_por_pieza.csv.
