# AFR2-20261117-13 - Matemática de los simuladores en un panel flotante arrastrable

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.1 (subversión mínima).

Estados:
- web = en fuente;
- atmega = solo versión;
- esp = solo versión.

Sin cambio de contrato.

## Pedido

La sección de matemáticas de los simuladores (auto, brazo y conjunto) pasa a ser un panel flotante y arrastrable con:
- el selector de la transformación de referencia y la matriz con valores en tiempo real;
- un botón de cierre que la oculta;
- en el simulador, el botón que alterna «Mostrar matemáticas» / «Ocultar matemáticas».

Así se ve en tiempo real sin quedar escondida abajo. Solo se puede mover dentro del área de contenido de la pantalla.

## Cambios (Web)

- `src/features/collector/FloatingMathPanel.tsx` (nuevo). Panel con `position: fixed`, en un portal a `body` (no se pierde al hacer scroll).
  - **Límites:** el área visible de `#afr2-main`, debajo de la barra superior y encima de la barra inferior del celular. Nunca pasa sobre la barra lateral ni la superior.
  - **Arrastre:** desde el encabezado con el puntero (captura de puntero, en táctil también) o con las flechas con el encabezado enfocado (16 px; 48 px con Shift).
  - Se vuelve a acomodar dentro del área al cambiar la ventana, al hacer scroll o al cambiar su propio tamaño.
  - **Posición:** la recuerda en este navegador (`afr2.sim.mathPanel.position.v1`). La primera vez abre arriba a la derecha.
  - **Encabezado:** icono de mover, título «Matemática en vivo» / «Transformaciones del modelo» y botón ✕ «Ocultar matemáticas», que toma el color de acción en hover.
  - **Cuerpo:** con scroll propio (alto máximo 78vh).
- `src/features/collector/SimulationMatrix.tsx`: deja de ser un `CollapsiblePanel` y es solo el contenido: selector, `HomogeneousMatrix` en vivo, ángulo/seno/coseno y expresión simbólica. Corrige tildes: «Articulación», «simbólica».
- `src/features/collector/CollectorPage.tsx`:
  - el botón del encabezado del simulador dice «Mostrar matemáticas» / «Ocultar matemáticas», con `aria-expanded` y `aria-pressed`;
  - se quita el `aside` inferior;
  - al cerrar, el foco vuelve al botón.
- `src/styles/collector.css`: estilos del panel flotante (sombra, estado arrastrando, entrada animada, movimiento reducido).

## Estado de validación

Revisado en Vite (5175), en Modelos y simulación → Brazo robótico:
- abre debajo de la barra superior con el selector y la matriz 4×4;
- el botón cambia a «Ocultar matemáticas»;
- arrastrado hasta el borde, frena en el límite del área de contenido y guarda la posición;
- la ✕ lo cierra, el botón vuelve a «Mostrar matemáticas» y recupera el foco;
- sin errores en consola.

Sin compilar: `npm run dev`/`build` siguen bloqueados por el control de huellas del OLED por cambios en curso de otra sesión.

## Pendiente

- Compilar la Web.
- Revisar en un celular real el arrastre táctil.
