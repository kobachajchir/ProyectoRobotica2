# AFR2-20261101-10 — Masa, vuelco y traccion del conjunto actual

Pedido humano: calcular con el brazo actual, carcasa/actuadores finales, carga200g y auto aproximadamente1,5kg, sobre sus ruedas. Este pedido habilita los calculos que se habian diferido; no implica modificar el CAD o autorizar ensayos.

Entrega: [informe explicativo](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/analisis_conjunto_20261101/INFORME.md), captura nativa nueva, asignacion de masas por cuerpo, scripts, resultados JSON y figura de planta.

- Brazo/montura/control2,345kg como presupuesto de volumen PLA macizo, masas comerciales y estimaciones. Total conauto/carga4,045kg. No masa fisica pesada ni perfil de infill.
- Apoyo nominal214x124mm entre centros inferidos del CAD, no huella medida de rodillos. Auto centrado horizontalmente; altura CG40mm supuesta.
- Brazo frontal horizontal:FS de momentos1,023, margen geometrico0,85mm. No alcanza1,2; ejemplos de frenada del auto yJ2 pierden apoyo en el modelo. No se aprueba trabajo cargado extendido en cualquier direccion.
- GM25-370:0,2942Nm nominales a9V por motor, cotejados con ficha oficialOSOYOO. Filtro de avance en llano con50% de fuerza utilizable hipotetica da margen2,84 aCrr0,10 y0,3m/s2. No acredita adherencia, maniobras laterales/diagonales, tension/driver ni operacion fisica.
-43.798 poses estaticas muestreadas y ejemplos de frenada finita30grados/s. No envolvente global, FEA, resistencia FDM, nueva revision de choques o pruebas fisicas.

Estados: CAD leido porMCP sin modificar; nuevos artefactos de calculo/documentacion. atmega/esp/web/Loader sin cambios por esta ficha. Sin compilaciones, tests de software, flasheos, COM o ensayos fisicos. Se conservan los cambios ajenos ya presentes en el arbol de trabajo.
