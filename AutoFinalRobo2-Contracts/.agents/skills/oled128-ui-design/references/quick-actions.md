# Acciones rápidas OLED128-4

Pedido del usuario: cards inspiradas en la imagen de lista seleccionable
aportada, conservar título y colocar arriba un contador de card en lugar de
puntos. Imagen guardada en `../assets/inspiration/quick-actions-list.png`;
autoría original no identificada. Se hereda composición, no sus funciones
Attendance/Students ni la hora como dato obligatorio del robot.

## Componentes

**QuickActionDeck** agrupa cards de tareas relacionadas, con IDs estables.
**QuickActionCard** tiene título y sección, marco y lista de acciones.
**QuickActionRow** dibuja icono, etiqueta y estado; el foco es una franja
invertida a todo el ancho interior. **CardCounter** muestra `1/3`, `2/3`, etc.
Estos componentes están diagramados y parametrizados en `components.json`;
el generador los usa para producir `quick-actions-proposal.png`.

## Distribución128×64

- Cabecera0..13: icono de sección y título a izquierda; contador a derecha.
  El contador queda **arriba y fuera del marco de la card**. Mantener título
  visible al recorrer las acciones del grupo. No puntos ni dos indicadores
  equivalentes a la vez. El número cuenta cards, no acciones ni filas visibles.
- Card en(0,16),128×34, marco1px y esquinas recortadas comunes2px.
- Dos filas16px, con ventana desplazable si hay más acciones. El ejemplo de
  mayor resolución tiene cuatro filas; no reducir todo para copiar su densidad.
- Icono12px desde x6, etiqueta7×10 desde x22: gap4px y mismo centro vertical.
  Etiquetas y bitmaps en negativo dentro del foco para conservar contraste.
- Franja enfocada122×14 desde x3, separada del contorno exterior. Esquinas
  recortadas de familia. No agregar también `>` al foco: la franja ya lo indica.
- Pie50..63: encoder+OK a izquierda, encoder mantenido+flecha a derecha.
  Se conserva padding y alineación del componente común.

El título se ajusta al área antes del contador. Reservar ancho según el total:
`1/3` no necesita lo mismo que `12/24`. Preferir títulos cortos y no desplazar
la cabecera debajo del contador. El marco exterior agrupa; no es foco.

## Navegación sin modos ocultos

Giro mueve el foco entre acciones de la card. Desde la última, otro paso abre
la card siguiente y enfoca su primera acción; desde la primera, giro inverso
abre la anterior y enfoca su última acción. El contador cambia con la card.
Al final del deck usar la política circular común del proyecto, sin disparar
acciones por cambiar de card. La entrada al deck restaura card/opción por ID.

Así, el mismo giro no intenta cambiar card y fila de forma independiente:
se recorre una sola secuencia de acciones agrupadas. Esta es la propuesta
operativa inicial del componente; no implica un cambio del encoder activo.

Pulsación corta confirma la acción enfocada. Mantener vuelve al contexto padre,
sin ejecutar la selección, y soltar después de largo no genera corto. No hacer
que corto signifique entrar al grupo en unas cards y ejecutar en otras.
Si una entrada navega a otra pantalla, declararla como destino en la ficha.

Seleccionar una acción no la activa. Al confirmar, consultar el servicio y
mostrar aceptación, espera, fallo o requisito real. Acciones no disponibles
mantienen su lugar con `!`; confirmar explica qué falta. Listas dinámicas se
resuelven por ID de acción para no ejecutar otra por cambios de índice.

## Consistencia con los demás componentes

La franja invertida se aplica a todas las filas de **acciones rápidas**;
no alternar fila invertida, marco y puntos según la card. Los menús históricos
con marco siguen siendo antecedentes del firmware; no afirmar su migración.
Las tarjetas de métricas no tienen selección: su contraste representa jerarquía,
mientras que esta lista de acciones usa explícitamente contraste como foco.

Se conservan iconos semánticos registrados: motor, servo y Wi-Fi no cambian
al pasar de dashboard a acciones. Un icono identifica el subsistema y la
etiqueta expresa el verbo. No copiar los rótulos de la captura al producto.

El atlas muestra Motor, Servo y Wi-Fi con dos acciones cada uno. Son ejemplos
de diseño; no se ejecutan comandos al generarlo. No se añade reloj, alumnado,
temporizador o dato del dispositivo que no tenga una fuente real.

Estado: diseño de skill y artefacto generado, sin integrar en firmware o Web,
sin tests, compilación o validaciones ejecutadas.
