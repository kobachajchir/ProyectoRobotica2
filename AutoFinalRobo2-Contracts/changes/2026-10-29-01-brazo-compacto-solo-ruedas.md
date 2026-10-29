# AFR2-20261029-01 — Reducir masa de actuadores, eslabones y pinza

29/10/2026. [Informe explicativo](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v14/README.md) · [BOM, CG, inercias y cargas](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v14/compacto_resultados.json).

Se incorporan las decisiones humanas: importacion con envio a Argentina admitida, trabajo exclusivamente sobre ruedas, reduccion de dimensiones y masa. Los pies de v13 y primeras pruebas v14 quedan descartados. No se adopta el voladizo sobre ATmega como aligerado ni se agrega lastre.

Se descartan como direccion los montajes pesados. Nuevo presupuesto con placas plegadas de aluminio delgadas, PLA PrintaLot localizado, PVC25/20 propuesto, ejes de acero y pinza pivotante pendiente de dimensionar. Tres ST-3215-C047 y cuatro STS3032 suman254.4g frente a375.2g previos. La oferta argentina STS3215 no confirma sufijoC047; envio argentinoSTS3032 pendiente. No hay seleccion/compras cerradas.

Se mantienen200g, seis ejes30grados/s y frenada finita50ms. Cuatro presupuestos,684trayectorias/14364estados por alternativa y rotacionJ1 para vuelco. Eslabones80/70:1059.4g sin carga,1190.6g con reserva estructural25%; margen externo minimo2.50 con reserva, pero apoyo hipotetico180x100 insuficiente. Eslabones55/50:1170.3g sin carga con reserva, alcance215mm, margen externo minimo2.65 y utilizacion de vuelco.928 sobre ese mismo rectangulo hipotetico. Los tramosPVC15/10mm cuestionan su conveniencia constructiva.

No se declara que el brazo resista fisicamente. Los esfuerzos incluyen gravedad, aceleracion e inercia de cuerpos, resultantes de corte/axial/flexion/torsion por seccion; faltan inercia interna, curva de frenado, resistencia de nuevas uniones, PLA, transmision/pinza, contactos/CG reales y choques de geometria nueva. La recta par-velocidad de accionamiento no acredita frenado. El caso extendido no envuelve cualquier trayectoria. Auto1.5kg aproximado, detenido y centrado, no frenada del auto.

| Modulo | Estado |
|---|---|
| CAD | Calculos mecanicos solicitados y presupuesto nuevo; Fusion v09 conservado, sin nuevo housing, barrido CAD, FEA o ensayo fisico |
| atmega/esp/web | Sin cambios de fuente, builds ni flasheos |
| Loader | Sin intervencion |
| Contracts | Ficha y enlaces; antecedentes preservados |

No se ejecutan tests de software ni compilaciones. La nueva arquitectura exige TTL y modo multivuelta para reducciones; firmware y gestion de referencia al encender pendientes. El commit publica fuentes reproducibles y resultados de predimensionamiento, no un CAD terminado ni la aceptacion del objetivo.
