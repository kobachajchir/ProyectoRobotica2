# Estado vigente — AFR2-20261025-11

25/10/2026. [Control directo7](../contracts/DIRECT_CONTROL_7.md),
[matriz](ALINEACION_MAESTRO.md), [Web11](../documentation/builds/2026-10-25-11/README.md) y [firmwares01](../documentation/builds/2026-10-25-01/README.md).

## Web

[Control táctil](../contracts/WEB_TOUCH_SIMULATION.md): modo local sin robot,
vista superior, dirección y flechas compartidas. Giro ideal de ruedas en
táctil/GOTO/secuencias, sin feedback físico. Reconectar no promueve sesión local.
Captura DOM sobre Canvas, foco inicial/teclado modal y flecha activa animada03.
Corregido falso fallo de Canvas.fallback; entrada local independiente de carga3D.
Chevrones09 conservados; modelo manual10 mayor por cámara60% frente52%.
Giro11 con dos dedos/Q-E local, rumbo persistente y fase de ruedas; sin yaw físico.
Web compilada/promovida11; nuevo gesto/captura sin comprobar en teléfono.

Web0.2.0 envía acciones directas: no permiso, adquisición, renovación o liberación.
Se elimina controlLease y la preparación de control en movimiento/modos/pruebas/
calibración/secuencias. Estado/STOP/visibilidad/soltar siguen supervisados.
Presencia interactiva cada500ms; no activa movimiento. Paquete compilado/promovido.

## ESP

Activos Web11 promovidos a data/dist; sin recompilar firmware en esta entrega.

Firmware0.9.0, controlMode=direct y negociación exacta EX01v7. Se elimina toda
concesión/token de acceso. WEB_PULSE0x12 retransmite edad de presencia a AVR cada500ms.
Sesión/transacción/generación/programas siguen correlacionados; desconexión pide STOP.

## ATmega

Firmware0.9.0 compilado: no Acquire, tokens de acceso o bandera de habilitación.
Acciones directas con límites/calibración/estado ocupado; operación Web caduca a1500ms.
Retiradas opciones OLED2/3/56; Control manual4ítems/Sistema7.59ScreenCodes/copia Web
conservados. Hall y memoria0; no habilita funciones físicas sin sus requisitos.

## Pendientes

Sin tests, validadores independientes, navegador, flasheo o pruebas físicas.
Calibración/cableado/Hall, memoria dinámica, flash/SPI y planos CAD siguen pendientes.
Copias anteriores en history/alignment15 y history/direct-control7 son históricas.
