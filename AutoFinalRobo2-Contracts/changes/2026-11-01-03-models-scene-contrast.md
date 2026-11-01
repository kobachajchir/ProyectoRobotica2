# AFR2-20261101-03 — Entorno y fichas de Modelos 3D

Pedido: usar en Modelos 3D los colores de fondo/piso de Simulaciones y reformar
las fichas ilegibles en claro y oscuro. Aplicación de identidad0.1.9 autorizada
para este alcance por el pedido; VIS-048. No redefinir la marca.

## Causa en fuente y cambios

El viewport de inspección usaba `--oled-bezel`, sin fondo Three explícito ni
piso, y una grilla con colores fijos. La ficha del auto dependía de vidrio;
las del brazo/cubo usaban `.afr2-model-info` sin fondo/borde/sombra propios y
una cabecera con clase distinta, sin reglas. El texto podía quedar sobre el
canvas oscuro o sobre el modelo. Es diagnóstico estático, sin estilos computados.

- `InspectionEnvironment.tsx`: fondo, piso y luces con la misma dirección
  cromática e iluminación de `CollectorScene`. Se aplica a auto, brazo y cubo.
  Paleta reconsultada después del cambio de tema; render bajo demanda conservado.
- Los tres tokens cielo/suelo/grilla pasan de `collector.css` a `tokens.css`,
  conservando exactamente sus valores de ambos temas. Ahora están disponibles
  aunque se abra Modelos 3D antes que Simulaciones. Sin importar el CSS completo
  de otra página ni duplicar su paleta.
- Cielo claro `#d6eaf5`, piso `#eee3cc`, grilla `#c1b393`; oscuro `#7896ac`,
  `#b8a98d`, `#897b60`. Extensión Propuesta: tinta carmesí `--sim-scene-ink`
  `#671326` para referencias sobre el canvas claro de ambos temas. Selección
  de marcadores del brazo/cubo también se distingue por tamaño.
- Fichas sólidas con `--surface-solid`, encabezado `--surface-subtle`, textos
  `--text`/`--text-strong`/`--text-muted`, borde y sombra de la identidad.
  Auto/brazo/cubo comparten estructura de cabecera, cerrar, cuerpo y acción.
  Se retira la clase glass del auto para que el fondo no altere la lectura.
- Descripciones y datos secundarios de 14px, títulos jerarquizados, saltos de
  línea, cuerpo con scroll y ficha inferior móvil existentes conservados.
  Iconos, marcadores, selección, foco, hover y active con colores semánticos.
  El botón del cubo lleva al simulador Conjunto (`tipo=conjunta`).

## Cobertura de fuente y estado real

| Región | Claro / oscuro | Verificación |
| --- | --- | --- |
| Cielo, piso y grilla auto/brazo/cubo | Tokens compartidos, sin colores OLED | Lectura de fuente |
| Fichas: títulos, descripción, datos y ayuda | Fondo sólido y pares semánticos | Lectura de fuente |
| Cerrar, acción y selectores | Base, foco, hover y active definidos | Lectura de fuente |
| Referencias proyectadas y esferas | Tinta de escena y selección identificable | Lectura de fuente |
| Carga, pausa y error | Superficies/textos semánticos existentes conservados | Lectura de fuente |

Perfil efectivo: manual0.1.9 draft ya autorizado y tokens CSS del proyecto.
No se creó ni modificó un perfil de marca portable. Las fuentes canónicas son
CollectorScene/collector.css, elegidas por el usuario; el manual y tokens son
supporting. Nueva ficha sólida, tinta y composición son Propuestas; su ejecución
y ratios de contraste reales son **No verificados**. No se inventan medidas.

ATmega/ESP/Loader/CAD: sin cambios. Web: fuente actualizada; modelos GLB,
materiales PBR, escalas, anclajes y control del robot conservados. Sin tests,
compilaciones, validadores, generadores, servidor, navegador, render visual,
promoción ESP ni cargas, conforme a AGENTS.md. Pendiente revisar ambos temas
y tamaños en ejecución autorizada. No se certifica contraste WCAG por lectura.

[Cobertura visual](../docs/design-system/coverage-20261101-03.json) ·
[Procedencia por archivo](../provenance/models-scene-contrast-20261101-03.json).
