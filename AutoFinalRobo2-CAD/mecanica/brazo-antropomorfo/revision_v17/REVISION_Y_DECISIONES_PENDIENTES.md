# Brazo v17: revisión en curso, no entrega cerrada

**Instruccion posterior vigente:** el usuario suspendio nuevos calculos de
vuelco y pide concentrarse en actuadores para200g y revisar el aligerado
enFusion. La pregunta sobre frenada quedo superada. Los resultados de
estabilidad siguientes son antecedentes; no ejecutar mas sensibilidades
ni bloquear el montaje por esa pregunta. No se acredita estabilidad por ello.

La fuente elegida deriva del maestro v09 y conserva la pinza de V7. Los
bancos v15/v16 y la pinza compacta alternativa no son el diseño elegido.
El F3D editable y sus fases aplicadas constan en `estado_cad_v17.json` y
`progreso_J3.jsonl`. Una fase escrita o un cálculo no equivalen a geometría
aplicada. Los errores se registran; una transacción fallida no se suma al CAD.

## Alcance acordado

- Carga máxima200g, pieza50x50x50mm; PLA PrintALot, perfilFDM sin definir.
- J1 axial, J2 hombro, J3 codo; motorJ3 en el primer eslabón próximo aJ2,
  transmisión externa por correa sin cruzar el giro axial de muñeca.
- Tubos25x1.2 y20x1mm; ejes de los eslabones70/60mm. No elevar la montura.
- Conservar recorridos y pinzaV7. Corregir apoyos, uniones y choques.
- Última prioridad: terminar y lograr estabilidad sobre ruedas en cualquier
  dirección de extensión cargada. El usuario rechazó limitar el azimut por
  control como solución al vuelco. Aligerado fino posterior por el usuario.
- Estudio de optimización de material expresamente diferido:
  [pendiente separado](PENDIENTE_OPTIMIZACION_MATERIAL.md). No ejecutarlo.

## Construido y correcciones

Se trasladó el STS3250 del codo y se construyeron carro, cuatro fijaciones
oblongas, housing rojo, dos625 externos, eje, acople25T, chavetas, retenciones
y ajusteM3. Motor, acople y ambos apoyos acompañan al carro al tensar.
La última relación aplicada es20/50=2.5, correaHP3M210x6, distancia nominal
50.4672mm y centro de entrada19.5328mm respecto del eje del hombro.
Se conserva el recorrido del codo; entrada requiere±225° y referencia
multivuelta al encender. Las poleas son envolventes conservadoras, sin
perfil comercial de dientes fabricable.

El primer contorno de correa tenía un centro exterior que no seguía al
interior al cambiar parámetros. Se reconstruyó el croquis con centros
coincidentes y cotas conductoras explícitas. No se oculta esa interferencia
como un supuesto contacto normal entre correa y polea.

Se reconstruyeron encastresA05/A06 para evitar cuerpos separados del
historial antiguo al reducir diámetros y largos. Se recuperó la caraD del
eje y se recortaron completos los asientos608 después de unir ramas y aros.
La oreja negativa del hombro recibió una diagonal que evita el barrido
de los anclajes del tensor; requiere su posterior revisión resistente.
La continuidad de sólidos se comprueba antes de aceptar cada reconstrucción.
Consultar el journal para el estado posterior deA11/A12 y sus despejes.

## Qué muestran las comprobaciones y qué no

La extracción nativa produce volúmenes, centros y tensores por cuerpo.
Las masas se sustituyen por ficha en servos/rodamientos y por densidad en
materiales. PLA se calcula macizo, no como peso real de una pieza laminada.
Las masas de los presupuestos de1.17/1.45kg no describen este CAD.

`calcular_cad_v17.py` incluye trabajo virtual de todos los grupos rígidos,
incluidas polea de entrada y piñones auxiliares. El muestreo considera200g,
30°/s articulares y frenada cúbica50ms, con pico900°/s².30°/s es una hipótesis
de operación, no la velocidad nominal bajo carga garantizada por el fabricante.
Faltan interiores del servo, flexibilidad, holguras y perfil/resistenciaFDM.
No es FEA ni certificación de parada.

El cálculo anterior a las últimas correcciones dio aproximadamente3.18Nm
enJ2,2.17Nm enJ3,0.736Nm enJ4 y0.915Nm enJ5. Por eso se descartó STS3032
directo enJ4 y se aumentó la reducciónJ3. Esos valores deben regenerarse
con los alojamientos y actuadores finalmente integrados; todavía faltan
J1/J2/J4/J5/J6. No comprar a partir de una selección incompleta.

## Vuelco: el peso central no basta

Solo el grupo00 está fijo al auto. Otros motores quedan próximos a las
articulaciones pero giran conJ1 oJ2; no todo ese peso permanece inmóvil.
El peso cercano al centro sí aumenta el momento estabilizador, pero una
frenada transmite también fuerzas de inercia y momentos al suelo.

La referencia CAD del auto coloca centros de ruedaX±107,Y=-32±62mm y
pisoZ=-80mm. Es una inferencia del modelo, no una medición del contacto de
los rodillos. Se usa auto1.5kg aproximado y CG horizontal centrado supuesto.
No se equipara el ancho exterior245mm del manual con el apoyo efectivo.

Una instantánea previa a las últimas correcciones, de2.235kg sin carga,
dio centro de presiones longitudinal88.96mm durante la frenada, frente a
62mm de semiapoyo supuesto. Utilización1.435 y reacción de un par de ruedas
−9.57N: ese caso exigiría levantar ruedas. No aprobar estabilidad por el
solo hecho de tener masa central. Ver `vuelco_cad_actual.json` y su alcance.

Se compararon centrado deJ1, hombro más bajo, muñeca más corta y un motorJ3
trasero, conservando las masas estructurales del CAD y reservando montaje.
Son **propuestas numéricas sin modelar**, no decisiones incorporadas al F3D.
Con50ms las comparaciones no cierran la estabilidad en todas las direcciones.
`sensibilidad_frenadas_estabilidad.json` conserva una comparación con tiempos
mayores sobre una instantánea única identificada porSHA256. Tampoco autoriza
una nueva frenada ni constituye una envolvente global o un ensayo físico.

Se pidió al usuario definir si admite una parada controlada más larga o si
50ms debe mantenerse como requisito. **No cambiarlo sin su respuesta.**
La pinza orientada verticalmente se estudió por separado, pero no se adopta
como sustituto de la exigencia de extensión cargada en cualquier dirección.
No hay aprobación del traslado/frenada del chasis ni de pendiente/deslizamiento.

## Próximo cierre

Resolver el requisito de frenada y fijar un layout que supere el cálculo
previo; terminar actuadores y housings sobre ese layout; repetir masa,
cargas, resistencia local y revisión CAD de movimientos/interferencias.
Conservar separados contactos de dientes/roscas y choques reales, sin
eliminar penetraciones mediante filtros generales. Guardar/exportar,
registrar estado real enContracts y publicar porGit según lo autorizado.
No ejecutar tests de software, builds ni flasheos.
