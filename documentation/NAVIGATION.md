# Navegación de un único encoder

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
