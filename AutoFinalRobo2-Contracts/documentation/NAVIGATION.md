## Vigente AFR2-20261025-14 — Vistas hermanas

Por pedido humano actual, Radar/Numérica e IR Gráfica/Valores alternan con giro en ambos sentidos sin apilar historial; corto vuelve al listado restaurando selección/scroll. Ángulo del servo: giro controla, corto vuelve. Es una excepción contextual explícita al requisito anterior de Volver por ítem. Largo800ms abre principal y nunca genera corto al liberar. Los demás menús conservan Volver por ítem. Cada hermana tiene ScreenCode distinto y navegación revisionada; la captura anterior queda inválida.

UI_ACTION permite navegar en estas vistas y volver, pero no mover servo por giro remoto; ese control usa robot.servo.set. Fuente C y biblioteca Web se entregan juntas, incluyendo recursos y comportamiento. [Contrato8](../contracts/SENSORS_RADAR_8.md). Sin ensayo de navegación/paridad.

# Navegación de un único encoder

AFR2-20261024-08: todos los submenús conservan Volver, incluidos IP/AP y
Sistema → Reinicio. Nuevas acciones solo por clic físico; ui.action no las
invoca ni abre la vista de clave AP. Avisos: corto cierra el aviso, largo
abre principal. Iconos fijos, solo texto seleccionado con pausa800ms/scroll40ms;
el giro reinicia la pausa. Sin delays ni callbacks de bus desde renderer.
Compilado; navegación/scroll físicos no probados.

AFR2-20261024-07: la nueva pantalla automática de secuencias conserva largo SW
para principal, corto confirma y Volver explícito a Control. No captura la
navegación después de que el usuario sale. Compilada, sin ensayo físico.
[Contrato](../contracts/EX01_SYSTEM_SEQUENCE_4.md).

Estado vigente AFR2-20261024-02: firmware y consumidor Web compilados. Largo SW
abre principal también desde teclado, descartando credenciales sin enviarlas.
Esto sustituye la excepción histórica de la ficha19 que aparece al final.
No se ejecutaron ensayos físicos ni de navegador en esta entrega.

Decisión del usuario: no hay botón de menú separado. Las entradas son CLK, DT y SW del encoder.

| Entrada | Evento semántico | Resultado |
| --- | --- | --- |
| Giro | rotation +/- | Cambia selección o valor en contexto de edición |
| SW corto | click al soltar | Activa opción/confirma; no significa atrás |
| SW largo | menu una vez mientras está pulsado | Abre menú principal desde cualquier pantalla |
| Soltar después de largo | ninguno | Nunca confirma una opción accidentalmente |
| Ítem Volver + corto | acción back del ítem | Retorna al padre definido |

Atrás solo por opción de submenú. No mapear largo, doble clic, giro de extremo ni botón remoto genérico al retorno automático. Un control Web de menú puede representar la misma acción semántica sin simular duración eléctrica, una vez exista contrato remoto.

Defaults fuente: muestreo 1 ms, debounce20 ms, largo800 ms, cuatro transiciones por detent; son parametrización inicial, no resultado medido. La dirección y dos/cuatro transiciones dependen del encoder real. Si SW arranca pulsado se espera la liberación antes de armar gestos. Transiciones cuadratura inválidas reinician acumulación.

En código inicial Afr2Ui_Apply da prioridad a menu, resetea selección del principal y no emite click a la vez. Principal: Sensores/Estado OLED/Volver a inicio. Sensores y Estado OLED: único ítem Volver al principal. No hay edición implementada todavía.

Para nuevas ediciones: mantener borrador separado del estado confirmado; largo abre menú sin guardar ni ejecutar acciones. Definir si se conserva el borrador y cómo se descarta explícitamente. Volver no debe mover el robot ni reanudar una misión por efecto lateral. STOP/control de seguridad son independientes de navegación.

## Árbol objetivo

El [plan de menús](MENU_PLAN.md) extiende la navegación a Estado, Control, Modos/GOTO/línea, Pruebas, Sensores, Calibración, Wi-Fi, Pantalla y Sistema. Es diseño pendiente; no amplía las cuatro pantallas del prototipo por sí solo. Conservar Volver como única navegación al padre y largo SW como menú.


## Integración AVR AFR2-20261023-15

Polling físico desde main integrado, sin DMA ni botón menú. Debounce20ms y
largo800ms desde flanco crudo que supera debounce; liberación al umbral largo
emite menú y suprime corto. Mantiene Volver por ítem y supresión al arrancar
pulsado. 2/4transiciones y sentido configurables en composición; valores reales
y cadencia no probados. Wire de acciones remotas continúa pendiente.

## Excepción de teclado y captura remota —19

Pedido expreso del usuario: en teclado SSID/password, SW largo cancela el borrador
y vuelve al contexto anterior; OK termina el campo. Fuera del teclado, largo abre
principal, descarta edición numérica y nunca confirma. Volver restaura el historial
de selección/scroll (hasta8 niveles). No hay botón menú añadido.

Entrada física cancela captura remota. Navegación y avisos asíncronos invalidan la
referencia de acción anterior. UI_ACTION no introduce editores ni privilegios;
Conectar un borrador Wi-Fi requiere clic físico. [Permisos/validación](../contracts/ROBO2_PRODUCT_EXTENSION.md).
