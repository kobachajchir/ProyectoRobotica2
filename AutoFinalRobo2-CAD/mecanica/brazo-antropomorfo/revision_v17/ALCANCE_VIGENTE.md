> Ultima indicacion: no continuar calculando vuelco. Verificar actuadores
> con la pieza200g mas el peso propio y preparar aligerado selectivo de
> Fusion. Estudio Shape Optimization sin resolver, diferido por indicacion
> anterior. La preseleccion vigente esta en SELECCION_ACTUADORES.md.

# Alcance vigente confirmado por el usuario

29/10/2026. Conservar la pinza del modeloV7, considerada adecuada por el
usuario. Se descarta la pinza compacta/paralelogramo v15 y el ensamble
alternativo v16. No se trasladan sus resultados de agarre al brazo existente.

La base de trabajo es una copia independiente de v09: mantiene la pinza y
cadena antropomorfa deV7 e incorpora las correcciones posteriores de ejes,
rodamientos, montura baja y housing. Los maestros anteriores quedan intactos.

J1: giro axial de base. J2: hombro. J3: codo. **Aclaracion final del usuario:**
trasladar el motorJ3 junto aJ2, sobre el primer eslabon (padre cinematico de
J3), y llevar el par al codo por correa con poleas de aluminio. El motor,
su soporte y el eje fijo de la polea de entrada giran solidariamente con el
primer eslabon al moverJ2: la distancia entre poleas permanece constante.
No trasladar ese motor a una pieza que no acompañe el movimientoJ2.

No se prolonga esta transmision a traves del eje axial de muñecaJ4, que
introduciria rotacion relativa entre planos de poleas. La correccion no
consiste en trasladar el motorJ2; queda sustituida la interpretacion anterior.
Reducir peso de eslabones, actuadores y soportes, conservando pinzaV7.

La copia inicial exportada contiene377 componentes y14 articulaciones.
Es la geometria original copiada; no acredita aun el rediseño de correa ni
una reduccion de masa. Calcular con la pinza existente y las masas reales
antes de elegir actuadores y modificar las articulaciones.

El usuario acepta un margen de calculo superior a1 o1.2. Se adopta1.2
como minimo de preseleccion de actuadores, sustituyendo el objetivo2
usado anteriormente para ese filtro. No se cambia retroactivamente la
resistencia del material ni se da por comprobada la frenada fisica.
El presupuesto junto aJ2 alcanza un minimo1.2508 enJ3: permite continuar
el diseño, condicionado a actualizar masas y cargas al terminar el CAD.
