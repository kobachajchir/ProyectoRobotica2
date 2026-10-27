# AFR2-20261026-05 — Prototipo paramétrico del brazo PVC32

26/10/2026. Pedido expreso del usuario: modelar en Fusion un prototipo del antropomorfo con eslabones de caño PVC32, articulaciones/housings impresos rojos, motores, rodamientos y control dentro de la base. Pieza compartida con el AGV: cubo de 50 × 50 × 50 mm y requisito de masa de 100 g. La documentación de referencia no se trató como una orden de ejecutar pruebas.

## Antes/después

Se incorpora un diseño independiente del auto en `AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/`, con F3D nativo, fuente de construcción, CSV de parámetros, guía y fuentes dimensionales/comerciales. Documento `AFR2_Brazo_PVC32_Prototipo_v01`, guardado en Fusion y exportado localmente; 88 parámetros de usuario, 96 componentes contando la raíz y 10 uniones de ensamble. Las imágenes de autoría se guardan en renders ignorado por Git.

La propuesta conserva 130/120 mm entre ejes, hombro a120 mm y muñeca–TCP75 mm. Caños Ø32/Ø28,3 con cortes iniciales80/70 mm. La foto del usuario indica también pared3,2 mm, incompatible con esos diámetros: se modela espesor derivado1,85 mm y se conserva la foto en reference. Las articulaciones y encastres se representan en PETG rojo.

Seis ejes con GX3240MGV1 de referencia en hombro con reducción2:1, DS3218MG en codo, dos MG946R y tres MG90S incluyendo pinza. Apoyos608-2Z y625-2Z nominales; Nano, PCA9685 y tres XL4016 representados dentro de la base. Se documentan fabricante, variante local, cotas propuestas e inferencias. Los dientes, splines y patrones comerciales son referencias geométricas; no son planos finales de fabricación.

## Productores/consumidores y secuencia compartida

La pieza50 mm/100 g queda como requisito geométrico y metadato, no como carga admisible demostrada. El concepto sigue siendo entrega del AGV a una estación fija y manipulación virtual con el brazo. No se implementaron docking, bandeja, secuencia de paletizado, mensajes entre equipos, control de motores ni simulación coordinada. No cambia wire, IDs de comandos, versión de firmware o recursos Web.

## Estado por módulo

| Módulo | Estado de esta ficha |
| --- | --- |
| Atmega | Sin cambios |
| ESP | Sin cambios |
| Web/OLED | Sin cambios |
| CAD | Prototipo geométrico nativo guardado, componentes y expresiones editables; pendientes revisión del usuario, interfaces finales y cálculos |
| Contracts/shared | Registro y datos coherentes de pieza/alcance; sin nuevo protocolo |

No se ejecutaron tests, builds, validadores, estudios de esfuerzo/pandeo/torque, detección de interferencias, barridos cinemáticos ni pruebas físicas. La composición visual durante el modelado no acredita funcionamiento mecánico. Se preservaron los cambios previos de KiCad y los documentos/Gerber ajenos a esta tarea. Sin commit ni push solicitados.

## Fuentes de la entrega

- [Guía de edición y límites](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/README.md).
- [Fichas dimensionales y oferta argentina](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/FUENTES_Y_COMPONENTES.md).
- [Maestro F3D](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_Prototipo_v01.f3d).
- [Manifest y referencia del documento Fusion](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/modelo_prototipo.json).
