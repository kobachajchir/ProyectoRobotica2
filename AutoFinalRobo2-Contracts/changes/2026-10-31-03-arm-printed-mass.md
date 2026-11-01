# AFR2-20261031-03 - Masa de impresos despues de optimizar geometria

Pedido expreso: medir los impresos PLA del brazo abierto y compararlos
con la ultima medicion y las optimizaciones manuales posteriores.

Se suman volumenes BRep activos por ocurrencia y se aplica densidad
PrintALotPLA1.24g/cm3, igual a la base historica. Sin motores, PVC, metales,
rodamientos, electronica, auto ni carga. No se usa el GLB simplificado.

| Referencia | Masa PLA maciza |
|---|---:|
| Ultima medicion guardada previa al cierre J1/J4/J5 |879.7g|
| Archivo3D terminado6041204, medido ahora |906.8g|
| Documento actual con optimizaciones manuales |788.9g|

Actual:32piezas,636.215cm3. Ahorro neto frente a ultima medicion90.8g,
10.3%. Frente al modelo terminado equivalente:117.9g,13.0%.
Principales descensos: torre51.8g, bastidor de pinza19.4g, encastre distal
de antebrazo13.1g y montura baja11.4g. El documento actual fue restituido
tras leer una copia temporal del archivo anterior, sin guardar cambios.

Estimacion de piezas macizas, no consumo del laminador ni pesaje fisico:
perfilFDM, paredes, relleno y soportes siguen sin definir. No se recalculan
pares, esfuerzos, estabilidad ni resistencia, y no se modifica geometria.
No tests, builds, navegador, flash ni promocionESP.

[Informe y desglose por pieza](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco/MASA_IMPRESION_PLA.md).
Fuentes, hashes, JSON y CSV quedan junto al informe. Cambio exclusivamente
CAD/documentacion; atmega/esp/web/Loader sin cambios de esta revision.
