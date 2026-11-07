# AFR2-20261106-17 · Ventana de prueba KEY/RST y boot ESP
Autoría: Koba Chajchir / AFR2; adaptación mediante Codex.
Pedido: ventana en Flasher que combine KEY/RST y detecte el mensaje boot.

## Adaptaciones por archivo
- AutoFinalRobo2-Loader/src/MainForm.MegaBridge.cs: acceso Probar KEY/RST y boot.
- AutoFinalRobo2-Loader/src/MainForm.KeyRstBootTest.cs: ventana con COM explícito, acción, cancelación, evidencia separada y registro; usa el dueño único del COM de ExecuteCore.
- AutoFinalRobo2-Loader/src/Operations.KeyRstBootTest.cs: diagnóstico sin escritura; boot observado no significa respuesta ROM ida/vuelta.
- AutoFinalRobo2-Loader/src/MainForm.Workflow.cs: etapas exclusivas del diagnóstico.
- AutoFinalRobo2-Loader/tools/esp_bridge_esptool.py: --test-keyrst-boot, captura enmarcada, ASCII/HEX, límites y parseo estricto.
- AutoFinalRobo2-Atmega/service/esp_uart_bridge.c: CMD_BOOT_TEST y recepción temporal por USART1; no modifica firmware de producto ni ESP/Web.
- AutoFinalRobo2-Loader/MANUAL.html: sección keyrst-boot-test.

## Protocolo
CMD_BOOT_TEST=0x14 usa A5 AFR2CTL cmd ~cmd. Bit3 de flags en A5 AFR2RSP anuncia soporte; bits0..2 se conservan.
El cliente consulta primero y no acciona pines si el flasher no tiene esa capacidad.
Después de AUTO_ON confirmado, KEY D32/violeta se mantiene y RST D33/marrón se pulsa.
Solo USART1 cambia a nominal 74880 (16 MHz, U2X1, UBRR1=26; efectivo aproximado 74074). USB conserva el baud configurado.
La captura acotada guarda hasta 256 bytes, suelta ambos pines y restaura USART1 antes de responder.
Tras la respuesta sigue A5 AFR2BOOT (9 bytes), longitud uint16 little-endian y datos. Status3 significa error UART/overflow.
El cliente exige respuesta completa y pines sueltos. Desactiva automático al cerrar sin otro RST: el ESP permanece en el modo observado.
Solo boot mode:(1,n) acredita boot UART, no carga verificada, flash detectada ni comunicación de ida/vuelta. Modo10 no equivale a modo1.

## Validación de fuente y GUI
Loader compilado y ventana abierta/revisada visualmente con COM8.
Parser y nueve escenarios simulados del wrapper PASS, sin puerto real: modo1, flasher anterior, otros modos incluido10, ruido/vacío, errores UART, pines retenidos y captura incompleta. Se comprobaron AUTO_OFF y cierre.

## Compilación y primera carga autorizadas
El usuario autorizó compilar/cargar el flasher en COM8 y repetir el test.
Primera imagen: programa 1890 bytes, RAM estática 1070 bytes. Esto no acredita uso máximo de pila.
SHA256 HEX: 77a03a984ba349e49f00dc59fd2b3e80032eec66acbea92c1a34ec7932f89e74.
Advertencia GCC -Wsign-compare en comparación del complemento de ctl_cmd; build sin errores.
Instalación: job 3a2f577b832a42f590254df59cb45da1, completed a las 19:11:19.
Firma 0x1e9801; avrdude acredita 1890 bytes escritos y verificados.
Registro 20261106-190954-315-a1b7d5.txt.
Backup doble contrastado: mega-20261106-190959-17ede9d5/backup.json; región aplicación 253952 bytes, sin EEPROM.
SHA256 aplicación del backup: 1c0785c3edb0a63fe6eb10fbcfdd0dc03e275d642fc3e371b9ef78904529e095.
Los respaldos previos se conservan. No se afirma que el respaldo nuevo sea firmware de producto: copia lo que estaba en el Mega.
Consulta posterior job 9de394a5f6cd4e1087c6e5b2fb34618a, completed: auto=no, key=suelto, rst=suelto.
Registro 20261106-191141-460-58cd9d.txt.

## Primera prueba y corrección
Prueba job 91bde4d9b0d440b3847b07627c5f2ad4, registro 20261106-191220-444-815121.txt.
Failed a las 19:12:29 por errores UART/captura. AUTO_ON y AUTO_OFF confirmados; pines sueltos. Sin escritura y sin boot acreditado.
Se encontró intervalo de sondeo de 1 ms, excesivo frente al buffer USART de dos bytes a 74880.
Corrección: sondeo ~20 us durante 60000 vueltas, ventana nominal 1,2 s. El wrapper muestra datos ASCII/HEX aun con status3, pero no acredita boot con errores.
Revisión corregida: programa 1886 bytes, RAM estática 1070 bytes.
HEX AutoFinalRobo2-Atmega/build/afr2-esp-bridge-115200.hex.
SHA256: b2775de6c7dc66d8eb0444c528978a2a496a6216403dcae0a0b3510612afe6fc.
Nueve escenarios simulados nuevamente PASS.
La imagen de 1890 bytes ya cargada NO incluye esta corrección.

## Estado pendiente
La solicitud ed3989e88e4a4a0f831ee8fca80f47b0 quedó awaiting_approval, sin escritura.
El Loader se cerró y el registro más reciente continúa siendo el fallo de las 19:12:29; no se acredita una segunda carga.
Se volvió a iniciar la GUI para completar la actualización autorizada.
Mantener confirmación física: D18/D19 desconectados para cargar el Mega y reconectados después para diagnosticar el ESP, RESET Mega libre y actuadores sin potencia.
ESP/Web sin modificaciones ni cargas por esta ficha.
Solicitud de segunda carga tras reinicio GUI (sesión 7ccab8c5b50c4eacacbf080883d067c6): rechazada por revisión automática antes de asignar job/ejecutar. Motivo: considera necesaria autorización explícita adicional para una nueva escritura física en COM8. No se reintentó por otra vía. Revisión de 1886 bytes pendiente de carga y prueba física.

El usuario autorizó explícitamente la segunda escritura de la revisión corregida en COM8 y repetir el test. Nueva solicitud 125cf7a3026d4070a2953f188d4ef1b9: awaiting_approval, autoaceptación desactivada, backup activado. HEX previamente comprobado SHA256 b2775de6c7dc66d8eb0444c528978a2a496a6216403dcae0a0b3510612afe6fc.

Resultado del trabajo 125cf7a3026d4070a2953f188d4ef1b9: failed a las 19:55:15 al comprobar disponibilidad de COM8 (The system cannot find the file specified). WritingStarted=false. Registro 20261106-195515-191-b300d0.txt. Enumeración posterior: COM3, COM6, COM7, COM4; COM8 ausente. La revisión corregida sigue sin cargarse; no se eligió otro puerto sin identificar la placa.

Usuario solicita retomar la prueba y reporta conducción colector-emisor al aplicar tensión al circuito; se registra como observación del usuario, no como prueba de boot. COM8 reaparece. Revisión corregida SHA256 b2775de6c7dc66d8eb0444c528978a2a496a6216403dcae0a0b3510612afe6fc confirmada. Solicitud c46a07682bd746368e1fb8639ba5abd7 en COM8: awaiting_approval inicialmente; backup activado, sin repetir trabajos anteriores.

Carga corregida completada: job c46a07682bd746368e1fb8639ba5abd7, ended 2026-11-06 20:22:58 -03:00. Firma 0x1e9801. Avrdude informa 1886 bytes escritos y 1886 bytes verificados. Registro 20261106-202135-804-dc9806.txt. Backup doble contrastado mega-20261106-202142-388152ac/backup.json, aplicación 253952 bytes, SHA256 391ce50c1b2202a250f35b5a00b2a5a7a3879e3bd1c4f8cc03b56b264469c3bb. Históricos conservados. Pendiente observar boot con esta revisión; cargar/verificar Mega no acredita aún funcionamiento del circuito ESP.

## Prueba física completada con la revisión corregida
Trabajo cefb4fa44ac2465f8191ad6443cf2b94, COM8, completed a las 20:25:24 -03:00.
Registro 20261106-202447-278-e4b373.txt y resumen JSON homónimo.
Confirmación física local del cableado antes de ejecutar.
El flasher confirma AUTO_ON, captura status=0 y sequence=confirmed.
Recibidos 51 bytes:
    ets Jan  8 2013,rst cause:2, boot mode:(1,7)
HEX:
0d 0a 20 65 74 73 20 4a 61 6e 20 20 38 20 32 30 31 33 2c 72 73 74 20 63 61 75 73 65 3a 32 2c 20 62 6f 6f 74 20 6d 6f 64 65 3a 28 31 2c 37 29 0d 0a 0d 0a
Parser mode=1, salida Python 0. AUTO_OFF final confirmado: auto=no, key=suelto, rst=suelto.
WritingStarted=false durante el diagnóstico.
Evidencia real de secuencia automática KEY/RST e ingreso UART de programación en esta configuración y ocasión.
No acredita consulta ROM ida/vuelta, tamaño de flash, firmware ESP cargado, verificación de contenido ni prueba repetida bajo otras condiciones.
El ESP quedó en el modo observado; la prueba no ejecuta un segundo RST para arrancar desde flash.
Estado actualizado por módulo: servicio ATmega compilado, cargado y bytes verificados; captura boot física PASS; Loader compilado/ejecutado y protocolo probado; ESP/Web sin escritura ni cambios por esta prueba.
