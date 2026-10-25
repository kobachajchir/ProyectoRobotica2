# AFR2-20261024-06 — Suelo, modelo final en Control y escala en metros

Pedidos humanos: la cuadrícula debe ser el suelo a la base de las ruedas;
Control también debe usar el modelo final; mapas y modelo deben conservar
unidades y proporciones del GLB. Se modifica presentación Web, no geometría CAD.

## Cambios

- `src/components/three/RobotModel.tsx`: cargador único del GLB final y Draco
  local para inspección y Control. Copias de escena con transformaciones
  independientes comparten geometría/materiales; no se agrega otra descarga ni
  otra copia del GLB al paquete. Reintentar elimina la carga rechazada compartida.
- Eliminada la normalización `2.4 / dimensiónMáxima`. El modelo conserva
  **escala1: una unidad Three.js equivale a un metro del GLB**. No se multiplican
  las dimensiones para llenar la pantalla; el encuadre cambia la cámara.
- El desplazamiento local del modelo es
  `(-centroX, -mínimoY, -centroZ)`. Se centra horizontalmente y el punto inferior
  queda en **Y=0**, donde está la cuadrícula. Antes se centraba también en Y y el
  suelo atravesaba la mitad de las ruedas. El GLB original no se modifica.
- El visor de inspección adapta distancia/objetivo de cámara a las dimensiones
  del modelo, sin escalar mallas. Su cuadrícula6m/12divisiones tiene celdas0,5m.
- `ControlMapScene.tsx` usa el mismo GLB en **Superior, Perspectiva y Seguir
  auto**. Superior pasa a cámara ortográfica con proporción del viewport; las
  otras vistas mantienen perspectiva. Zoom cambia cámara/frustum, no el robot.
  Cuadrícula16m/32divisiones: celdas0,5m. Ambos visores indican unidades.
- Conversión de pose conservada: posición Three `(−yMm/1000, 0, −xMm/1000)` y
  giro Y `thetaMrad/1000`. No se altera el protocolo ni se integran órdenes de
  mando para aparentar movimiento medido. Posición ausente/caducada y fuente
  Simulación siguen explícitas; sin extrapolación ni calibración inventada.
- `ControlMap.tsx` usa la escena3D en todas las vistas y conserva el SVG como
  **esquema de respaldo explícito** ante fallo del GLB/WebGL, con Reintentar.
  El SVG de respaldo sigue siendo esquemático, no una vista del GLB a escala.
  Cambiar cámara, zoom, centrado o reintentar conserva `onViewChange` y el corte
  de hold manual. Se elimina `MecanumChassis.tsx`, sin consumidores activos.
- `OrbitControls.tsx` admite altura de objetivo del modelo apoyado, para orbitar
  alrededor del robot y no de un origen elevado/centrado arbitrariamente.

La escala es la del exportado CAD, no una acreditación de cotas físicas,
calibración de odometría o registro entre origen de pose y ejes reales. Centrar
la geometría es una convención visual; no calibra la ubicación del controlador.
El GLB conserva SHA-256
`d10684704200caa4da0a6a98715e52abc446b9dee9cccdde32f33ee05fc927c7`.

## Estado y evidencia

| Módulo | Estado de esta ficha |
| --- | --- |
| atmega | Sin cambios; evidencia y bloqueos físicos de02 conservados |
| esp | Firmware sin cambios; paquete Web actualizado en data/dist |
| web | `npm run build:esp` completado: TypeScript/Vite/paquete/promoción, exit0 |
| CAD | Modelo y fuentes sin modificaciones |

Paquete: **1.450.617 bytes gzip**,7 activos. Estimación FS con reserva:
**1.540.096 bytes**, bajo el presupuesto2800KiB del candidato4m3m. Manifiestos
Web y ESP coinciden. Se conserva el layout solicitado de3MiB nominales.

Revisión estática de transformaciones/fuente y compilación autorizada. Sin
suite de tests, validadores independientes, render visual de navegador,
ejecución de firmware, imagen LittleFS ni flasheo. El cambio resuelve el origen
matemático del plano elevado; no se afirma comprobación visual posterior.

[Compilación y manifiesto](../documentation/builds/2026-10-24-06/README.md) ·
[Procedencia por archivo](../provenance/grounded-model-control-20261024.json).
