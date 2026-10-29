# Resultados numericos v13

AFR2-20261028-09. Prediseno externo, sin nuevo CAD ni resistencia fisica aprobada. Masa del auto1.5kg aproximada; CG centrado supuesto.

## Comparacion de alternativas

| Caso | Masa con200g y reservas (g) | Minimo par disponible/demanda en sensibilidad | Vuelco ejes inferidos | Vuelco pies320 |
|---|---:|---:|---:|---:|
| centrado_L130_120 | 1495.4 | 1.174 | 2.181 | 0.845 |
| centrado_L100_85 | 1486.2 | 1.256 | 1.730 | 0.670 |
| centrado_PWM_L130_120 | 1840.1 | 1.644 | 2.068 | 0.801 |
| centrado_PWM_L100_85 | 1830.9 | 2.151 | 1.633 | 0.633 |
| centrado_PWM_L100_85_reserva25 | 2062.5 | 1.996 | 1.645 | 0.637 |
| centrado_PWM_L90_75 | 1828.0 | 2.358 | 1.505 | 0.583 |
| centrado_PWM_L90_75_reserva25 | 2058.9 | 2.190 | 1.515 | 0.587 |

Par>=2 es filtro preliminar; utiliza pares maximos y recta par/velocidad SUPUESTA, no pares continuos acreditados. Vuelco<1 indica centro de presiones dentro del rectangulo, sin demostrar toda la distribucion/friccion. Los casos PWM incluyen250g de pies; reserva25 aumenta estructura y pies25%, manteniendo masas de motores/electronica/bateria.

## Candidato compacto90/75

- Masa sin carga, con pies: **1628.0g**; con carga: **1828.0g**.
- Sin carga ni pies propuestos: **1378.0g**. Pies250g ya incluidos arriba, no masa gratuita.
- Comparacion v09 sin carga2200.43g: ahorro con pies **26.0%**, previo a definir todo el montaje.
- Con auto1.5kg y carga200g: **3.328kg** totales.
- Reserva estructural25%: **1858.9g** sin carga, no es una cota de masa real garantizada.
- Alcance al centro de la pieza373mm frente a458mm nativos; hombro100mm frente a209mm. Se mantiene toda la cadena distal55/65/88mm.

| Material/funcion | Masa presupuestada(g) |
|---|---:|
| Al6061 | 314.58 |
| PLA | 206.57 |
| Servo | 401.00 |
| Electronica | 77.00 |
| Potencia | 90.00 |
| Cables | 20.00 |
| Acero | 137.17 |
| Rodamiento | 113.00 |
| PVC | 15.32 |
| Carga | 200.00 |
| Polimero | 0.96 |
| Pads | 2.39 |
| Varios | 250.00 |

Los servos suman401.0g: son mas livianos que los RS01 descartados, pero no menos que los375.2g de servos v09. El ahorro total procede principalmente de PLA/bridas/control; esas piezas y los77g de electronica todavia no son un BOM comprado ni un CAD definitivo.

## Demandas por eje

| Eje | Demanda pico motor(kgfcm) | Par maximo usado(kgfcm) | Minimo cociente con recta supuesta | Fuerza resultante(N) | Momento resultante(Nm) |
|---|---:|---:|---:|---:|---:|
| J1 | 13.173 | 55.00 | 4.050 | 16.623 | 3.5791 |
| J2 | 21.904 | 55.00 | 2.358 | 14.357 | 3.5710 |
| J3 | 19.629 | 55.00 | 2.680 | 11.822 | 2.3628 |
| J4 | 1.148 | 3.50 | 2.996 | 10.344 | 1.5242 |
| J5 | 9.688 | 55.00 | 5.507 | 7.710 | 0.9808 |
| J6 | 0.067 | 1.80 | 26.070 | 6.976 | 0.4973 |

Fuerza y momento son normas maximas de envolvente, no componentes simultaneas de un unico estado. No hay inercias internas conocidas de servos.

## Masa y centro de masa por grupo

| Grupo | Masa(g) | CG local X,Y,Z(mm) |
|---|---:|---|
| 00 | 713.68 | -6.63, -25.54, 11.62 |
| 01 | 235.27 | -0.46, 21.12, 21.74 |
| 02 | 239.21 | 0.00, 21.79, 54.47 |
| 03 | 117.38 | 8.54, 0.00, 39.41 |
| 04 | 167.89 | 0.00, 27.40, 42.87 |
| 05 | 41.65 | 0.00, 0.00, 39.15 |
| 06 | 112.94 | 1.55, -0.03, 33.26 |
| 90 | 200.00 | 0.00, 0.00, 0.00 |

00 es fijo al auto;01 es base movil antes de hombro;02 brazo;03 antebrazo;04/05 muñeca;06 herramienta;90 carga. Coordenadas locales de cada articulacion; tensor completo enJSON. El CG global horizontal se guarda en resultados_liviano.json, no es el CG medido del auto completo.

## Resistencia minima requerida

| Caso | Maximo Rp requerido eje(MPa) | Maximo Rp requerido placas(MPa) | Rp montura(MPa) | C0 maximo requerido(N) | Mu de pads requerido para factor2 |
|---|---:|---:|---:|---:|---:|
| centrado_PWM_L130_120 | 548.1 | 277.9 | 216.1 | 1089.8 | 0.511 |
| centrado_PWM_L100_85 | 436.6 | 211.2 | 164.3 | 837.2 | 0.476 |
| centrado_PWM_L100_85_reserva25 | 459.6 | 227.5 | 176.9 | 903.0 | 0.476 |
| centrado_PWM_L90_75 | 426.5 | 192.3 | 149.6 | 765.7 | 0.465 |
| centrado_PWM_L90_75_reserva25 | 448.9 | 207.0 | 161.0 | 825.2 | 0.465 |

n=2/Kt=2 provisionales en ejes/placas; rodamientos usan cotaFr+Fa con n2 y separaciones propuestas. Requiere acero certificadoRp>=600MPa, aluminio certificadoRp>=240MPa y ligamentos netos declarados. No basta con esa tabla: faltan acoples/ranuras, fijaciones, asientos, acrilico y apoyos. El coeficiente de pads es requisito dinamico, no un dato medido; la pinza usa par de bloqueo y eficiencia.7 supuesta.

| Tubo del candidato | Sigma envolvente(MPa) | Tau envolvente(MPa) | Resistencia efectiva requerida con n2/Kt2(MPa) |
|---|---:|---:|---:|
| PVC25 | 5.945 | 3.167 | 32.356 |
| PVC20 | 6.429 | 3.418 | 34.960 |

Al PVC no se le asigna aqui una resistencia medida. Al PLA PrintaLot tampoco se le asigna el valor de la resina como resistencia FDM. Fluencia diferida, temperatura, taladros y contacto permanecen pendientes.

## Traccion con la masa de este candidato

Mismo escenario de sensibilidad: bateria7.4V, motorV=7.4-1.2x.49, eficiencia.6, a=.5m/s2,Crr=.05 y masa rotante equivalente1kg. La transferencia par/tension es una hipotesis de cribado.

| Caso | Masa total(kg) | Pendiente(deg) | Fuerza requerida(N) | Fuerza diagonal disponible(N) | Cociente |
|---|---:|---:|---:|---:|---:|
| centrado_PWM_L90_75 | 3.328 | 0 | 3.796 | 9.447 | 2.489 |
| centrado_PWM_L90_75 | 3.328 | 5 | 6.634 | 9.447 | 1.424 |
| centrado_PWM_L90_75_reserva25 | 3.559 | 0 | 4.025 | 9.447 | 2.347 |
| centrado_PWM_L90_75_reserva25 | 3.559 | 5 | 7.060 | 9.447 | 1.338 |

Estos resultados no prueban arranque, desplazamiento real ni frenada del AGV. No se usa par de bloqueo para declarar una carga maxima enkg.

**Conclusion:** candidata de predimensionamiento con condicion de apoyos; compra y nuevos housings siguen sin cerrar. No se ha demostrado aun el cumplimiento fisico simultaneo del objetivo.
