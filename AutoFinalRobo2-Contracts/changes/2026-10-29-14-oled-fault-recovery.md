> **AFR2-20261029-14 compilado y cargado:** 29/10/2026 11:37:12, COM8, Loader1.1.2,74516bytes escritos y verificados, exit0. Producto/comunicación/biblioteca compilados sin errores/advertencias. SRAM estática6889bytes, margen1303bytes; main259bytes según .su individual, sin certificación de máximo dinámico. [Evidencia](../documentation/builds/2026-10-29-14/README.md). La entrega sin compilar indicada debajo es histórica; sin ensayo físico posterior de UI.

# AFR2-20261029-14 — Recuperación OLED sin apagado y servicio con plazo

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Estado: ATmega implementado en fuente y revisado por lectura; **sin compilar,
tests, validadores, flasheo ni prueba física en esta revisión**. El HEX cargado
en COM8 sigue siendo11. ESP sin cambios; Web solo fingerprint revisado del
transporte C, sin cambios de dibujo ni nueva compilación/promoción.

## Reporte y alcance de la evidencia

El usuario observa negro permanente hasta reiniciar, tiempos variables y a
veces un repintado completo durante navegación local. No usa la Web; no se
dispone de captura que distinga reinicio del micro, bloqueo del programa y
fallo exclusivo del OLED. No se afirma causa raíz demostrada ni reparación
física verificada.

En11 cualquier ERROR/TIMEOUT o fallo inmediato de Submit invalida todas las
páginas, pone initialized=false y espera1000ms. La siguiente inicialización
empieza con0xAE (DISPLAY OFF). Si falla antes de llegar a0xAF puede dejar el
panel apagado hasta conseguir una inicialización completa. Esto explica una
ruta software de apagado/repintado general, pero no prueba que haya ocurrido.
Además main pausa el OLED sin plazo si quedan input o respuesta UART pendientes;
por sí sola esa pausa no envía el comando de apagado.

## Adaptaciones por archivo

- Atmega/src/display/afr2_oled.c: fallo de comandos de posición o datos invalida
  solo el tramo afectado, conserva ACKs del resto y reintenta tras50ms. Fallo
  de ajustes vuelve a marcarlos pendientes. Tres fallos sin progreso útil
  fuerzan reconfiguración y sincronización completa para contemplar pérdida
  de configuración del panel; después de la primera inicialización correcta
  se omite0xAE en recuperaciones y se conserva0xAF. Una inicialización fallida
  de arranque sigue pendiente y puede repetirse con0xAE. Éxitos de comando de
  posición no reinician el contador de fallos: no son progreso de datos.
- Mismo archivo: send publica la fase intentada antes de Submit para clasificar
  fallos inmediatos correctamente. BUSY restaura la fase anterior y no cuenta
  como fallo. Callback sigue limitado a registrar, sin iniciar transferencias.
- Atmega/include/afr2_oled.h: cuatro bytes de estado adicional en AVR: último
  error y fase fallida retenidos, fallos consecutivos y configuración previa.
  Son datos del handle; no se añadió reporte UART0 ni nuevo comando wire.
- Atmega/src/main.c: nueva oportunidad de servicio cada5ms cuando otras tareas
  mantienen la petición de pausa, después de atender servicios/input/render.
  No garantiza una transferencia cada5ms: BUSY, cooldown, recuperación y el
  tiempo de una vuelta de main siguen aplicando. El servicio continúa sin
  esperas activas, hasta16bytes de datos por transferencia. Reloj actualizado
  justo antes del servicio; nuevo timestamp local de4bytes.
- Web/src/features/oled/firmwareRenderer.sources.json: fingerprint del texto
  normalizado LF revisado. Primitivas, glifos, layout, ScreenCodes, snapshots,
  navegación y copia TypeScript permanecen con su comportamiento anterior.
- README/INFORME_ADAPTACION/COMPOSICION del módulo y README de Contracts:
  estado y vínculo a esta ficha. Autorías originales preservadas.

Todas las rutas siguen usando I2C Manager y el backend TWI ya existente, único
cliente OLED. No se cambió bitrate, cableado, EEPROM, configuración de motores,
sensor Hall ni interrupciones del encoder. El diagnóstico separado comparte
este driver, pero no se recompiló ni cargó ese firmware.

## Límites y seguimiento

Los repintados completos siguen siendo necesarios al arrancar, tras recuperar
configuración después de fallos persistentes y cuando cambia realmente toda
la imagen. Error aislado deja de forzarlos. La captura TX y confirmación solo
tras ACK se mantienen; no se transmite el framebuffer transitorio de Clear.

Build11: SRAM estática6885bytes, margen1307bytes, main257bytes de pila según
.su individual. No equivalen al máximo dinámico de toda la cadena más ISR.
Esta revisión agrega4bytes al handle y un reloj local; necesita nuevo mapa y
revisión de pila cuando se autorice compilar. No se diagnostica desbordamiento
de pila solo con ese presupuesto, ni se acredita memoria suficiente.

El próximo paso de comprobación requiere compilar/cargar esta fuente y observar
si existe reinicio real (BT01/boot id y reset cause ya disponibles por USART1)
o errores OLED. No se abrió COM8, no se interrogó el controlador ni se ejecutó
el firmware como parte de esta investigación.
