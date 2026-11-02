---
name: oled128-ui-design
description: Diseña pantallas y flujos consistentes para OLED monocromas 128x64 en electrónica, con iconos y componentes compartidos. Crea dashboards, menús, acciones, gráficos y pantallas QR; convierte contenido elegido en bitmaps QR para OLED. Úsala para planificar o implementar estas interfaces y su simulador, no para diseño web general.
---

# Interfaces OLED 128×64

Crear una familia de interfaces reconocible entre proyectos: cada concepto usa
el mismo icono, cada tipo de pantalla usa la misma plantilla y cada gesto
mantiene su significado. Las funciones del equipo determinan el contenido;
la plantilla determina cómo se presenta y se recorre.

## Alcance y estado

Perfil **OLED128-5, QR de contenido elegido y bitmaps**. La regla del usuario de
consistencia visual e iconografía es un requisito. Las coordenadas de las
plantillas nuevas son una propuesta concreta para iterar; no afirmar que ya
están implementadas en los proyectos existentes. No migrar pantallas, recursos,
firmware ni simuladores como efecto de crear o leer esta skill.

Leer [componentes](references/components.md) como especificación vigente y
[sistema visual](references/design-system.md) para su contexto y antecedentes al diseñar pantallas, y
[funciones a flujo](references/flow-design.md) al diagramar funciones o estados.
Consultar [origen y adaptación](references/sources-and-adapters.md) para reutilizar
los recursos adjuntos o implementar en AutoFinalRobo2.
El [atlas de componentes](assets/component-atlas.png) muestra la propuesta con
datos ficticios. `scripts/draw_component_atlas.py` lo genera desde los tokens,
iconos y fuentes adjuntos; usarlo cuando se solicite producir/actualizar ese
artefacto. Generar el atlas no acredita verificación visual o de hardware.
Para dashboards aplicar también [composición visual](references/dashboard.md),
inspirada en la imagen de monitor aportada por el usuario, y ver la
[propuesta de tres páginas](assets/dashboard-proposal.png).
Para acciones rápidas usar [cards y selección](references/quick-actions.md):
título conservado, contador numérico arriba y fila seleccionada invertida.
Ver [propuesta de cards](assets/quick-actions-proposal.png).
Para mostrar códigos QR leer [pantalla QR y generación](references/qr.md).
`scripts/generate_qr.py` convierte el contenido exacto en PNG, bitmap62×62
por filas y framebuffer128×64 por páginas, usando un encoder MIT adjunto.
Respeta zona clara4módulos, escala entera y límites de capacidad. No inventar
un QR dibujando patrones a mano ni usar generación de imágenes para codificarlo.

## Reglas de familia

- Lienzo monocromo 128×64, coordenadas enteras, fuente base 7×10, sin suavizado.
  Fuente mini3×5 registrada para etiquetas/valores compactos. Menús de lista
  desplazable con filas16px, icono fijo y marco de foco; variante con pie usa
  dos filas, variante de tres filas ocupa el área completa sin pie superpuesto.
- Una tarea por página; mostrar lo necesario. Varias páginas manuales por giro
  permiten repartir secciones. No paginar por defecto los sensores: cinco u
  ocho barras verticales simultáneas caben ajustando anchos y fuente numérica.
- Usar componentes de familia: misma cabecera, pie, selección, barra de carga/
  progreso y celda de sensor. Separación icono-contenido4px como token inicial;
  centrar todas las piezas de un grupo sobre el mismo eje vertical.
- Mantener un registro semántico de iconos. Para referenciar el encoder UI en
  la OLED, dibujar su icono; no volver a escribir «encoder». El texto que lo
  acompaña explica la función o gesto. No sustituir acciones diferentes por el
  mismo icono sin contexto. No reutilizar el icono UI para sensores Hall.
- Pulsación larga usa `input.encoder.long`, con **L interna**, según corrección
  explícita del usuario. No subrayado, barrita debajo ni otra marca inventada.
  La variante está registrada en `assets/encoder-gestures.json`; conservar
  su procedencia: adaptación del bitmap base, original conL aún no localizado.
- Elegir una plantilla del sistema antes de inventar un layout. No alternar
  menús de cuadrícula, carrusel y lista para funciones equivalentes.
- Acciones rápidas: QuickActionCard con lista y franja invertida de foco,
  título arriba y contador `actual/total` en cabecera a derecha. No puntos
  de paginación. Giro selecciona y pasa a otra card al superar el grupo;
  OK confirma la acción enfocada. Todos los grupos usan el mismo patrón.
- Separar foco de selección, valor confirmado, borrador, disponibilidad y
  estado activo. No usar el mismo marcador para significados diferentes.
- Dashboard resume el equipo; menú permite elegir acciones; editor modifica
  un valor; aviso informa un evento. No mezclar roles accidentalmente.
- QR: usar QRScreen, excepción de layout a altura completa con columna lateral.
  Módulos oscuros sobre campo claro y zona de silencio intacta; no dibujar,
  invertir, poner logos ni superponer cabecera/pie dentro del área protegida.
  Perfil inicial2px/módulo en caja62px. Si no cabe, informar y pedir acortar o
  elegir densidad mayor; nunca truncar ni bajar ECC/resolución silenciosamente.
- Dashboard: cabecera de alto contraste, hasta dos bloques con jerarquía clara
  y lectura principal con numerales registrados. Repartir secciones por páginas;
  no copiar la densidad del monitor de referencia a128×64. La carga indeterminada
  omite la cifra; no escribir `Sin porcentaje` ni explicaciones de su ausencia.
- Definir unidad, rango, precisión, validez y antigüedad de cada dato.
  `--` es desconocido; cero es una lectura válida. Nunca completar datos reales
  ausentes con ejemplos. Simulación con fixtures requiere contexto identificado.
- Perfil de diseño nuevo: giro cambia página, opción o borrador según contexto;
  corto confirma, representado por icono del encoder + `OK`; largo vuelve un
  nivel, representado por encoder con L interna + flecha de retorno,
  sin escribir `Volver`. Umbral inicial800ms; soltar tras largo nunca confirma.
  En raíz, corto abre principal y largo no navega; no crear un botón inexistente.
  Este pedido sustituye en el diseño el largo=principal anterior; el firmware
  existente sigue con su contrato hasta una migración explícita y vinculada.
- Leer restricciones actuales del proyecto antes de implementar. No ejecutar
  tests, builds, validadores, hardware ni flasheos sin pedido expreso en los
  proyectos de este usuario. Elaborar el diseño no autoriza esas ejecuciones.

## De una explicación funcional a una interfaz

1. Extraer objetivos y tareas de la explicación del usuario: qué mira, qué
   cambia, qué inicia/detiene, qué necesita calibrar y qué puede fallar.
2. Consultar funciones/servicios y controles reales del proyecto. Preguntar
   solo por decisiones que cambien el flujo; registrar supuestos de diseño.
   No suponer actuadores o sensores disponibles.
3. Producir mapa de tareas, árbol y transiciones por estado, incluyendo entradas
   asíncronas. Elegir plantilla y contrato de datos para cada pantalla.
4. Entregar fichas, bocetos anotados con coordenadas y diccionario de iconos.
   Indicar qué se hereda, qué se propone y qué falta implementar. Un boceto
   textual no acredita rasterización ni legibilidad física.
5. Si se solicita implementar, mantener separados estado/servicios/snapshot/
   renderer y adaptar la biblioteca real. Si existe réplica Web, compartir
   identidad, datos, recursos y layout. Declarar limitaciones del simulador.

## Entrega útil

Presentar primero el comportamiento para el usuario, después el árbol, las
plantillas/fichas y los cambios necesarios. Incluir estados sin dato, ocupado,
bloqueado, error, finalización y cancelación que realmente apliquen.
Cerrar con el estado real: diseño, fuente, recursos disponibles y comprobaciones
ejecutadas o pendientes. No afirmar uniformidad ya conseguida por escribir
una guía. No crear un nuevo patrón cuando un componente de familia lo resuelve.
