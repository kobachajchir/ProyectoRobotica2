# Autoría y reproducción de la iteración v09

La fuente de esta revisión es [`revise_J1_v09.py`](../revise_J1_v09.py), que reutiliza los auxiliares de autoría versionados de las revisiones anteriores. El maestro nativo v08 se conservó y se trabajó sobre una copia v09. No se incorporó un CAD de fabricante como si sus interfaces fueran cotas verificadas.

El ensamble F3D y los componentes F3D independientes son la entrega editable para abrir en otra PC. La fuente de automatización contiene rutas del workspace `D:/ProyectoRobotica2`; para regenerarla en otro directorio hay que resolver esas referencias de forma coherente. No es necesario ejecutar los scripts para abrir o editar los parámetros nativos del F3D.

Las fases son mutaciones del documento: **no deben repetirse** sobre una copia ya terminada. La secuencia realizada, registrada parcialmente en `authoring_journal.jsonl` y `progress.jsonl`, fue:

1. Copia de v08 mediante Guardar como; `parameters`, `motor`, `structure`, `structure1`, `structure2`, `structure3`, `bearings`, `cap`, `shaft`.
2. `split_cart`, `cart_seats`, `motor_fasteners`; `flange` con `FLANGE_START=0` y después2; `platform`, `horn`, `adapter`.
3. `cart_bolts`, cuatro ejecuciones con `BOLT_START=0/1/2/3`; `shaft_fit`; `hardware_finish` con `HARDWARE_START=0/2`; `plate_access`; `limiters` con `LIMITER_START=0/2`.
4. Asignación PLA, reubicación de `bornera`, `labels`, apertura pasante `horn_clearance`, soporte `XL_clearance` y ajuste `XL_spacing`. La fase final PLA propaga el material a los cuerpos nuevos. Las asignaciones de material y parámetros pueden actualizarse sin recrear las fases geométricas.
5. Revisión geométrica y cálculos; `save` guarda y exporta el maestro, los componentes seleccionados y los parámetros. El estado guardado se registra en `modelo_v09.json`.

Los hallazgos previos a las correcciones no son resultados de aceptación: `interferencias_iteracion_inicial.json` y `interferencias_membrana_cartucho.json` conservan evidencia de por qué se corrigieron bornera y cartucho. `montajes_internos.json` y `informe_movimiento_v09.json` corresponden a la revisión final indicada en el informe.

El cálculo externo tiene este orden: `extraer_v09.py` y `extraer_secciones_v09.py` en Fusion; `calcular_v09.py`, `resistencia_v09.py` y, una vez finalizado el muestreo CAD de cargas, `cerrar_informe_v09.py`. El script de equivalencia de inercias contrasta la suma por cuerpo con la agregación rígida mediante el teorema de ejes paralelos; no es una comprobación física.

La geometría de servos, estriado, roscas, anillo y tuercas sigue siendo una referencia simplificada. Los dientes existentes son de representación. Esta autoría no libera interfaces comerciales inferidas ni piezas de fabricación.
