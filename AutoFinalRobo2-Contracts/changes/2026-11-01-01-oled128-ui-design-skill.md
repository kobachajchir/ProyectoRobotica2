# AFR2-20261101-01 - Skill de diseño OLED128

01/11/2026. Pedido humano: convertir las pantallas OLED128×64 existentes en una
guía reutilizable de interfaz y flujo, con iconografía semántica y patrones
consistentes entre proyectos. Solicita explicación detallada para iterar.

## Entrega

Nueva skill `oled128-ui-design`, con entrada breve, sistema visual, diseño de
flujos y procedencia/adaptadores. Fuente versionable en
`.agents/skills/oled128-ui-design/`; copia personal descubrible en
`C:/Users/kobac/.codex/skills/oled128-ui-design/`.

Referencia los53 nodos/11 avisos del catálogo y renderers actuales, no las
cuatro pantallas históricas iniciales. Distingue requisitos humanos, patrones
observados por lectura y composiciones propuestas. Reutiliza copias de iconos
y fuente con comentarios de autoría; no instala drivers ni crea iconos nuevos.
El nuevo registro semántico evita confundir el mando UI con Hall de rueda.

MENÚ-3 hereda tabla, filas16px, marco, iconos y desplazamiento del texto
seleccionado. ESTADO-3, MEDIDA-1, editor, gráficos, progreso, avisos, teclado y
confirmación quedan documentados con zonas, datos, eventos y límites.
Cabeceras uniformes y disposición de dashboards son propuestas; no afirman
que se hayan migrado los renderers actuales. No se deduce fidelidad por fuente.

Flujo: función → tarea → árbol/transiciones → plantilla/ficha → implementación
solo cuando se solicite. Mantiene snapshot/renderer/servicios separados,
unidades y validez, gestos de encoder y variantes hermanas contextualizadas.

La skill local `afr2-screen-authoring` enlaza la nueva guía conservando sus
reglas de integración y contraparte Web. La personal es independiente de las
rutas del proyecto salvo notas de procedencia/adaptación opcionales.

## Estado por módulo

- atmega/esp/web: sin cambios de fuente, recursos activos o artefactos por esta
  ficha; no se migran textos ni iconos de pantallas o simulador.
- shared/Contracts: skill, recursos de referencia, enlace y registro documental.
- CAD/Loader: sin cambios.

No se ejecutaron tests, compilaciones, validadores de skill, renders, navegador,
puertos ni flasheos, conforme al pedido humano. Sin commit ni push.
Wire, ScreenCodes y paquetes ESP conservados.

## Recursos y origen

Copias literales de `Atmega/src/ui/afr2_icons.c`, `include/afr2_icons.h` y
`src/display/font7x10.c` en `assets/` de la skill. Sus dependencias AVR/AFR2 son
parte de la referencia, no una API portable ya implementada. El archivo
`references/sources-and-adapters.md` enumera fuentes leídas y restricciones.

[Skill](../.agents/skills/oled128-ui-design/SKILL.md) ·
[Guía visual](../.agents/skills/oled128-ui-design/references/design-system.md) ·
[Diseño de flujo](../.agents/skills/oled128-ui-design/references/flow-design.md).
