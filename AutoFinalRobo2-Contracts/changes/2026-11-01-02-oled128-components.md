# AFR2-20261101-02 - Componentes OLED128-2

01/11/2026. Continuación del pedido humano de skill: iconografía consistente,
interfaz limpia con contenido necesario, páginas por encoder, alineación de
iconos, barras comunes y canales de sensor con número arriba y valor abajo.
Corrección humana posterior: cinco u ocho sensores simultáneos caben, ajustar
ancho de barras y consultar AutoMicro; no limitar a tres canales por página.

## Cambios de diseño

Skill personal y copia Contracts actualizadas a perfil OLED128-2. Agrega
`references/components.md` y `assets/components.json`: iconos semánticos,
IconLabel, IconPair, Header, EncoderHint, NavigationFooter, MenuRow, ScrollMenu,
PageDeck, Metric, ProgressBar, SensorBar/Group y Notice.

Gap inicial4px entre elementos asociados; centro vertical compartido en grupos,
con redondeo entero uniforme. Recursos por ID: servo/sistema/encoder siempre
el mismo recurso. Icono de sistema incorpora ID `system.core`, con alias
documentado de settings. No cambia los enums de firmware.

El nuevo diseño pide corto=encoder+OK y largo=encoder marcado+flecha de retorno,
sin palabra Volver. Largo vuelve al padre y no al principal; su liberación no
emite corto. Es una revisión explícita del perfil de diseño anterior. El
firmware y simulador actuales aún usan la navegación anterior; no se migran
por esta ficha. Editor/teclado descartan borrador al retornar, sin guardar.

Una sola ProgressBar124×8 para avance determinado o carga indeterminada; cambia
el contenido interno, no el marco. Secuencias nuevas usan contador de etapa
fuera de la barra común. La segmentada existente queda como antecedente.

SensorBarGroup diagrama cinco y ocho barras simultáneas dentro de un rectángulo.
ID arriba, barra vertical, valor debajo; orden físico, escala compartida y
centrado por canal. Mini numeración3×5 permite cuatro dígitos por celda de15/16px
con ocho canales; no dividir el gráfico por defecto. Otra página puede ampliar
cifras o cambiar sección, sin ocultar el conjunto por límite arbitrario.

## Referencia consultada

AutoMicro preservado en `AutoFinalRobo2-Atmega/reference/f4/Core/Src/oled_utils.c`:
RenderIRGraphScene, DrawIRGroupFrame, UpdateIRBars y mini glifos. Paneles49/77px,
barras verticales8/7px. También ocho barras en
`AutoFinalRobo2-Web/reference/web/src/screens/sensorScreens.ts`.
Solo lectura; D:/AutoProyecto no existe en esta sesión. No se modifica ningún
AutoMicro. Valores inferiores son una ampliación propuesta, no un atributo
afirmado de esa pantalla de referencia.

## Artefacto y estado real

Se generó `assets/component-atlas.png` mediante
`scripts/draw_component_atlas.py`, con10 diagramas128×64 y datos ficticios:
iconos, pares de entrada, menú, dashboard, progreso/carga, cinco/ocho sensores,
validez y aviso. Generación documental ejecutada, no prueba ni validación.
No hubo inspección/render en navegador ni validadores, tests o compilaciones.
No se afirma legibilidad física o paridad con una OLED real.

- atmega/esp/web: sin cambios de código, dibujos activos ni artefactos por esta ficha.
- shared/Contracts: skill y documentos; réplica personal sincronizada.
- Loader/CAD: sin cambios.
- Sin regeneración de biblioteca activa, promociónESP, puertos o flasheos.
- Sin commit ni push. Los cambios concurrentes previos se conservan.

[Componentes](../.agents/skills/oled128-ui-design/references/components.md) ·
[Atlas](../.agents/skills/oled128-ui-design/assets/component-atlas.png).
