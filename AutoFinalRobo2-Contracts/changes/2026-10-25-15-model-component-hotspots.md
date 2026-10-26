# AFR2-20261025-15 — Componentes interactivos del modelo 3D

Pedido humano: identificar piezas mediante puntos fijados al modelo y mostrar
su explicación en una ficha flotante estable, con acceso a controlar, cierre
por X/clic exterior y consistencia visual con el producto.

## Resultado en fuente

Once puntos con numeración, nombre accesible y alternativa por desplegable.
Ficha única dentro del visor para SG90, HC-SR04, cuatro ruedas, ATmega, ESP,
OLED, encoder y sensor de línea inferior. La proyección sigue la cámara y
separa selectores próximos mediante líneas al punto original. La ficha queda
en un rincón fijo; girar/zoom no la cierra, un clic exterior sí. X/Escape y
retorno de foco incluidos. Escritorio y móvil consumen las primitivas/tokens
del manual0.1.9 y la familia de iconos propia.

Los accesos abren la tarea precisa: Distancia y servo, Línea IR, Prueba de
rueda con FL/FR/RL/RR preseleccionada, Control, Wi-Fi o Pantalla OLED.
Navegar no inicia acciones físicas. Se conserva pose estática del visor,
modelo/escala/materiales y los otros consumidores de `RobotModel`.

| Antes | Después | Motivo |
|---|---|---|
| Visor sin identificación interactiva | Puntos proyectados y ficha de una pieza | Explorar el ensamble sin salir de la vista |
| Acercarse a piezas pequeñas sin selección textual | Selectores44px y desplegable con los once nombres | Acceso táctil y por teclado |
| Rutas abrían su primera tarea | Entradas de Sensores y rueda individual por parámetros | Llegar directamente al control relacionado |
| Aviso hablaba de pieza/PCB futuras | Aviso describe el ensamble actual con límites físicos | Alinear la explicación con el GLB vigente |

## Archivos y alcance

- Web `src/features/model3d/Model3DPage.tsx`: integración del visor anotado y
  texto actualizado; conserva pausa, restablecer, reintento y frontera de error.
- Web `src/features/model3d/ModelInspection.tsx`: cámara/proyección, selectores,
  separación, ficha local, cierre que distingue clic/arrastre y enlaces.
- Web `src/features/model3d/modelComponents.ts`: catálogo y referencias CAD.
- Web `src/styles/model-inspection.css`: composición adaptativa y tokens.
- Web `src/features/sensors/SensorsPage.tsx`: entrada por `seccion`.
- Web `src/features/control/ControlPage.tsx`: tarea/rueda inicial desde el enlace.
- Contracts: [contrato del visor](../contracts/WEB_MODEL_INSPECTION.md), registro,
  decisión VIS-036 y [cobertura](../docs/design-system/coverage-20261025-15.json).

## Estado real

| Módulo | Estado de esta entrega |
|---|---|
| ATmega | Sin cambios; mantiene la fuente/build anterior |
| ESP | Sin cambios; `data/dist` conserva su paquete anterior |
| Web | Fuente implementada; sin compilar, ejecutar ni empaquetar |
| CAD | Sin cambios en fuentes ni exportados; GLB leído como insumo |
| Contracts | Comportamiento, procedencia y cobertura documentados |

No se ejecutaron tests, compilaciones, validadores, navegador, renders ni
flasheos, conforme a la instrucción humana. No se acredita revisión visual,
contraste, selección en dispositivo ni precisión física por escribir el código.
Las referencias ESP/OLED/encoder son inferencias documentadas del exportado
actual; las piezas OLED/encoder pertenecen al mismo nodo. Sin dependencias
nuevas, cambio de protocolo o promoción al ESP. Cambios concurrentes preservados.
