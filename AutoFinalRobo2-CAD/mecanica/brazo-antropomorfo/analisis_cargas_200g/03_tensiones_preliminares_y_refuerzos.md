# 3. Tensiones nominales y criterios para los refuerzos

Este capítulo hace una primera comprobación analítica de los tramos de tubo y prepara las cargas locales. **No es FEA ni un dictamen de resistencia de las articulaciones impresas.** No se agregan refuerzos al CAD antes de resolver el peso y las cargas que ellos mismos sumarían.

## Tubos PVC Ø32/28,3 mm

La pared de estos diámetros es 1,85 mm; no 3,2 mm. El área e inercias de la sección anular son:

```text
A = pi (D^2 − d^2)/4 = 175.23 mm²
I = pi (D^4 − d^4)/64 = 19985.99 mm⁴
J_polar = 2I = 39971.98 mm⁴
W = I/(D/2) = 1249.12 mm³
sigma_flex = M_flex/W
tau_torsion = T (D/2)/J_polar
```

Las fuerzas/momentos de corte incluyen carga útil y peso propio distal. Para el PVC que atraviesa una sección se integra la masa del tramo restante; los otros cuerpos se concentran en sus CG. Las zonas impresas se consideran rígidas en esta primera aproximación. Las secciones próximas a los manguitos necesitan un modelo de contacto posterior.

| Tubo | Distancia desde su eje proximal (mm) | Momento (N·m) | Tensión nominal (MPa) | Con carga multiplicada ×2 (MPa) |
| --- | --- | --- | --- | --- |
| PVC1 | 25 | 2.641 | 2.114 | 4.228 |
| PVC1 | 42 | 2.460 | 1.970 | 3.939 |
| PVC1 | 65 | 2.218 | 1.775 | 3.551 |
| PVC1 | 88 | 1.976 | 1.582 | 3.164 |
| PVC1 | 105 | 1.799 | 1.440 | 2.880 |
| PVC2 | 25 | 1.371 | 1.097 | 2.194 |
| PVC2 | 42 | 1.251 | 1.001 | 2.003 |
| PVC2 | 60 | 1.125 | 0.901 | 1.801 |
| PVC2 | 78 | 1.000 | 0.800 | 1.601 |
| PVC2 | 95 | 0.882 | 0.706 | 1.413 |


La parte expuesta del primer tubo va de 42 a 88 mm desde el hombro y la del segundo de 42 a 78 mm desde el codo. La mayor tensión nominal en la salida proximal libre es aproximadamente **1,97 MPa** en el primero y **1,00 MPa** en el segundo; con cargas duplicadas serían 3,94 y 2,00 MPa. Es una comparación muy favorable respecto de los 52 MPa típicos de rotura a tracción de un PVC-U de referencia, pero no autoriza a usar ese cociente como factor de seguridad del conjunto: faltan la formulación real, muescas, fatiga, aplastamiento de abrazadera, temperatura y creep.

La extensión horizontal no es un caso de compresión axial que deba resolverse como pandeo de Euler. Predominan flexión, torsión y deformación de uniones. El peso propio de los dos tubos es solamente **38,1 g**; no parece conveniente comenzar agregando caño o metal a toda su longitud.

Con trabajo virtual `delta = integral[M(s) × (458−s)/(E I) ds]`, sumando solo los dos tramos de PVC expuestos y haciendo rígidas las demás piezas, la contribución estimada a la caída de la carga es **0.85…0.95 mm** para E=3,0…2,7 GPa de corta duración. La caída total real será mayor si hay flexibilidad de PETG, juego, deslizamiento o giro de engranajes. No es una predicción de deformación a largo plazo.

## Dónde reforzar primero

| Zona | Carga que gobierna en este análisis | Dirección de diseño propuesta, todavía sin modelar |
| --- | --- | --- |
| Cartucho J1 y apoyo a montura | Par de vuelco 2,93 N·m; alrededor de 367 N radiales por apoyo con 8 mm entre centros | Buscar más separación entre apoyos, asiento continuo y nervios a la placa. Mantener la altura baja; estudiar accionamiento lateral si ampliar el cartucho lo exige |
| Hombro y dientes 20/40T | Par de salida 2,91 N·m; 97 N tangenciales en el engrane | Nervios desde el portarodamiento a las raíces de la torre; revisar raíz de diente, ancho de cara y unión del cubo/D, no solo el tubo |
| Manguito proximal eslabón 1 | Momento de salida de tubo cercano a 2,46 N·m | Estudiar solape útil 25–30 mm frente a los 17 mm actuales, transiciones redondeadas y nervios en el plano principal de flexión, conservando giro y accesos |
| Codo/horquilla distal del eslabón 1 | Par de actuador 1,55 N·m y carga distal 8,39 N | Nervios simétricos hasta el manguito; revisar D, horn y fijaciones. Resolver además el margen del DS3218: más plástico no aumenta su par |
| Antebrazo y cartucho J4 | Momento de tubo cercano a 1,25 N·m; apoyos J4 ~92/97 N | Refuerzo local de la copa y raíces, mayor separación de apoyos si cabe. Evitar hacer macizo todo el antebrazo |
| Horquilla J5 y cuello J6 | Par de J5 0,427 N·m; J6 en flexión 0,209 N·m | Nervios cortos de la horquilla al alojamiento; en J6 estudiar apoyos más separados, hoy solo 6 mm entre centros |
| Palma y dedos | Peso distal 3,06 N sobre J6, más fuerza de cierre dependiente de fricción | Reforzar pie del dedo, unión carro–cremallera y guías; conservar recorrido y holguras |
| Contraplaca y montura | Resultante añadida 21,86 N y momento externo de unos 2,8 N·m sobre el apriete | Verificar contacto con acrílico, arandelas, flexión de la placa de 3 mm y precarga; no tratar todos los agujeros como empotramientos ideales |

La separación de apoyos merece prioridad: para el mismo par de J1, pasar idealmente de 8 a 20 mm bajaría la fuerza de par de 367 a unos 147 N. Esto es un objetivo geométrico, **no un cambio directo aprobado**: hay que acomodar motor y eje manteniendo la montura baja y volver a comprobar el barrido. Un refuerzo distal adicional de 50 g a 450 mm agrega `0,05×9,80665×0,45 ≈ 0,221 N·m` al hombro; a 65 mm agrega solo 0,032 N·m. El refuerzo debe justificar la masa que incorpora.

No se determina un espesor final de nervio a partir de la densidad. Para una sección simple, el requisito nominal es `W_requerido ≥ n M / sigma_admisible`, pero `sigma_admisible` debe proceder de la pieza impresa, orientación, temperatura y duración de carga. Aún no se conoce para el PETG real. Los asientos y las raíces de horquilla no se reducen a esa fórmula sin revisar concentración y contacto.

## Pinza: la carga de cierre no es el peso del cubo

En una orientación de agarre lateral, para sostener el cubo por fricción: `2 mu N ≥ m g`. Con piñón de radio 12 mm y eficiencia supuesta 0,8, `tau_motor ≈ 2 N r / 0,8`. Las fuerzas sobre los dos racks se suman en el par del piñón.

| Fricción supuesta mu | Normal mínima por dedo (N) | Servo estático (kgf·cm) | Objetivo ×2 (kgf·cm) |
| --- | --- | --- | --- |
| 0.20 | 4.90 | 1.50 | 3.00 |
| 0.30 | 3.27 | 1.00 | 2.00 |
| 0.50 | 1.96 | 0.60 | 1.20 |


Los coeficientes son sensibilidad asumida, no propiedades medidas de las almohadillas. Para mu=0,30 el objetivo es 2,0 kgf·cm, mayor que los 1,8 kgf·cm de bloqueo publicados del MG90S a 4,8 V. Con mu=0,50 resulta 1,2 kgf·cm. Deben confirmarse goma/superficie y fuerza real; el análisis de gravedad con el cubo perfectamente centrado no resuelve por sí solo el agarre.

## Criterios antes del análisis de tensiones detallado

1. Confirmar filamento y caño, orientación de impresión, perímetros, capas superiores/inferiores y masa del laminador. Medir masa real de servos, rodamientos, horns y tornillos que se comprarán. Sustituir los supuestos del presupuesto de masas.
2. Resolver primero el margen del codo y revisar agarre. Una reducción mecánica reduce el recorrido angular disponible: duplicar relación exigiría revisar el rango de 180° del codo y el recorrido del servo. Alternativas: actuador de mayor par, menor masa distal o compensación elástica que se dimensionará para todo el recorrido.
3. Preparar material de PETG impreso, incluyendo dirección entre capas y comportamiento dependiente del tiempo. No usar un material isotrópico macizo para declarar segura una impresión con relleno parcial.
4. Aplicar gravedad por cuerpo y carga útil en su CG. Para un submodelo usar la resultante de interfaz equivalente. No aplicar simultáneamente gravedad de las mismas piezas distales y la fuerza resultante que ya las representa.
5. Representar contactos de manguitos/PVC, aros/shaft y precarga de tornillos; imponer la libertad de cada articulación y el par del accionamiento. El [CSV de reacciones](reacciones_para_FEA.csv) da fuerzas del apoyo sobre el eje; sobre el alojamiento se invierten. No duplicar ese par en ambos aros.
6. Estudiar primero el caso estático, luego una envolvente de carga ×1,5/×2 claramente identificada. Esos multiplicadores no sustituyen un modelo de aceleración ni una curva térmica del servo. Evaluar desplazamientos, raíz de nervios/dientes, asientos, D, aplastamiento y separación de contactos, además de una tensión equivalente.
7. Con resultados de tensión y desplazamiento, iterar masa, CG y par después de cada refuerzo. La revisión geométrica de interferencias deberá repetirse cuando cambien las piezas.

**Estado:** masas, CG y esfuerzos estáticos estimados calculados; primera flexión de tubos calculada; refuerzos definidos como criterios. No se ejecutó FEA, no se modificaron las piezas con nervios nuevos y no se acredita todavía «resiste/no resiste» para las articulaciones completas.
