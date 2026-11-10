# AFR2-20261110-08 — Repositorio del proyecto con QR en Desarrollador

Versión: **1.16.0 (subversión)**.

Pedido: integrar un div con QR al repositorio GitHub, visualmente acorde a la interfaz que se viene desarrollando.

## Fuente implementada

- `DeveloperPage.tsx`: tarjeta debajo del perfil y antes de Protocol Studio, con título, breve descripción, URL visible, enlace «Abrir en GitHub» y QR también enlazado. Destino único: `https://github.com/kobachajchir/ProyectoRobotica2`, sin el punto de puntuación final del mensaje.
- El QR se calcula al montar la página mediante `useMemo` con la biblioteca Nayuki MIT ya existente en `features/oled/vendor/qrcodegen.ts`. Se reutiliza únicamente el generador; no se modifican pantallas, flujo ni firmware OLED.
- Dibujo SVG local, nivel mínimo de corrección MEDIUM y zona libre de cuatro módulos. Negro sobre blanco en ambos temas, sin logo superpuesto, filtros ni tintas de tema en los módulos. Sin servicio remoto, descarga de imagen o dependencia nueva.
- `developer.css`: composición de dos columnas y una columna por debajo de 600 px; QR de 208 px con ancho máximo del contenedor, enlace directo de área táctil mínima heredada, foco visible y URL con salto de línea. La tarjeta usa `afr2-protocol-card`, los tokens de espacio/radio/color y la capa común `visual-language.css`.
- Los dos enlaces abren en pestaña nueva con `noopener noreferrer` y nombre accesible. La tarjeta permanece estática; no se añaden animaciones ni una variante de color de marca.

## Identidad y evidencia

Modo APLICAR de `visual-identity-system`, sobre el manual 0.1.11 draft cuya aplicación de presentación ya estaba autorizada en VIS-062/AFR2-20261102-15. El pedido actual autoriza la nueva tarjeta en Desarrollador. Fuentes: manual/index/components/tokens, material común de `visual-language.css`, tarjeta de Protocol Studio y enlaces existentes.

URL y presencia del QR: Confirmadas por el pedido. Superficie, acentos y tokens compartidos: Observados en código vigente. Distribución nueva, escala y jerarquía de la tarjeta: Propuestas, implementadas en fuente y no verificadas en ejecución. Blanco/negro del QR es una excepción funcional al tema, circunscrita al dibujo escaneable. [Cobertura](../docs/design-system/coverage-20261110-08.json).

## Versionado y estado

Versión común actualizada en `contracts/afr2_build_info.h`, `Web/package.json` y las dos entradas raíz de `package-lock.json`; historial en `docs/VERSIONADO.md`. La función nueva es compatible y no cambia contratos del enlace ni comandos.

Web: fuente implementada. ATmega/ESP: solo versión compartida; no se cambia lógica. CAD/Loader: sin cambios. Se preservan los demás cambios pendientes del workspace.

Sin tests, compilaciones, validadores, navegador, comprobación de escaneo, empaquetado, promoción al ESP, puertos ni flasheos por instrucción del usuario. No se afirma que se haya observado el bloque en ejecución. Sin commit ni push nuevos en esta tarea.
