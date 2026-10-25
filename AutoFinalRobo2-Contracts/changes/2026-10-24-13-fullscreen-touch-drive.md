# AFR2-20261024-13 — Conducción táctil a pantalla completa

Pedido humano: botón junto a Conducción manual, por encima de parámetros/modo;
overlay con auto3D grande centrado, X superior izquierda, título, flechas al
arrastrar/mantener, dirección solicitada y barra inferior de velocidad.

## Implementación Web

Control táctil abre una variante fullscreen del FloatingModal existente: dialog
nativo, foco/confinamiento/restauración, Escape, material desde el disparador y
salida diferida. Añade callbacks de desactivación antes de cerrar/STOP para
cortar input antes de la animación. X izquierda, título centrado y STOP contextual
dentro del dialog, accesible mientras el resto de la página es inert.

Reutiliza GLB final y caché del visor; la cámara encuadra sus dimensiones sin
escalar la malla. Modelo elevado sobre sombra suave, sin bucle de flotación ni
integración de pose. Carga lazy, WebGL demand y DPR máximo1,5. Carga/contexto
no disponible bloquea el gesto; reintentar limpia caché de error/remonta visor.

Pointer Events unifica dedo, lápiz y ratón con capture. Arrastrar22px arma una
de8direcciones; volver a14px libera. Histéresis angular30° evita oscilación
entre sectores. Soltar, cancel/lost capture, salir del área/ventana, multitouch,
blur/hidden/resize, STOP, fallo/modelo no disponible o cierre cancela la entrada.
Nada se reanuda al reconectar/restaurar contexto: exige un nuevo gesto. Teclado
mantiene flechas y combina diagonales; keyup/blur libera. Zoom de página conserva
su configuración; solo la superficie de control toma ambos ejes táctiles.

Parámetro de velocidad compartido con Movimiento, limitado por capacidad actual.
Diagonales reparten la velocidad entre ambos ejes para acotar las ruedas:
`componente=floor(min(velocidad,limite)/(abs(vxFactor)+abs(vyFactor)))`.
El texto Moviendo hacia informa una consigna; la vista no representa pose medida.

## Comunicación y cancelación

Reutiliza control.acquire/control.renew, robot.drive y robot.stop existentes;
sin nuevo comando UART/WS. Abrir no adquiere mando ni mueve. Una adquisición
por gesto y un target mutable: cambios de dirección/velocidad reemplazan target,
sin crear temporizadores/acquisiciones por pixel. Heartbeat180ms, TTL400ms y
single-latest-slot existentes. Soltar envía cero; fallo de envío solicita STOP.
Guardas de generación descartan errores/respuestas obsoletos, cancelan adquisición
tardía sin gesto y evitan que un rechazo viejo termine un gesto nuevo. Se cancela
el hold ante STOP/disponibilidad perdida y se bloquea conducción durante parada
enviándose/no confirmada. Los límites/watchdogs y la autoridad AVR se conservan.
Drive participa en el epoch de movimiento compartido: STOP sigue disponible
hasta una consulta posterior que confirme salidas detenidas, incluso al soltar
con cero pendiente en la ranura del cliente. Un snapshot previo no oculta STOP.

## Estado real

| Módulo | Estado13 |
| --- | --- |
| Web | Fuente nueva y documentación; compilación y paquete en evidencia13 |
| ESP | Sin cambios de firmware/protocolo; recibe activos Web13 por promoción local |
| ATmega | Sin cambios; evidencia compilada12, Hall/memoria0 conservados |
| Contracts | Flujo, guías, cobertura/procedencia y evidencia actualizados |
| CAD | Sin cambios |

Se aplican animate/mobile-native y el borrador visual0.1.9 ya autorizado.
Entrada ocasional: Spatial consistency/State indication con material240ms,
salida200ms, contenido opacity y flechas140ms easeOut. Teclado inmediato y
reduced motion sin desplazamiento. El modelo de control frecuente queda fijo.
Márgenes seguros/dvh, hover por capacidad y gesto local sin scroll de página.
Dirección Confirmada; composición Propuesta; verificación visual No verificada.

Compilación no acredita tacto/arrastre, safe areas, foco, animaciones, recuperación
ni conducción física. No se ejecutaron tests, validadores independientes,
navegador, hardware/serie/reset/flash ni CAD. Denegación previa de navegador
respetada. Firmware conserva0.8.0/EX01v6; no nueva pantalla OLED ni ScreenCode.
[Build13](../documentation/builds/2026-10-24-13/README.md) y
[procedencia](../provenance/fullscreen-touch-drive-20261024.json).
