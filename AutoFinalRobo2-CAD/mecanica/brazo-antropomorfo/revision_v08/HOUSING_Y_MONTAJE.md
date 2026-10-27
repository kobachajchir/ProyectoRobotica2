# Housing GX3270BLS: construcción y montaje v08

Revisión sobre una copia v07. El codo conserva el servo GX3270BLS 270°, eje Ø8 y dos rodamientos 608. Se completa el alojamiento protector y se corrigen los espacios que los refuerzos de v07 habían invadido.

## Pieza impresa

`A06_Encastre_distal_brazo_PETG` contiene un solo sólido: manguito de solape27 mm, horquilla, cuna, bancos de pestañas y paredes protectoras. No se agregan placas impresas apoyadas sin fijación ni una tapa decorativa flotante.

Las paredes laterales y posterior usan `v08_housing_wall=3 mm`; hay juego lateral nominal 0,6 mm y posterior 0,8 mm. La zona superior y frontal queda abierta. Dos ventanas alivian laterales/trasera y una reserva lateral de 16×12 mm permite salir al cable. Son dimensiones propuestas: el catálogo no acota cable, pasacable ni conector. Las cuatro aristas exteriores traseras reciben radio 0,8 mm; los asientos de rodamientos y planos de pestañas se conservan.

La cuna no constituye un encapsulado térmico ni una clasificación IP. Su material es PETG rojo propuesto; espesor, orientación de impresión, tolerancias y resistencia siguen por determinar mediante el análisis y proceso reales.

## Fijaciones y orden de montaje propuesto

1. Insertar los cuatro insertos M3 de la cuna y los insertos de retención608, con las piezas aún separadas. Los agujeros representan envolventes nominales, sin estrías ni rosca de fabricación.
2. Montar eje, brida y rodamientos según sus apoyos. La tapa exterior608 es desmontable; el housing tiene una reserva Ø38,6 mm para su contorno Ø38.
3. Introducir el servo desde el frente, dirección−Y local del primer eslabón, con el horn retirado. El cuerpo y sus pestañas deben asentarse sin atravesar material.
4. Fijar las pestañas con cuatro M3×10 y arandelas Ø7 sobre los pasos Ø4,5 del motor. Los pasos M3 y cajas de insertos se cortan **después** de unir las paredes, para conservarlos libres.
5. Conectar el horn metálico al estriado y a la brida. La envolvente central del tornillo se representa como paso Ø3,2×4 mm; rosca y longitud útil del motor real no están documentadas. No interpretar ese paso como una especificación del fabricante.
6. Colocar las tapas de retención y comprobar acceso de herramienta. Para mantenimiento puede ser necesario retirar la tapa 608 exterior y el horn antes de extraer el servo. La secuencia es una propuesta CAD, sin ensayo de montaje.

El patrón del horn comercial y el offset de su eje siguen siendo referencias editables. [Selección, plano e inferencias de v07](../revision_v07/ACTUADOR_Y_MONTAJE.md).

## Correcciones alrededor del housing

- Reserva de la tapa 608 en la bandeja, conservando completa la tapa independiente.
- Alojamientos de los cuatro insertos608 llevados a las posiciones de los insertos existentes.
- Espacio inferior de ambas pestañas del motor; la bandeja no invade su espesor.
- Pasos y acceso a cabeza/arandelas/tuerca de la abrazadera proximal después de agregar nervios.
- Alivio localizado de los nervios de base frente a PCB/disipador del XL4016 izquierdo, conservando la columna y el soporte de la electrónica.
- Paso central del servo compatible con la envolvente del tornillo de referencia, sin recortar el tornillo para ocultar un choque.

La base continúa baja, Z=6…12 mm, y las longitudes de eslabones/tubos no cambian. El nuevo housing requiere actualizar masas y repetir las comprobaciones CAD; el resultado vigente está en [README](README.md).
