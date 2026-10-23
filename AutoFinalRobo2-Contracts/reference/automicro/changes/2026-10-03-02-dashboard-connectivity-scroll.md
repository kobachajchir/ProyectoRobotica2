# AMI-20261003-02 — Scroll de SSID e IP del dashboard F4

- Fecha: 2026-10-03 (America/Buenos_Aires)
- Origen: FirmwareF4, checkout local modificado; código sin commit independiente.
- Estado del origen: implementado en fuente.
- Área: comportamiento visual de pantalla 0x010101.

## Antes y después

Antes: SSID limitado a 9 caracteres STA/8 AP; IP hasta 15 caracteres, pudiendo superponerse con USB/RF. Después: dos ventanas recortadas independientes y scroll automático de cadenas completas. La selección STA antes que AP permanece.

Font_7x10; nombre y=0..9, IP y=10..19. Inicio STA x=1, AP x=21; final exclusivo fijo x=64. Toda la mitad derecha x=64..127 queda reservada para indicadores, aun cuando estén ocultos; nombre e IP nunca invaden ese espacio. El icono AP queda fuera de las ventanas. No se borra el separador y=20 ni los iconos.

Si el texto cabe, no se mueve. Si desborda: 800 ms de pausa inicial, 1 pixel/40 ms hasta mostrar el final, 800 ms de pausa final, retorno al inicio y repetición. Fases independientes, reiniciadas cuando cambia texto/geometría o al volver a la pantalla. Redibujados ordinarios conservan fase. Notificaciones y otras pantallas suspenden el scroll. Solo se actualizan páginas OLED 0..2 cuando hay un desplazamiento nuevo y el DMA está libre.

## Flujo y wire

OLED_MainTask llama OledUtils_DashboardScrollTick. El dashboard configura ambas ventanas; se reutiliza Oled_DrawStrScrolled, con estado independiente del menú. La animación es local: ningún request, ACK, evento o timeout de comunicación nuevo.

Screen code 0x010101, snapshot dashboard y payloads existentes sin cambios; no se transmite fase temporal. Sin cambio de autenticación o capacidades. No afirmar paridad de Web solo porque comparte screen code.

## Repositorios y acciones

| Repositorio | Estado | Acción |
| --- | --- | --- |
| FirmwareF4 | origen | Completado en fuente: renderer, tarea OLED, declaración y documentación local. |
| Firmware ESP01 | no_aplica | Transporte y payload existentes permanecen; sin cambios de código. |
| Web ESP01 | pendiente | Builder dashboard conserva truncamiento previo y geometría distinta; adaptar ventanas y representación temporal antes de afirmar paridad visual. |
| AutoMicroQtDesktop | no_aplica | No consume offsets ni controla la animación local; sin cambios de código. |

## Evidencia y límites

Fuentes: Core/Src/oled_utils.c, Core/Src/app_tasks.c, Core/Inc/oled_utils.h; explicación local en Docs/DASHBOARD_CONNECTIVITY_SCROLL_20261003.md. No ejecutados compilación, tests, validaciones automáticas ni ensayo físico en esta entrega. Para desplegar se requiere compilar y cargar F4. No requiere actualizar ESP/Qt por wire.

Pendientes independientes de este cambio: cruces SSID en eventos 0x81/0x92, política de arranque AP/normal, prioridades AP+STA entre pantallas, confirmación final de configuración AP y resto de auditoría. No marcarlos resueltos por esta animación.

## Actualización 2026-10-03 — reserva fija de indicadores

Por pedido posterior del usuario, se reemplazó el ancho dependiente de USB/RF por un límite fijo x=64 para ambas filas. También se limita defensivamente el extremo recibido por DashboardScroll_DrawLane. Se mantiene el inicio AP x=21 para proteger su icono izquierdo. El intervalo de texto STA es [1,64), AP [21,64); y=0..9 y y=10..19. USB x=91..106, RF x=110..126 y los controles inferiores quedan fuera de las ventanas tanto al dibujar como al borrar durante el scroll. Fases y pausa sin cambios. El límite anterior variable queda reemplazado por este comportamiento.

Acción F4 completada en fuente. No ejecutados tests, compilación ni prueba física. Web sigue pendiente de adaptar esta reserva fija y la animación. ESP/Qt sin cambios. El resto de la auditoría AP/WiFi continúa pendiente.
