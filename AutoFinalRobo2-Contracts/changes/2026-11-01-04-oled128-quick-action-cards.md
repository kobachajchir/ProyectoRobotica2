# AFR2-20261101-04 - Cards de acciones rápidas OLED128-4

01/11/2026. Pedido humano: acciones rápidas con cards inspiradas en imagen de
lista seleccionable; conservar título y arriba contador numérico de card,
sustituyendo indicadores de puntos.

## Entrega concreta

Skill de diseño actualizada aOLED128-4. QuickActionDeck/Card/Row y CardCounter
registrados en `assets/components.json`, documentados en
`references/quick-actions.md` y usados en el generador documental.

Cabecera con icono, título y fracción `actual/total` sobre la card, sin puntos.
Marco128×34 desde y16; dos filas16px con icono12 y gap4px al texto, centrados.
La fila seleccionada usa franja invertida122×14 con esquinas de familia.
Texto/iconos en negativo; no cursor redundante. Pie común encoder+OK y
encoder mantenido+flecha. Las filas adicionales usan ventana desplazable.

Propuesta de navegación: giro recorre acciones y pasa al grupo adyacente
al cruzar el extremo; OK ejecuta solo lo enfocado; largo retorna. La lista
tiene IDs estables. Cambiar card no ejecuta una acción. No hay nuevo modo
oculto para alternar giro de fila/card. El contador identifica grupos,
no la cantidad de acciones o primera fila visible.

Tres muestras: Motor (Adelante/Detener), Servo (Centrar/Desactivar),
Wi-Fi (Actualizar/Buscar red). Especificación y dibujos, no comandos enviados.
La imagen de referencia queda preservada como inspiración, con autoría
original no identificada; no se copian sus funciones ni se agrega reloj.

## Estado real

Se generan lámina de cards y atlas actualizado mediante el generador
documental de la skill. Sin tests, compilaciones, validadores, navegador,
puertos o flasheos. No se acredita comportamiento de un dispositivo por el
artefacto. Se conserva la navegación activa del firmware/simulador anterior.

- shared/Contracts: skill, tokens, documentos y artefactos de diseño.
- atmega/esp/web: sin cambios de código o recursos activos en esta ficha.
- Loader/CAD: sin cambios.
- Skill personal sincronizada; sin commit/push ni promociónESP.

[Guía](../.agents/skills/oled128-ui-design/references/quick-actions.md) ·
[Propuesta](../.agents/skills/oled128-ui-design/assets/quick-actions-proposal.png).
