# AFR2-20261110-12 · Herramientas GOTO con arcos y curvas

Versión: 1.20.0 (subversión).

Pedido: selector de herramienta para generar/modificar arcos y curvas por clics y borrador de puntos o pistas.

Recta agrega un destino. Arco usa como inicio el último destino (o el origen) y requiere dos clics: punto de paso y destino final. Se calcula la circunferencia por tres puntos y el barrido que pasa por el punto intermedio. Curva usa dos clics: control y destino, con Bézier cuadrática. Editar selecciona un punto/control o tramo y el siguiente clic reubica el punto; la lista ofrece Editar y hay campos X/Y, selección de destino/control y conversión del tramo a Recta/Arco/Curva. Borrador elimina un comando al tocar su destino o tramo; borrar un control convierte su arco/curva en recta. Deshacer/Rehacer conserva hasta 30 ediciones locales.

Una misma geometría muestreada alimenta línea prevista SVG/3D, selección y reproducción. El avance se calcula por longitud acumulada de los segmentos de la polilínea, con rumbo constante Mecanum; longitudes y tiempos de curvas son aproximaciones, no una planificación dinámica calibrada. Se conservan ±50 m, máximo 64 comandos, 10 minutos y 50–500 mm/s. Arcos alineados/coincidentes no se aceptan al crear. Si borrar/reordenar deja inválido otro arco, la edición se conserva para poder corregirla o deshacerla; su reproducción queda bloqueada y la causa se muestra junto a las herramientas.

Las herramientas están encima del mapa y los errores permanecen visibles con Recorrido local cerrado. No se envían curvas al GOTO físico: se amplía el simulador local existente, manteniendo intactos los comandos AVR/ESP, sus permisos y odometría. La referencia de signos lateral de las ruedas visuales ahora usa MOTION_REFERENCE igual que el longitudinal.

Fuentes: Web/src/features/control/goto/{gotoSimulation.ts,GotoSimulatorPanel.tsx,GotoCommandList.tsx}; ControlMap.tsx, TopMap.tsx, ControlMapScene.tsx, controlMapTypes.ts y styles/{goto-simulator,control-map,control-refinements}.css. ATmega/ESP solo versión compartida por esta ficha. Solo fuente, sin tests, compilaciones, validaciones, navegador, paquetes, COM ni flasheos. No hay prueba visual ni de hardware. Sin commit/push.
