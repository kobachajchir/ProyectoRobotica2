# AFR2-20261028-08 - Mantener30grados/s: par nominal, inercia y apoyo dinamico

28/10/2026. [Informe y calculos v12](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v12/README.md). Continuacion del objetivo activo; no se reduce la velocidad a la de la opcionSTS3215/reductores de v11. Fusion permanece en v09, sin nuevos housings.

Se comparan12 presupuestosSTS3250/STS3215 con30grados/s en los seis ejes,carga200g y frenada50ms. Incluso optimizando la reduccionJ2 con el presupuesto fijo, el margen maximo en la sensibilidad de curva es1.776 frente al objetivo2. Se documenta oferta de importacionSTS3250C001/C002 con Argentina entre mercados, sin afirmar stock nacional ni costo final de entrega.

La fichaRobStride RS recuperada de la API oficial lleva nombre2026.010.09, distinta de la tabla indexadaJul2026. RS01 actual:395+-3g,equivalente de inercia0.0042kgm2,7Nm a100rpm bajo disipador150x150mm y6NmRated en curva de reposo. Se estudian tresRS01 con reducciones3/6/3,STS3215 distales con1/4/1,MG92B de pinza, disipadores3/5mm de espesor supuesto y650g de potencia/control reservados.

Ocho presupuestosRS01+STS pasan SOLO el filtro de par calculado: la masa llega5455-5819g. El caso5mm/eta0.70/inerciax2 queda en2.002 de margenJ2; no es reserva robusta frente a datos faltantes. Se incluye inercia equivalente axialRS01 y reaccion sobre padres; siguen abiertas la inerciaSTS y la equivalencia termica del disipador real, su alimentacion y frenado.

Se aplican cargas a99378 estadosJ1 por alternativa: requisito FDM maximo22.08MPa, requisitoRp del husillo527MPa con n2/Kt2,P0 maximo795.5N. La geometria de cajas,rodamientos y contactos nuevos no esta creada ni validada. PLA PrintaLot sin perfil/propiedades FDM asignadas: no se aprueba la resistencia por la ficha de filamento.

La estabilidad se calcula ademas de forma DINAMICA sobre rectangulos hipoteticos, no soloCGestatico. Con semiejes100/80mm,sueloz=-40mm y auto quieto, el escenario v09 requiere4.87kg de masa de auto adicional centrada; DS51150 reforzado5.91kg y RS01 pesado7.38kg. No son masas medidas ni apoyos reales. Se pidieron al usuario masa del auto equipado y separaciones de contactos con el piso; no se sustituye esa evidencia por el ancho exterior de la foto.

Resolver el par no cierra la compatibilidad de peso,uniones y apoyo. El objetivo permanece activo y no se recomiendan compras ni housings de la opcion pesada como si ya estuviera aprobada.

| Modulo | Estado de esta ficha |
|---|---|
| atmega/esp/web | Sin cambios, builds ni flasheos |
| CAD | Calculos mecanicos externos solicitados, fuentes, JSON/CSV e informe; sin nuevo CAD, FEA ni pruebas fisicas |
| Loader | Sin intervencion |
| Contracts | Registro y enlaces |

NPZ regenerables quedan enCAD/exports; fuentes y resumenes versionados con hashes de textoLF. No se ejecutaron tests ni compilaciones.
