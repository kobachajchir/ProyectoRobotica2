# AFR2-20261117-04 — Superficie de imagen al enfocar Ir

Fecha: 17/11/2026. Versión: **1.33.2 (subversión mínima)**.

## Cambio

El hover previo desplazaba la tarjeta y su flecha y usaba un fondo CSS con zoom 1.08 y tiempos distintos. Ir ahora conserva su geometría y contenido: una capa interior recortada al radio contiene un `img` decorativo con `object-fit: cover`, ancho/alto 100% y `object-position: 50% 64%`, elegido para priorizar el vehículo y el recorrido del SVG disponible. Imagen y velo oscuro aparecen juntos sobre el fondo sólido mediante opacidad; el zoom pasa de 1.04 a 1. Los efectos revierten desde su valor actual mediante transiciones CSS de 400 ms con el token existente `--ease-standard`, siguiendo la skill animate y la duración solicitada.

El marco azul/violeta de 2 px se superpone al borde original con un pseudo-elemento independiente y anima solo opacidad, sin modificar padding ni dimensiones. El icono conserva su apariencia; título, descripción y flecha aclaran su color para legibilidad, sin desplazarse. Las reglas previas de hover excluyen Ir; no hay selectores de grupo que alteren las tarjetas vecinas. El comportamiento propio de Controlar y Seguidor de línea se conserva.

El foco visible de teclado muestra la misma superficie y conserva el contorno de foco. El hover de Ir requiere `(hover: hover) and (pointer: fine)` para evitar activación persistente por toque. `prefers-reduced-motion` elimina el zoom y acorta el fade al token `--duration-fast`.

## Imagen y límite de entrega

El recurso existente `/home/quick-goto.svg` es una ilustración, no una fotografía original. El usuario adjuntó una composición de las tres tarjetas con el mapa ya debajo del contenido de Ir. Usar esa composición como fondo repetiría textos, bordes e iconos; no se incorpora ni se inventa una imagen sustituta. La capa queda conectada al SVG actual hasta recibir la imagen limpia del mapa. El encuadre final debe ajustarse al incorporar ese recurso.

## Estado por módulo

- Web: cambios en fuente de HomeQuickLinks, estilos y tokens, más versión de package.json y ambas entradas raíz de package-lock.json.
- ATmega/ESP: solo versión común en contracts/afr2_build_info.h; sin cambios funcionales.
- Wire/CMD, OLED, CAD y Loader: sin cambios por esta ficha.
- Sin tests, compilaciones, validaciones visuales o automáticas, navegador, generadores, promoción ESP ni flasheos, por instrucción del usuario.
- Cambios previos preservados. Sin commit ni publicación por esta tarea.
