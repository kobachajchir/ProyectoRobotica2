# AFR2-20261024-07 — Control, secuencias OLED y sistema

Pedido humano: corregir apoyo del robot, homogeneizar Control, editor de ancho
completo y detalles debajo, parada global condicional, conexión automática al
mismo origen, reinicios/compilaciones/boot, desarrollador oculto y documentación
Web/ESP/ATmega. Skills solicitadas Emil Design Eng, Apple Design y Visual
Identity System; además afr2-screen-authoring para OLED.

## Interfaz y geometría

| Before | After | Why |
| --- | --- | --- |
| Cajas de piezas giradas para hallar suelo | Vértices reales, Box3 precise=true; resultado cacheado por asset | Las esquinas de cajas aproximadas podían quedar bajo las ruedas y elevar el modelo |
| Transformación de todos los spans del summary | SVG Chevron compartido, solo el icono gira | Los títulos permanecen horizontales |
| Propiedades laterales/modal y ancho reservado | Editor de ancho completo, propiedades inmediatamente debajo de la pista seleccionada | Mantener relación entre bloque y edición sin perder ancho |
| Paddings/bordes mezclados | Ritmo24px escritorio/20px móvil, separaciones8/12/16/24 y tokens semánticos de borde | Composición consistente con la identidad propia |
| Mapa con selector de pose real sin feedback | Solo simulación local, con unidades del GLB y rótulo explícito | No representar orientación física desconocida |
| Tomar mando y varias paradas/cancelaciones | Adquisición al iniciar acción y STOP único de cabecera cuando corresponde | Menos pasos y una acción reconocible de interrupción |
| URL/botón de conexión | ws/wss al host actual y reconexión automática | La aplicación la sirve el propio ESP |

No se modifica el GLB ni sus materiales/unidades. Precisión del apoyo calculada
desde fuente; no se certifica resultado visual tras el cambio ni cotas físicas.
Se conserva el modelo final, WebP, Draco local y presupuesto LittleFS3MiB.

STOP aparece durante simulación, transferencia/ejecución de secuencia, mando
activo o parada sin confirmar. Una simulación aislada sin enlace puede detenerse
sin anunciar parada física. Las acciones reales conservan lease1500ms renovado,
TTL y corte por foco/visibilidad/desmontaje; soltar durante adquisición anula el
gesto pendiente. No se conserva un comando para ejecutarlo luego de reconectar.
No hay selector de auto real. Se mantiene Enviar al robot con requisitos reales.

Desplegables nativos conservan teclado y usan chevrón común (`select:open`);
Disclosure gira solo su SVG. Reduced motion y modalidad teclado suprimen la
transición. Sin instalar una librería de animación o iconos adicional.

## Sistema y desarrollador

Nueva región Sistema dentro de Conexión/apariencia: información consultada de
placas, arranques observados y comandos de reinicio ATmega/ESP. Info ausente se
muestra sin inventar valores. Los reinicios requieren petición explícita y
estado detenido; el ACK de programación no se presenta como arranque terminado.

Triple toque consecutivo (intervalos menores900ms) en el robot de Inicio abre
`#/desarrollador`. Conserva AppShell y título propio, sin enlace activo en la
barra. La ruta es discreta, no autenticación ni protección de acceso. Contenido
y estructura adaptados de `D:/ST2026-Radal/src/presentation/components/modals/developer-modal.tsx`
y datos de `src/shared/app-info.ts`: Koba Chajchir, enlaces y contacto, con marca
AutoFinalRobo2. No se copia el logo/paleta de Radal. Referencias solo leídas.

Se retira Estado y capacidades del robot de Inicio. Documentación de producto
reescrita en secciones Web/ESP/ATmega: comandos vigentes, navegación, módulos,
calibración, seguridad, modelo, distribución, shield y límites reales.

## ATmega, ESP y contratos

Extensión EX01v4: nueva pantalla0x03002c, etiquetas de segmento y servicios de
sistema. [Formato y semántica](../contracts/EX01_SYSTEM_SEQUENCE_4.md).
Nombres/iconos originales AFR2 en PROGMEM; snapshot pequeño preparado en main,
renderer puro y único I2C Manager. Barra por segmento ejecutable, rellenados
completados, contorno pendientes, icono/nombre y tiempo configurado. Volver y SW
largo conservan sus reglas. No se renumeró ni eliminó el árbol de menús.

Eventos BT01 en UART y system.boot en WebSocket, distinguiendo arranque recibido
de identidad consultada. Info de compilador/fecha/revisión/versiones; reset
diferido. USART1 de Mega D18/D19 y cableado del shield sin cambios.

## Estado real y límites

| Módulo | Entrega | Evidencia pendiente |
| --- | --- | --- |
| atmega | firmware, comunicación y biblioteca compilados | Secuencia, OLED, boot/reset y movimiento en placa no ejecutados |
| esp | firmware candidato compilado; puente/info/reset/boot implementados | UART, radio, heap bajo carga y reinicios reales no probados |
| web | TypeScript/Vite/paquete/promoción compilados | Navegador denegó localhost; revisión visual/interactiva no ejecutada |
| CAD | GLB maestro sin cambios | Contacto visual y cotas físicas no certificados |

**AFR2_SEQUENCE_MEMORY_APPROVED=0 y AFR2_HALL_CONNECTED=0 se conservan.**
La pantalla y el editor no aprueban memoria/pila ni calibración. Ejecutar una
secuencia física sigue bloqueado por esos requisitos. No se intentó eludir la
denegación previa que impedía activar el flag de memoria.

AVR final: .text53182 + .data452 = **53634bytes flash**;
.data452 + .bss3834 + .noinit9 = **4295/8192bytes SRAM estática**.
No es una cota de pila total ni margen certificado. ESP: **45476/81920bytes
RAM estática**, **419945/1044464bytes programa** del candidato.
El primer build ESP advirtió dos condicionales en una línea; separados y
recompilados sin advertencias finales. Builds y tamaños finales en
[evidencia](../documentation/builds/2026-10-24-07/README.md).

La herramienta de navegador rechazó abrir localhost por falta de autorización.
No se usó otra superficie ni automatización alternativa para eludirlo. Se
completó fuente/compilación/documentación y se dejó estética/interacción como
No verificada. Sin suite de tests, validadores independientes, monitor serie,
imagen LittleFS, flasheo, reinicio ni pruebas físicas.

[Adaptaciones y hashes por archivo](../provenance/control-system-sequence-20261024.json).
