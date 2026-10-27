# AFR2-20261027-19 — Loader 0.3: monitor serie y diagnóstico de fallos

27/10/2026. Pedido humano: lectura serie con apertura/cierre, cambio de baudios,
visualización del texto, avisos de programación/reinicio/carga y detalle de
fallos. Se implementa en el módulo raíz AutoFinalRobo2-Loader.

## Comportamiento implementado en fuente

- Monitor COM de solo lectura en segundo plano, apertura/cierre manuales,
  baudios de lista o escritos (300–2000000, según adaptador), 8N1, DTR/RTS
  inactivos y codificación UTF-8/ASCII/Latin-1. No apertura al iniciar ni envío.
- Aplicar baudios cierra la captura y abre otra al valor elegido. No cambia
  la UART remota ni el perfil de velocidades de esptool/avrdude. Las selecciones
  del monitor son de sesión. Sin reconexión automática ni cambio de velocidad
  deducido del texto recibido.
- Consola RX conserva fragmentos sin inyectar líneas; seguir texto, pausa de
  vista, limpiar y copiar. Colas/pantalla acotadas, aviso si se omiten caracteres
  de pantalla. Capturas en LOCALAPPDATA/logs: .rx.txt, bytes .rx.bin, eventos y
  resumen JSON al terminar. Si falla el registro se informa y termina lectura.
- BootObserver incremental compartido por monitor y escucha de programación:
  rst cause, boot mode 1/3/otros, con reinicio de estado al cambiar captura.
  La recepción no acredita ida/vuelta ni carga correcta ni modo permanente.
- Cierre esperado del monitor antes de diagnóstico/carga, por GUI y MCP. Si
  no se confirma liberación, se detiene antes de invocar la herramienta. El
  monitor permanece cerrado al terminar; un build puede mantenerlo abierto.
- Eventos y errores, icono/estado visible, fallo resaltado y pestaña abierta.
  Etapas tipadas: espera, modo observado, sincronización, escritura, verificación
  y verificado. Auto-reset AVR se describe como solicitado, no observado UART.
- Errores de comando incluyen ejecutable, exit code y hasta 24000 caracteres
  de salida final, además del log general. Informes failure-*.txt y acciones
  Ver/Copiar último error, con sugerencias explícitamente diagnósticas.
- `contentVerified` en resumen de carga. Ningún OK/done recibido por UART
  acredita escritura: se exige el resultado verificado del flujo existente.
- MCP `loader_serial` consulta última captura y registro incremental por
  captureId/cursor sin abrir/cerrar COM. loader_status incluye captura y último
  informe de error. No cambia el mecanismo de registro de Codex ni lo ejecuta.

[README](../../AutoFinalRobo2-Loader/README.md),
[manual](../../AutoFinalRobo2-Loader/MANUAL.html#monitor),
[guía MCP](../../AutoFinalRobo2-Loader/MCP.md).

## Estados y límites

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente 0.3.0 y documentación. No compilado/ejecutado/validado. |
| Contracts | Ficha compartida y enlaces. |
| atmega | Sin cambios de firmware ni compilación/carga de esta ficha. |
| esp | Sin cambios de firmware ni compilación/carga de esta ficha. |
| web | Sin cambios ni build/promoción de esta ficha. |
| CAD | Sin cambios de esta ficha. |

No se ejecutaron tests, validaciones, compilaciones, interfaz, MCP ni apertura
de puertos/flasheos. El pedido autoriza implementar la funcionalidad, no una
prueba física o carga en esta sesión. No se infiere funcionamiento por fuente.
La publicación en Git no acredita funcionamiento. El monitor no mide cables/tensiones ni
puede observar mensajes UART mientras la herramienta externa es dueña del COM.
Las capturas no tienen rotación automática; dependen del espacio/permisos del
disco. El original binario conserva bytes recibidos, no recupera bytes perdidos
antes de abrir ni corregidos por elegir baudios equivocados.
