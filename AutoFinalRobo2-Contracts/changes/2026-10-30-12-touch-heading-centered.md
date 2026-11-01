# AFR2-20261030-12 — Centro estable y rumbo Web/OLED

30/10/2026. Pedido del usuario: título centrado independiente de STOP, pill central Simulado solo sin conexión/ángulo solo conectado, transmitir rumbo a la pantalla táctil del auto y mostrar bitmap superior actual.

## Implementación

Web: cabecera con columnas laterales simétricas; grupo STOP/conexión conserva derecha. A900px o menos, título en fila propia centrada, sin depender de aparición de STOP. Pill de modo/rumbo centrada sobre la vista; conectado oculta etiqueta/icono y divisor, dejando ángulo solo. Error/bloqueo continúa en el pie, no se oculta como disponibilidad.

El envío robot.touch.ui deja de depender de modo robot y permanece activo durante la vista local conectada. Cierre, ocultación, desconexión y limpieza conservan liberación/caducidad. No se envían órdenes de giro físico. Se conserva protocolo de24sectores15grados: redondeo al más cercano, módulo24. Conectado el modelo3D y pill usan ese mismo rumbo; desconectado conserva giro local continuo. La rotación bitmap existente usa el mismo índice Q8; no cambia el recurso16x24,48bytes XBM LSB.

OLED030035 conserva introducción1000ms y presentación temporal de potencia1200ms. Después dibuja el bitmap girado y pie `Angulo: N grados` (N=índice*15), limpiando banda inferior0..127/y53..62 además del gráfico32..95/y12..51. Copia TypeScript vinculada con mismas coordenadas y texto; hash del C actualizado después del port. No cambia IDs, longitudes, campos ni EX01. Ángulo solicitado, no orientación medida. Memoria/pila de fuente modificada sin certificar.

## Procedencia por archivo y estado

- AutoFinalRobo2-Web/src/styles/manual-drive.css: centro de cabecera independiente de grupo derecho y formato de pill solo ángulo.
- AutoFinalRobo2-Web/src/features/control/manual/ManualDriveOverlay.tsx: presentación conectada durante giro local, índice compartido y etiqueta condicional.
- AutoFinalRobo2-Atmega/src/ui/afr2_screens.c: pie numérico actualizado del rumbo solicitado.
- AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.ts: copia literal del pie numérico.
- AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.sources.json: hash de fuente C portada, guard conservado.
- AutoFinalRobo2-Contracts/documentation/screens.json: campo/variante del pie de030035, identidad conservada.
- AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md: precedencia del contrato actual y actualización de bandas.
- AutoFinalRobo2-Contracts/changes/2026-10-30-12-touch-heading-centered.md: pedido, adaptación y estados.

Autorías previas conservadas. ATmega/Web solo fuente; ESP sin cambios. Sin tests, validadores, generador OLED, compilación, navegador, teléfono, COM, promoción, flasheos ni prueba física. Solo lectura manual, edición, cálculo de hash y render solicitado del recurso binario para mostrar en chat, a escala12x, en D:/ProyectoRobotica2/.tmp/touch-car-current.png; esa imagen no es captura de la placa ni ensayo del renderer. Paquetes/cargas actuales no se actualizan por esta entrega.
