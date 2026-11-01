# AFR2-20261028-09 - Brazo liviano: aluminio, reducciones y traccion

28/10/2026. [Informe](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v13/README.md) · [Resultados](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v13/RESUMEN_NUMERICO.md). Objetivo activo, sin declarar cumplimiento fisico.

El usuario informa auto de aproximadamente1.5kg y autoriza aluminio, PVC menor, ventanas y simplificacion. Se descarta continuar la opcion pesadaRS01. Fusion v09 sigue guardado/sin modificar; no se crean housings antes de cerrar la preseleccion.

Se estudian siete presupuestos con placas/bridas de aluminio, ejes de acero, PVC25/20 y PLA PrintaLot localizado. La pinza conserva cuerpos/CG/tensores v09 y presupuesta cambioMG92B pendiente de alojamiento. El ahorro principal esta en los1015g de PLA y437g de acero anteriores; el PVC anterior solo pesaba38g.

Candidato90/75mm, alcance458->373mm explicitamente reducido, hombro209->100mm propuesto. CuatroGX3270BLS90g, MG92B enJ4/pinza yMG90S enJ6:401g de servos,26g mas que v09 pero mucho menos queRS01. ReduccionesJ2=1.9 yJ3=1.4; sobremarchaJ4=.85 permite153grados de motor para180de articulacion y ahorra41.2g frente alMG946R provisional.

Presupuesto1628.0g sin carga/con pies;1378.0g sin carga ni pies;1828.0g con200g. Ahorro26.0% frente a2200.43g de v09 sin carga, incluyendo250g propuestos de pies. Reserva estructural25%:1858.9g sin carga. Incluye eje de hombro, hub integral de base y J5 prolongado para atravesar apoyos; sus uniones de torque siguen pendientes. No son masas de CAD nuevo ni pesos medidos.

Se calculan5502estados/262trayectorias,30grados/s,200g,50ms y19orientacionesJ1. Margen minimo preliminar de par2.358, reserva2.190, usando maximosGX55kgfcm6V y recta par-velocidad supuesta. Pares continuos, inercia interna y frenado real no acreditados. No se declara que esta familia extendida envuelva toda trayectoria posible.

El calculo inicial rechaza ejes5mm de codo/J4: se presupuestan8mm/608 y6mm/626-2Z. Con reserva se requieren hasta448.9MPa en ejes y207.0MPa en placas, bajo n2/Kt2 provisionales. Se especifican aceroRp>=600 y aluminioRp>=240 certificados, sin asignarlos a material comprado. Contactos, ranuras/acoples, tornillos, acrilico, PVC y PLA FDM siguen pendientes. Pads requierenmu>=.465 bajo hipotesis de pinza; agarre no aprobado.

Sobre el rectangulo de ejes inferidos del CAD, utilizacion de vuelco1.505: falla. El manual acota245mm exteriores, no contactos reales. Se estudian pies desplegables320x320,250g: utilizacion.583/.587 con reserva. Son propuesta para manipular detenido, no mecanismo aceptado/modelado ni estabilidad de transporte demostrada. La extraccion de ruedas muestra discrepancias que impiden llamar contactos fisicos a sus vertices inferiores.

Traccion separada: fichaGM25-3709V/3kgfcm nominales, rueda80. Conjunto compacto3.328kg: escenario7.4V/eta.6/a.5/Crr.05/masa rotante1kg da aproximadamente3.80N requeridos y9.45N diagonales disponibles; margen2.49 plano,1.42 a5grados. Sensibilidad de tension/rodadura, no capacidad homologada. Variante instalada, bateria y friccion pendientes.

| Modulo | Estado de esta ficha |
|---|---|
| atmega/esp/web | Sin cambios, builds ni flasheos |
| CAD | Predimensionamiento mecanico solicitado, lectura Fusion y fuentes/scripts/JSON/CSV. Sin nuevo CAD, housing, FEA ni ensayo fisico |
| Loader | Sin intervencion |
| Contracts | Registro y enlaces |

Faltan curvas/limites y calibracion de actuadores, productos argentinos exactos, alimentacion, apoyos, agarre y uniones antes de seleccionar definitivamente. No se ejecutaron tests de software ni compilaciones; solo analisis mecanicos pedidos y consultas de referencia.
