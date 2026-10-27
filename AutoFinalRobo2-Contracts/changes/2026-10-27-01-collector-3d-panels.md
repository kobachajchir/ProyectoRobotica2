# AFR2-20261027-01 — Robot recolector 3D y panel del brazo

> Rutas y reorganizacion de Control sustituidas por
> [AFR2-20261027-02](2026-10-27-02-preserve-control-arm-combined.md), por correccion
> humana: Control original intacto, solo Brazo individual y Simulacion conjunta.

27/10/2026. Pedido humano: implementar el plan del recolector y ampliar el
simulador a3D, con referencias reversibles, auto seleccionable/movil, motores
en ambos sentidos y brazo articulado en un panel separado.

## Antes / despues

Control reunia el auto y sus simulaciones, y Modelo3D era inspeccion del GLB.
Ahora Inicio y la barra ofrecen Robot y Brazo robotico. Comparten una escena3D
en memoria: auto actual, rig del brazo v02, pinza, deposito propuesto de cuatro
celdas y cubos por semilla. El Control real sigue en /control, accesible desde
Robot, conservando sus tareas. Modelo3D y Sensores mantienen sus rutas.

Se implementan orbita/zoom/vistas, seleccion por clic/lista, arrastre del auto,
ruedas individuales adelante/atras, Mecanum, ejes y pinza, sensor ideal, nube de
alcance vertical y estados de recoleccion/fallo. Fuera de foco/ruta/modal/STOP
se detiene la simulacion; al cambiar panel conserva posiciones y cubos.

RobotModel admite angulo de servo simulado independiente de lecturas fisicas;
la busqueda del pivote usa nombres CAD sanitizados como GLTFLoader. OrbitControls
admite enabled y targetX/Z opcionales; defaults existentes conservados.

## Productores / consumidores / wire

Solo estado local Web. No nuevos IDs, wire, capacidades ni campos de telemetria.
Mision consume ecos simulados, no centros/IDs del generador. El contacto de pinza
es geometria ideal de simulacion. No se envian ordenes fisicas del brazo.

## Estado por modulo

| Modulo | Estado |
| --- | --- |
| atmega | Sin cambios de esta ficha; HC-SR04/servo reales pendientes de comprobar |
| esp | Sin cambios; data/dist sin promover |
| web | Fuente implementada; sin tests, compilacion, validadores ni navegador |
| shared | Plan, limites, procedimiento futuro y ficha registrados en Contracts |
| cad | SVG propio de propuesta; maestros auto/brazo sin modificar; ensamble nativo pendiente |

La propuesta baja el sensor virtual a45 mm y agrega bandeja saliente; no son
medidas del montaje fisico. El informe de cinematica en curso no se encuentra
incorporado. La IK implementada cubre una rama de agarre vertical; colisiones
por volumenes aproximados, sin autocollisiones BRep, dinamica o fuerza real.
No se declara completado el plan fisico ni un ciclo de recoleccion comprobado.

Procedencia: archivos nuevos propios y reutilizacion interna detallada en el
[plan](../docs/ROBOT_RECOLECTOR.md). Identidad0.1.9 autorizada, tokens/primitivas
existentes, extension visual Propuesta VIS-039 y aspecto No verificado.

Sin tests/build/validaciones/navegador/flasheo/puertos serie ni promocion ESP.
Sin commit ni push en esta entrega. Cambios preexistentes de PCB y brazo preservados.
