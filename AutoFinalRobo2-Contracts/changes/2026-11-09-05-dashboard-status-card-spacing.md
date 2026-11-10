# AFR2-20261109-05 — Tarjeta de estados del dashboard más amplia y centrada

Pedido humano: aprovechar el espacio libre a la izquierda y debajo de la tarjeta que alterna las partes/sistemas del robot, ampliándola y centrando mejor icono y texto.

Versión: **1.10.1 (subversión mínima)**. Ajuste de presentación compatible; versión compartida en `afr2_build_info.h`, `Web/package.json` y las dos entradas raíz de `package-lock.json`.

## Composición en fuente, ScreenCode 0x010001

| Elemento | Antes, código vigente | Ahora |
| --- | --- | --- |
| Tarjeta | (62,27), 64×16 px | (54,27), 72×22 px |
| Caja de icono | 14 px, y28 | 18 px, y29 |
| Texto | Fuente 7×10, y30 | Misma fuente, y33 |
| Separación icono/texto | 4 px | 4 px |
| Borrado de MODO | x0..59 | x0..51 |

El borde derecho se mantiene en x125. La tarjeta gana 8 px a la izquierda y 6 hacia abajo; su borde inferior queda en y48. El grupo icono+texto se centra horizontalmente usando el ancho real de cada etiqueta; las alturas de 18 y 10 px se centran individualmente sobre el eje vertical y38 de la tarjeta. El icono mantiene su proporción mediante la primitiva existente.

El borrado de la columna de modo deja de invadir el nuevo marco. `SECUENC`, el nombre más largo, mantiene su fuente y ubicación. La rotación de estados borra solo dos bandas interiores que respetan las esquinas redondeadas. Se conservan la cabecera, el pie y los tiempos de rotación/pausa, prioridad de fallo y navegación. El anillo de pulsación mantenida conserva su prioridad de overlay en la esquina inferior derecha; al soltar se conserva el repintado completo existente.

## Fuentes vinculadas y procedencia

- `AutoFinalRobo2-Atmega/src/ui/afr2_dashboard.c`: coordenadas, tamaño, centrado y borrado parcial.
- `AutoFinalRobo2-Web/src/features/oled/dashboardRenderer.ts`: mismas constantes y fórmulas con truncamiento entero.
- `AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.sources.json`: hash del renderer C actualizado después de adaptar la contraparte TS; no acredita paridad ejecutada.
- `documentation/screens.json`, `documentation/overlays/connections.json` y contrato `OLED_DASHBOARD_STATUS_20261102.md`: composición y estado de entrega.

Se reutilizan iconos, fuente y primitivas existentes de AFR2. Sin bytes nuevos de recursos, nuevo ScreenCode, buffers/framebuffers, estado persistente o mensajes wire. La biblioteca generada no se reescribe en esta tarea; conserva los cambios locales que ya tenía. No se ejecuta el generador.

## Estado real

- **atmega:** fuente del dashboard y versión editadas; sin compilar ni cargar.
- **web:** renderer TS y versión editados; sin compilar, ejecutar en navegador, generar recursos ni comparar frames.
- **esp:** solo versión compartida; sin compilación, promoción de paquete o flasheo.
- **CAD / Loader:** sin cambios por esta ficha.

No se ejecutaron tests, compilaciones, validadores, renders, generadores, puertos ni pruebas físicas, por instrucción del usuario. El centrado se especifica por coordenadas; legibilidad física y resultado visual quedan pendientes de observación autorizada. No se declara vigente para este ajuste la paridad histórica de otros layouts.
