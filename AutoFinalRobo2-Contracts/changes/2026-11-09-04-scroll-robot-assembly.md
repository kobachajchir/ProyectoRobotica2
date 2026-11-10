> **Presentación sustituida por AFR2-20261109-07:** [banner de producto](2026-11-09-07-cinematic-assembly-banner.md), con modelo flotante, fondo de Infraestructura y fade de piezas. El alcance y estado de la primera entrega que sigue se conservan como antecedente.

# AFR2-20261109-04 — Armado del robot en Explorar

Versión: 1.10.0 (subversión). Función Web nueva compatible, integrada sobre `origin/main` (`e47be73`). ID renumerado a -04 porque el remoto ya usa -01 para radar/servo/Wi-Fi. Se conserva el contenido de ambos registros.

Pedido humano: implementar el recorrido de ensamblaje descrito en el chat, ubicarlo en Explorar y terminar con el robot tomando una caja. Scroll reversible con piezas, cámara y textos coordinados.

## Comportamiento incorporado en fuente

- Nuevo destino **Explorar → Armado del robot**, `#/armado`, cargado bajo demanda. La navegación común también lo incorpora en Inicio y Más; hay otro acceso desde Modelos.
- Escena fija durante el scroll nativo del documento. Todo el estado depende de un único progreso reversible: componentes, cámara, textos, apertura de pinza y ubicación del cubo. Sin reproducción automática ni integración de velocidades.
- Los gestos horizontales de rueda/trackpad sobre la escena avanzan el recorrido; se respetan el zoom y el desplazamiento de la lista de capítulos. El scroll vertical y el desplazamiento táctil vertical son nativos.
- Trece capítulos: placa inferior, separadores/fijaciones, motores, placa superior, control/alimentación, sensores/panel, ruedas, montura, J1/J2, J3/J4, muñeca/pinza, presentación y agarre.
- Placas/componentes desde arriba, ruedas desde los laterales, traslación y rotación de fijaciones. El brazo usa los nodos articulados y las transmisiones del descriptor V17, incluyendo ambas mordazas.
- Textos con entradas laterales alternadas, opacidad, sombra suave y difuminado breve. La cámara describe una órbita y termina en vista frontal isométrica mientras el brazo se orienta hacia el cubo.
- Agarre ilustrativo del cubo existente de 50 mm. La pose de contacto determina su posición y alinea sus caras con las mordazas. Desde el cierre, conserva su transformación respecto de la herramienta durante la elevación; al retroceder, se apoya antes de abrir la pinza.
- Progreso, Anterior/Siguiente, capítulos y transcripción completa. Movimiento reducido del sistema y selector local: etapas estáticas, cámara fija y sin movimiento continuo del agarre.
- Carga de modelos, recuperación de errores y alternativa de lectura por etapas si no está disponible el 3D.

## Fuentes y alcance

Se reutilizan `AutoFinalRobo2.glb`, `AFR2_Brazo_v17_Blanco.glb` y `AFR2_Cubo_50mm.glb`, junto con el descriptor `PVC25_V17`, sus marcos y posición de montaje. La vinculación del brazo se adapta de `ArmVisualAdapter.tsx` para agregar desplazamientos de presentación sobre una copia independiente.

El auto conserva rígida cada ocurrencia CAD con sus materiales. Las colecciones exportadas de separadores, fijaciones de placas y prisioneros se separan en buffers locales a esta vista por conectividad/posición y proximidad de envolventes; no se editan los buffers originales. Los tornillos inferiores/superiores se distinguen por su altura respecto de las placas. El eje visual de los prisioneros se infiere de su envolvente. Estas clasificaciones no sustituyen una BOM individual de tornillería.

El brazo se ensambla por grupos del GLB (articulaciones/transmisiones); no se anima individualmente cada tornillo interno. Los CAD y PDFs permanecen sin cambios. No se agregan assets descargables, bibliotecas ni dependencias. Se renderiza bajo demanda y se liberan los buffers/materiales creados para esta presentación.

La vista no modifica el estado de movimiento, los puntos guardados o la sesión del simulador, ni envía órdenes a las placas. La coreografía de agarre es ilustrativa; no es un recorrido físico comprobado ni una secuencia de montaje/fabricación aprobada. No se ejecutaron cálculos de colisión, cargas o estabilidad.

## Identidad visual

Manual general **0.1.11**, con autorización previa VIS-062 para aplicar el borrador a la Web. Fuentes: tokens, PageHeader, Button, lenguaje de Infraestructura y SimulationViewport. Se conservan la paleta compartida del entorno y los materiales del catálogo. El recorrido por scroll es una extensión Propuesta; [matriz de cobertura](../docs/design-system/coverage-20261109-04.json).

## Archivos

En `AutoFinalRobo2-Web/src/features/assembly/`:

- `AssemblyPage.tsx`: presentación, textos, estados y controles.
- `useAssemblyScroll.ts`: progreso, gestos y altura de cabecera.
- `assemblyTimeline.ts`: capítulos, intervalos, curva y poses.
- `assemblyCar.ts`: copia del auto, agrupación y fijaciones.
- `assemblyArm.ts`: copia del brazo, marcos y articulación.
- `AssemblyScene.tsx`: modelos, cámara, iluminación y cubo.

También `src/styles/assembly.css`, `src/app/App.tsx`, `src/app/navigation.ts`, `src/features/models/ModelsPage.tsx` y `src/components/ui/PageHeader.tsx` en el mismo módulo.

## Estado real

| Módulo | Estado de esta entrega |
| --- | --- |
| atmega | Solo versión compartida; lógica sin cambios por esta ficha. |
| esp | Solo versión compartida; no se promueve paquete Web ni LittleFS. |
| web | Fuente implementada. Sin tests, compilación, validadores, navegador ni ejecución de la animación. |
| CAD | Sin cambios en maestros, modelos publicados ni láminas. |
| Loader | Sin cambios. |

Por instrucción del usuario no se ejecutaron pruebas, builds, validaciones, renders, apertura de puertos ni flasheos. Encuadre, trayectorias, separación de fijaciones y contacto visual de mordazas quedan sin observar en ejecución. No se acredita rendimiento o interacción en dispositivos reales.
