# AFR2-20261101-02 — Simulador UI y flujo AVR actual

Pedido del usuario: revisar pantallas/opciones del ATmega, corregir el simulador
Web según giro/corto/largo y entregar un árbol de flujo.

## Entrega en fuente

- `uiSimulation.ts`: revisión de `Afr2Ui_Apply/Open/go_back/back`, catálogo,
  `Afr2App` y respuestas `afr2_app_wire`. Retorno contextual, selección/scroll,
  corto Inicio→Menú, largo Inicio→Rápidas, siete tarjetas limitadas, flags,
  editores, selector de motor circular, navegación de hermanas sin envolver,
  prueba encoder, capacidades/conexión por páginas y modo Wi-Fi.
- Sí/No obligatorio al salir con cambios. Guardado ficticio asíncrono y estado
  de espera; No no evita el retorno pendiente. Cuatro borradores Hall, asistente
  de selección/conteo, cancelación de intento preservando otros motores.
- Menús dinámicos resueltos con el mismo snapshot del dibujo: recuento de redes,
  Conectar/Desconectar/Reintentar, filas ocultas sin cambios y etiquetas de toggle.
  Scan→aviso→listado, teclado y entrega Web→Configurar, consulta de credenciales
  locales→Datos, borrado al salir/30 s. Selector de cliente Web ficticio para
  recorrer también el caso No hay clientes. No conexión al auto.
- `uiSimulationFlow.ts` y `UiSimulationTree.tsx`: guía contextual y árbol
  desplegable de los nodos del catálogo compartido; referencias evitan ciclos.
- `UiSimulator.tsx`: usa el umbral largo configurable, el flujo y la guía nueva.
  `EncoderControl.tsx`: corrige textos históricos que limitaban retorno a Volver
  o llamaban Menú al largo. Conserva emisión al soltar y cancelación del gesto.

Árbol completo, matriz de gestos, excepciones y alcance:
[UI_SCREEN_FLOW.md](../documentation/UI_SCREEN_FLOW.md).

## Hallazgo de documentación histórica

El C actual no usa corto para volver desde IR o Ángulo servo. Radar/Numérica
abren Opciones radar por corto. El simulador sigue ese C; las notas antiguas
de corto=volver/largo=menú se señalan como históricas, sin modificar firmware.

Identidades compartidas sin altas ni renumeración. Se reportan variantes
030044 (selector de motor) y 030040 (potencia), en lugar del código del menú
de fondo. Catálogo y overlay enlazan esta revisión. Recursos/raster y sus hashes
revisados se conservan; no se ejecuta el generador ni se altera su guard.

## Estado real y límites

ATmega: leído, sin cambios por esta ficha. ESP: sin cambios ni promoción.
Web: implementación en fuente revisada por lectura; servicios y pulsos Hall
ficticios, tiempos de respuesta de demostración, sin emular UART/EEPROM/física
ni todas las causas de fallo. Los movimientos usan guardas de ejemplo: PWM
manual no exige pose/HC, línea y GOTO sí dependen de sus requisitos preparados.

No se ejecutaron tests, compilaciones, validadores, generadores, Vite, navegador,
COM, flasheos ni pruebas físicas, según instrucciones del proyecto. No se
certifica ejecución, paridad diferencial de framebuffer ni memoria dinámica.
El próximo predev autorizado por el usuario seguirá copiando recursos AVR.
Se conservan modificaciones locales anteriores; no se crea commit ni se publica.

Autorías previas conservadas. Adaptación propia por archivo:
[procedencia](../provenance/ui-simulator-avr-flow-20261101-02.json).
