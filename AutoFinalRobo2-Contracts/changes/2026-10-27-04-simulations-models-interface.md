# AFR2-20261027-04 — Simulaciones y Modelos 3D

27/10/2026. Pedido humano de reorganizacion visual: reunir tres simulaciones,
mantener Control aparte, pluralizar Modelos3D y preparar selector Auto/Brazo.

## Comportamiento y presentacion

- Simulaciones en `/simulaciones?tipo=auto|brazo|conjunta`, con pills compartidas
  TaskSections. Tres estados locales independientes. Las rutas /brazo y
  /simulacion redirigen a la pill correspondiente; Control conserva /control.
- Auto muestra solamente el auto y sus controles de movimiento/motores; Brazo
  muestra el rig individual; Conjunto conserva cubos, posiciones, deposito,
  Buscar y recolectar / Solo barrer. Sin cambios del Control fisico/tactil.
- Articulaciones y gripper son bloques separados. Eje activo independiente del
  componente seleccionado: apertura/cierre no deselecciona J1…J6. Cerrar/Abrir
  ocupan el ancho con flex y justify-content:space-evenly.
- Las articulaciones aceptan clic y arrastre izquierdo o derecho. Se captura el
  puntero, se pausa la orbita durante el gesto y el angulo pasa por los limites
  y filtro de trayectoria existentes. Soltar/cancelar/perder foco libera el gesto.
- Campos de simulacion: titulo izquierdo, numero editable arriba a la derecha y
  barra debajo. Numero/barra comparten valor. Motores en pills Atrás / nombre /
  Adelante. Sin reestilizar sliders de Control ni cambiar productores fisicos.
- Matriz homogenea lateral persistente, con seleccion de articulacion, pinza o
  mundo/chasis segun vista; angulo, seno, coseno y opcion simbolica. Valores del
  mismo estado que mueve el modelo, sin animacion editorial ni datos aleatorios.
  Modelo matematico provisional: no se atribuye al informe final pendiente.
- Se retiran Brazo individual, Detener brazo, Modelo y restricciones y las
  referencias de las simulaciones. Se conserva el selector J1…J6 y aro activo.
  La envolvente calculada actual sigue disponible; el aspecto definitivo de la
  burbuja espera la imagen del usuario, sin inventar esa referencia.

## Modelos 3D y activos ESP

Inicio/barra/titulo dicen Modelos3D. En la misma ventana, pills Auto/Brazo robotico
mantienen selector de componentes y Mostrar/Ocultar referencias. Auto conserva
su GLB, referencias y fichas. Brazo muestra un estado pendiente del archivo final,
con el mismo marco/herramientas; no se generan anclas, fichas ni modelo final.
El estado de visibilidad de referencias se conserva al alternar modelos.

Solo se monta el visor activo. No se agrega GLB, textura, decoder ni dependencia.
Se conserva Draco local y el empaquetado existente. Politica actual de
tools/web-budget.mjs: presupuesto2800 KiB, bloques8192 y reserva64 KiB, sobre
LittleFS nominal3 MiB candidato. Esto no demuestra espacio libre real.
Cuando llegue el brazo: optimizar su exportado, agregarlo a la lista explicita
de activos de Vite y acreditar el presupuesto del paquete completo con permiso
de compilacion/verificacion. No se afirma que ambos modelos ya entren en el ESP.

## Estado y procedencia

| Modulo | Estado |
| --- | --- |
| atmega | Sin modificaciones de esta ficha |
| esp | Sin cambios ni promocion de data/dist |
| web | Fuente de interfaz implementada; sin compilar ni comprobar en navegador |
| contracts | Ficha, estado y cobertura VIS-041 actualizados |
| cad | Sin cambios; modelo final del brazo y burbuja de referencia pendientes |

Fuente propia. Reutilizacion interna: TaskSections, Button, ModelInspection,
cadena v02 y simulador Mecanum. Sin tocar cambios concurrentes de CAD/RobotModel.
Sin tests, builds, validadores, navegador, flasheos, commit ni push.

Escenarios pendientes de autorizacion: cambio entre tres estados, matrices en
movimiento, arrastre izquierdo/derecho/cancelacion, gripper con eje seleccionado,
sincronizacion de campos, temas/movil/teclado, selector de modelos y carga/error.
