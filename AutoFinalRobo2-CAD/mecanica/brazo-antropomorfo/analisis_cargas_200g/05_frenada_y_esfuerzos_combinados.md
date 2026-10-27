# 5. Frenada de emergencia y esfuerzos combinados

[Índice del informe](INFORME_ITERACION_DISENO.md) · [Anterior: evolución](04_evolucion_y_correcciones.md) · [Siguiente: plan v07](06_plan_rediseño_v07.md)

**Estado:** formulación y criterios de diseño. Este capítulo no contiene resultados numéricos de dinámica ni FEA. Los valores de los capítulos 1–3 siguen siendo estáticos.

## Qué significa el caso pesimista solicitado

El caso propuesto parte del brazo extendido, con carga máxima y movimiento previo, y solicita detenerlo de emergencia. Reúne alcance, peso propio e inercia, por lo que debe estudiarse. No basta con sumar «velocidad máxima + aceleración máxima + gravedad»: deben definirse una trayectoria, un estado inicial alcanzable, sentidos de movimiento y una ley de parada.

La velocidad máxima sostenida tiene aceleración nula. Una frenada produce desaceleración, cuyo límite puede ser distinto del de aceleración normal. La velocidad sin carga de un catálogo tampoco acredita velocidad o aceleración alcanzables con el brazo y la pieza. Si el disparo de emergencia ocurre mientras todavía se acelera, el perfil de parada debe partir de esa aceleración inicial y representar la transición.

La postura horizontal extendida es importante para la gravedad sobre hombro y codo. La mayor carga sobre otra sección puede aparecer en una postura distinta, en otro eje o antes de llegar al punto final. Los términos dinámicos dependen de postura, velocidad y aceleración, como explica [Modern Robotics, Newton–Euler](https://modernrobotics.northwestern.edu/nu-gm-book-resource/8-3-newton-euler-inverse-dynamics/). Esta es la razón para buscar una envolvente y no declarar universal un único caso.

## Parada finita, impulso y recorrido disponible

Para un eje rígido aislado con inercia equivalente constante:

$$
\int_0^{t_b}\tau_{\mathrm{neta}}(t)\,dt=\Delta H,
\qquad
|\overline{\alpha}|=\frac{|\omega_0|}{t_b},
\qquad
|\overline{\tau}_{\mathrm{inercial}}|=J_{\mathrm{eq}}\frac{|\omega_0|}{t_b}.
$$

Los valores con barra son promedios; el pico depende del perfil y del mecanismo. En el brazo multieje J_eq no sustituye el modelo completo. La relación torque–cambio de momento angular está desarrollada en [OpenStax](https://openstax.org/books/college-physics-2e/pages/10-5-angular-momentum-and-its-conservation).

Tomar t_b=0 para una velocidad inicial no nula no produce una carga finita de diseño. En el robot real una parada por impacto depende de rigidez, amortiguamiento, juego, contactos y tiempo de impacto. Debe separarse de la frenada controlada por el servo. Quitar alimentación a un servo tampoco acredita que retenga o frene la carga.

Se propone una sensibilidad inicial, sin elegir aún un tiempo operativo:

| Tiempo total de parada | Desaceleración media para la misma velocidad inicial | Par inercial medio relativo, mismo J_eq |
| --- | --- | --- |
| 200 ms | 5·abs(ω₀) rad/s² si ω₀ se expresa en rad/s | 1 |
| 100 ms | 10·abs(ω₀) rad/s² | 2 |
| 50 ms | 20·abs(ω₀) rad/s² | 4 |

Para desaceleración constante, el recorrido angular hasta detenerse es |Δq|=|ω₀|·t_b/2. Debe existir espacio para recorrerlo sin sobrepasar límites ni chocar. Si se desea acabar extendido horizontalmente, el inicio de la frenada debe ubicarse antes de esa postura, no suponerse velocidad nula desde el primer instante.

Como ejemplo algebraico de perfil suave, con s=t/t_b:

$$
\dot q(t)=\dot q_0(1-3s^2+2s^3),\qquad
\ddot q(t)=\frac{\dot q_0}{t_b}(-6s+6s^2).
$$

Este perfil comienza y termina con aceleración cero, recorre Δq=dot(q)_0·t_b/2 y tiene un pico |α|=1,5·|dot(q)_0|/t_b. Muestra por qué usar solo la desaceleración media subestima un pico. Es un perfil ilustrativo propuesto, no una función ejecutada ni una prestación acreditada del servo. Para una aceleración inicial distinta de cero se necesita otro perfil compatible.

## La carga interna completa de una sección

Se adopta un eje local x longitudinal a la sección. La solicitación se expresa como:

$$
\mathbf{s}=[N,\ V_y,\ V_z,\ T,\ M_y,\ M_z]^T.
$$

N es axial, V_y/V_z son cortantes, T es torsión y M_y/M_z son flexión. Fuerzas se expresan en N y momentos en N·m. «Cortante» es una fuerza; «momento flector» y «momento torsor» son momentos. El uso de términos distintos evita confundir sus unidades y efectos.

Las componentes deben transformarse al sistema de cada pieza. El mismo momento espacial puede ser torsión para un tramo y flexión para otro. Al trasladar una resultante del punto A al punto B, con r_BA dirigido de B hacia A:

$$
\mathbf F_B=\mathbf F_A,\qquad
\mathbf M_B=\mathbf M_A+\mathbf r_{BA}\times\mathbf F_A.
$$

No es correcto aplicar a la base únicamente el torque escalar del servo del codo, ni copiar todas las fuerzas y momentos sin trasladar su punto de aplicación. [Referencia: fuerzas y momentos espaciales, Modern Robotics](https://modernrobotics.northwestern.edu/nu-gm-book-resource/3-4-wrenches/).

## Cómo se obtendrán las resultantes dinámicas

Para cada cuerpo i, en un mismo sistema de referencia y con tensor de inercia respecto de su centro de masa:

$$
\mathbf F_i^{req}=m_i(\mathbf a_{G_i}-\mathbf g),
$$

$$
\mathbf M_{i,O}^{req}
=(\mathbf r_{G_i}-\mathbf r_O)\times\mathbf F_i^{req}
+\mathbf I_{G_i}\boldsymbol\alpha_i
+\boldsymbol\omega_i\times(\mathbf I_{G_i}\boldsymbol\omega_i).
$$

La suma de estas resultantes sobre los cuerpos distales determina la fuerza y momento que debe suministrar la interfaz proximal, descontando otras acciones externas conocidas sobre ese conjunto. Al incluir gravedad mediante a_G−g, no se agrega de nuevo su peso como segunda carga. La carga útil se representa con su masa/CG/inercia o con una acción equivalente, evitando contarla dos veces. [Referencia de cuerpo rígido: Modern Robotics](https://modernrobotics.northwestern.edu/nu-gm-book-resource/8-2-dynamics-of-a-single-rigid-body-part-2-of-2/).

Las velocidades y aceleraciones de cada cuerpo se obtienen de la cinemática multieje, incluyendo aceleraciones tangenciales, centrípetas y acoplamientos. La dinámica de articulaciones se puede expresar como:

$$
\boldsymbol\tau=\mathbf M(q)\ddot q+\mathbf C(q,\dot q)\dot q+\mathbf g(q),
$$

con las acciones externas, rozamiento y transmisión añadidos según el modelo. El símbolo g(q) aquí representa el vector de pares gravitatorios, mientras que g en la ecuación por cuerpo es la aceleración gravitatoria. [Referencia: matriz de masa y términos dinámicos, Modern Robotics](https://modernrobotics.northwestern.edu/nu-gm-book-resource/8-1-3-understanding-the-mass-matrix/).

El algoritmo devuelve pares de accionamiento y resultantes espaciales. La proyección sobre el eje articular determina el par que debe transmitir el accionamiento; el resto requiere un modelo de eje, rodamientos y alojamiento. Las fuerzas de engrane se añaden al estudiar la transmisión y sus apoyos según su diagrama de cuerpo libre, sin sumarlas otra vez como una carga externa sobre todo el robot.

## Datos que faltan para producir números de frenada

Los volúmenes y centroides existentes permiten el presupuesto de masas, pero no bastan por sí solos para una dinámica completa. Se necesitan tensores de inercia de las piezas, distribución de masa de los nuevos actuadores y movimientos de los elementos de transmisión. Los tensores geométricos del CAD deberán recibir densidad/masa coherente; los interiores desconocidos de servos se tratarán como aproximaciones declaradas. La inercia de rotor reflejada por una reducción depende del cuadrado de su relación y requiere datos o sensibilidad.

También faltan velocidades bajo carga, límites de aceleración, perfil/tiempo de frenada realizable, holguras, rigidez/amortiguamiento, fuerzas de agarre, masas de cables y aceleración del auto. El PWM es una consigna y no suministra por sí solo estas mediciones. El método puede planificarse con hipótesis, pero no presentarlas como valores nominales certificados.

## Zonas críticas y camino de cargas

El camino general va de pieza/dedos a palma, muñecas, antebrazo, codo, primer eslabón, hombro, cartucho J1, montura, tornillos/contraplaca, acrílico y chasis. La electrónica se fija para sostener su propia masa, no se utiliza como soporte estructural del brazo.

| Zona | Solicitaciones a combinar durante la frenada | Mecanismos que se estudiarán |
| --- | --- | --- |
| Cartucho J1 y unión a montura | Axial, cortantes, flexión y torsión; cargas del brazo y del giro de base | Flexión de eje, reacción de apoyos, aplastamiento o apertura de asiento, rotura de tapa/nervio |
| Montura, seis M3 y apriete central | Fuerza y momento trasladados a sus contactos | Reparto por rigidez/precarga, levantamiento, deslizamiento, flexión y daño del acrílico |
| Hombro y horquilla | Par de accionamiento, flexión fuera del eje y fuerzas de engrane | Raíces, dientes, cubo, planos de arrastre, fijaciones y contactos de rodamientos |
| Primer eslabón y manguitos | Flexión en dos planos, torsión, cortantes y axial | Concentración en salida de manguito, deslizamiento, aplastamiento de tubo y separación de capas |
| Codo, muñecas y pinza | Cargas transmitidas más inercias locales y fuerza de cierre | Ejes/acoples, apoyos, guías, pie de dedo y retención de la pieza |

La estática ya da 2,933 N·m de flexión en J1 y unos 367 N radiales por apoyo con 8 mm entre centros. El criterio idealizado F≈M/L explica estudiar mayor separación; por ejemplo, 20 mm darían unos 147 N para ese mismo par estático. No son resultados de una base rediseñada ni de una frenada.

## Envolvente y análisis de resistencia posterior

Se estudiarán sostén, aceleraciones y frenadas por eje en ambos sentidos; combinaciones alcanzables; posturas relevantes y parada del AGV con el brazo desplegado. La simultaneidad debe respetar recorridos, pares y alimentación disponibles. Una cota con máximos independientes puede servir como hipótesis conservadora, pero se identificará como tal y no como trayectoria real.

Para cada sección se conservará el vector completo s(t), el instante, la postura y el caso que lo produce. El máximo cortante y el máximo flector pueden ocurrir en momentos distintos: no se combinarán como si fueran un único estado real. Los casos gobernantes se trasladarán a submodelos de tensión/contacto.

Se revisarán tensiones normales y cortantes combinadas, desplazamiento, contactos, precarga, deslizamiento y fatiga. Para metal puede corresponder una tensión equivalente; en PETG FDM también se deben considerar orientación, resistencia entre capas, temperatura y deformación sostenida. Un modelo isotrópico macizo no acredita una impresión con relleno parcial.

Si la frenada excita modos flexibles o produce impactos de holgura, el modelo rígido y las cargas equivalentes no bastan para predecir el pico local: se deberá incorporar una respuesta transitoria apropiada. El factor estático ×2 existente no sustituye este estudio ni se aplicará varias veces sin definir qué incertidumbre cubre.

**Criterio de aceptación pendiente:** resistir una envolvente definida de carga y operación, manteniendo agarre y estabilidad, con márgenes justificados. Resistir una parada aislada no demuestra resistencia a cualquier movimiento, impactos no incluidos o servicio repetido.
