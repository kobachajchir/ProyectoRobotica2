> **Ampliación AFR2-20261109-10:** [etapa7 Nuestros agregados y circuito compacto](2026-11-09-10-assembly-additions-battery-circuit.md), reutilización de piezas en navegador y zoom desde mitad de la nueva etapa12. La entrega inicial del banner que sigue se conserva como antecedente.

# AFR2-20261109-07 — Banner cinematográfico para el armado del robot

Pedido humano: presentar el armado como un banner de producto, con el modelo flotando y luciendo sus materiales, tomando la foto de la notebook adjunta como referencia de composición y usando el fondo de Infraestructura. Las piezas deben aparecer con fade de opacidad al scrollear. Sustituye la presentación con aspecto de simulador de AFR2-20261109-04 en `#/armado`.

Versión: **1.10.3 (subversión mínima)**. Ajuste de presentación de la función existente. ATmega/ESP solo cambian su versión compartida.

## Cambios en fuente

- El recorrido comienza directamente con un banner grande, con el título «De las piezas al movimiento», nombre de la sección y enlace al modelo. Se eliminan la cabecera separada, la barra de herramientas y la tarjeta de controles del área de presentación.
- El fondo reutiliza `--infra-hero-*`: degradados suaves, halos, tinta y acentos adaptados al tema. Sin cuadrícula decorativa, suelo ni escenario de simulador.
- Canvas transparente propio, separado de `SimulationViewport`. Los modelos quedan suspendidos sobre el fondo HTML. El entorno de reflejos se produce en el navegador con `RoomEnvironment` y PMREM; luces de estudio principal, relleno frío y recorte cálido. No se descargan panoramas ni se agregan dependencias.
- Cámara más cercana, deriva suave durante el montaje y órbita final conservada. En ancho de banner >=760px el encuadre se desplaza al lado opuesto del texto, con mezcla progresiva entre capítulos. En formatos estrechos el texto ocupa la zona superior y el producto queda debajo.
- Las ocurrencias del auto y los grupos articulados del brazo reciben materiales clonados por pieza: fade real de opacidad, independiente de sus vecinas y reversible con el scroll. Desplazamientos de llegada más cortos. La placa inferior comienza su aparición antes del inicio visible de la línea de tiempo para que ya haya una pieza en el primer encuadre.
- Se conservan geometría, texturas y propiedades PBR de origen. Solo se editan las propiedades de presentación de las copias. Se liberan materiales derivados, buffers propios y mapa de reflejos al desmontar.
- Títulos y descripciones se superponen sin recuadro, alternando el espacio libre con entrada, salida y difuminado. El banner conserva únicamente una indicación de scroll, una línea de avance manipulable y el contador de capítulo.
- Capítulos, Anterior/Siguiente y selector de movimiento reducido pasan a «Etapas y opciones del recorrido», debajo del banner. La transcripción y los enlaces al modelo/simulación siguen disponibles.
- Movimiento reducido: estados estáticos por etapa, cámara fija y composición estable; se conserva el acceso nativo por teclado y la lectura textual. No se añaden loops decorativos ni reproducción automática.

La referencia de notebook se usa por la suspensión visual, el protagonismo del objeto y el espacio libre. No se incorpora la fotografía a la Web ni se alteran las piezas para imitar el producto de la referencia.

## Fuentes

En `AutoFinalRobo2-Web/src/features/assembly/`: `AssemblyPage.tsx`, `AssemblyScene.tsx`, `assemblyCar.ts`, `assemblyArm.ts` y nuevo `assemblyMaterials.ts`. Estilos en `src/styles/assembly.css`. Se reutilizan los GLB y el descriptor V17 existentes. Sin editar los renderers del simulador, la OLED, los CAD o los recursos generados.

Manual visual general0.1.11 autorizado VIS-062; fuentes canónicas de esta adaptación: el fondo de Infraestructura indicado por el usuario, sus tokens y la referencia adjunta. El banner y la composición de producto son una extensión Propuesta con implementación autorizada; [cobertura](../docs/design-system/coverage-20261109-07.json).

## Estado real

| Módulo | Estado de esta ficha |
| --- | --- |
| web | Fuente de presentación y versión editadas; sin compilar ni ejecutar. |
| atmega / esp | Solo versión compartida. No se modifica su lógica por esta ficha. |
| CAD / Loader | Sin cambios. |

No se ejecutaron tests, builds, validadores, navegador, renders, generadores, comparaciones visuales, promoción de paquetes o flasheos, por instrucción del usuario. Iluminación, encuadre, orden de transparencias y legibilidad en dispositivos reales quedan sin observar en ejecución. La coreografía de agarre continúa siendo ilustrativa y no envía comandos al robot.
