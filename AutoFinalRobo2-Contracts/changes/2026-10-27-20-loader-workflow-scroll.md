# AFR2-20261027-20 — Loader 0.4: estado, scroll, destino y guía K/RST

27/10/2026. Pedido humano: mostrar la actividad en una barra inferior y con
casillas junto al registro, corregir el desplazamiento forzado, elegir qué
compilar y dejar claro qué se flashea, mover la detección junto a Destino y
separar cableado de los pasos de botones del shield.

## Antes y después en fuente

- El temporizador de Operación hacía ScrollToCaret cada 100 ms, incluso sin
  mensajes. Ahora agrega un lote solo con datos nuevos. LogTextBox conserva
  posición vertical/horizontal y selección al recibir datos. Rueda, barras,
  teclas de navegación o selección con mouse desmarcan Seguir; marcarlo de
  nuevo vuelve al final. El monitor serie usa el mismo control. Al expirar
  texto por el límite de memoria se ajusta la posición al tramo conservado.
- Barra inferior global con etapa actual, monitor serie y revisión MCP. La
  lista lateral usa eventos tipados de la operación: cableado confirmado por
  el usuario, reinicio ESP observado, modo UART, conexión, compilaciones
  ATmega/ESP/Web-LittleFS, escritura, verificación y carga verificada. Casillas
  vacías/rellenas/marcadas indican pendiente/en curso/completado; fallo o
  cancelación conservan su descripción, sin declarar éxito.
- Compilar tiene flecha con ATmega, ESP, Web/LittleFS y ESP + Web/LittleFS.
  Elegir una opción cambia el destino y compila. El cuerpo compila la selección
  actual. Flashear nombra ese destino. Compilar todo conserva la selección de
  carga; las rutas de artefactos externos siguen configurables y se revisan
  antes de escribir.
- Detectar queda junto a Destino. Para ATmega consulta la firma mediante
  avrdude, `-n` y sin `-U`, sin exigir HEX ni escribir. Distingue auto-reset
  solicitado de reinicio observado. ESP conserva consulta ROM/flash.
- Cableado ESP: RESET ATmega a GND, D0 a RX central (E_RX), D1 a TX central
  (E_TX) del **selector ESP URAT**. Se aclara que son los cables antes usados
  en D18/D19; se quita la casilla redundante de retirarlos otra vez. La cabecera
  directa **ESP UART** sigue identificada por separado como lógica de 3,3 V.
  Esto cambia la redacción del asistente, no el contrato eléctrico.
- Tras confirmar cableado, con guía activada se abre otra pantalla. Continuar
  abre primero la escucha. Solo después de abrir el COM se anuncia PUERTO LISTO:
  mantener K, apretar RST, soltar RST manteniendo K y finalmente soltar K. Si se
  observa modo 3 se pide repetir mientras sigue escuchando. Cancelar durante
  la escucha espera su cierre y liberación antes de salir. Luego se mantiene
  la consulta bidireccional ROM, la revisión de archivos/offsets y la confirmación
  de escritura. Desactivar la guía para un ESP ya preparado conserva la consulta.
- Pedido complementario en la misma sesión: al terminar una carga verificada
  ATmega/ESP/LittleFS se abre un aviso para recordar el cableado operativo:
  **D18/TX1 Mega → E_RX / RX central** y **E_TX / TX central → D19/RX1 Mega**.
  Incluye desconectar alimentación, retirar servicio D0/D1 y RESET Mega–GND,
  y dejar K libre. También queda en el log. No aparece tras build/diagnóstico,
  error, cancelación ni carga no verificada.

## Integración y procedencia

Implementación propia sobre el Loader de Koba Chajchir; autoría conservada.
[Adaptaciones por archivo](../provenance/loader-workflow-20261027.json).
[README Loader](../../AutoFinalRobo2-Loader/README.md),
[manual](../../AutoFinalRobo2-Loader/MANUAL.html#resultado).

GUI y solicitudes MCP aprobadas comparten el asistente y los eventos. No se
modifican herramientas/argumentos MCP públicos ni wire ROBO2/EX01/ScreenCodes.
Detectar ATmega se incorpora a la GUI; no se agrega una acción MCP pública.
UART sigue sin acreditar escritura; la marca final exige verificación de
contenido. La observación de reinicio ESP se muestra solo si llegó su mensaje:
observar modo 1 no inventa una observación de reset faltante.

## Estado real de esta ficha

| Módulo | Estado |
| --- | --- |
| Loader | Fuente 0.4.0 y documentación; sin compilar, ejecutar ni probar la interfaz. |
| Contracts | Ficha, procedencia y enlaces actualizados. |
| atmega | Sin cambios de firmware, compilación ni carga. |
| esp | Sin cambios de firmware, compilación, identificación de flash ni carga. |
| web | Sin cambios de fuente, build ni promoción. |
| CAD | Sin cambios. |

Se realizaron lecturas y edición de fuentes/documentación. No se ejecutaron
tests, compilaciones, validadores, aplicación, MCP, apertura de COM, flasheos ni
pruebas físicas. La corrección está implementada en fuente y queda pendiente
su comprobación ejecutada con autorización expresa. No se declara hardware
identificado o probado, ni tolerancia eléctrica por la serigrafía del conector.
