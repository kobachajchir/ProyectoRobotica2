# AFR2-20261027-18 — Informe integrado de esfuerzos e iteración del brazo

27/10/2026. El usuario pide integrar las razones de las correcciones y los esfuerzos del brazo en un informe explicativo para conservar la justificación de cada iteración. Se incorporan sus pedidos de revisar actuadores argentinos, frenada de emergencia y cargas combinadas en base/primer eslabón.

## Antes/después

Los fundamentos estaban distribuidos entre versiones de CAD, el análisis estático de 200 g y la conversación de planificación. Se agrega un [índice integrado](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/INFORME_ITERACION_DISENO.md), con tres capítulos explicativos nuevos y enlaces a los tres capítulos de cálculo originales.

- Evolución v01–v06: problema, corrección, razón mecánica y límite de la evidencia. La solución elevada v04 figura descartada; la montura baja v05 y la contraplaca única v06 son antecedentes vigentes.
- Se conservan los resultados estáticos existentes y sus supuestos de masa; no se llaman resultados de frenada ni se recalculan.
- Dinámica: parada finita, sensibilidad propuesta de 200/100/50 ms, perfil ilustrativo, resultantes completas y traslado entre secciones. Sin trayectoria ni prestaciones reales acreditadas.
- Base, hombro, encastres del primer eslabón, anclajes y acrílico se priorizan por carga combinada. Se distinguen cortantes, axial, torsión y flexión en dos planos.
- Plan v07: selección por eje, masa/inercia y alimentación, modelado previo del motor con cotas trazables y rediseño de articulación. GX3240MGV1 y DYM BLS-HV70 son candidatos de oferta local, no elección definitiva.

## Interfaces y alcance

No se cambian IDs, protocolos, pinout, firmware, perfil Web ni parámetros nativos de Fusion. PWM sigue siendo consigna y no lectura física de posición. Los despieces permanecen diferidos.

Se amplía la documentación técnica de CAD y sus fuentes; se enlaza desde CAD, brazo, índice de cálculo, informe maestro y Contracts. Los datos/CSV/JSON, fuentes de cálculo y F3D existentes se conservan. No se ejecuta ningún script de cálculo o comprobación.

| Módulo | Estado de esta entrega |
| --- | --- |
| CAD | Informe y plan documentados; geometría v06 sin modificar, v07 pendiente |
| Contracts | Ficha, índice y changelog actualizados |
| Atmega | Sin cambios |
| ESP | Sin cambios ni promoción |
| Web | Sin cambios; no se acredita modelo con capacidad de 200 g |
| Loader | Sin cambios |

Solo integración documental y consulta de fuentes. Sin nuevos cálculos numéricos, dinámica ejecutada, FEA, modelado, tests, compilaciones, validadores, flasheo, impresión ni ensayo físico. Los 200 g y la frenada son condiciones de diseño por resolver.
