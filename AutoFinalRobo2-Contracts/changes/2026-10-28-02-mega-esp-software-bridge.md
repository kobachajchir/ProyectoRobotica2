# AFR2-20261028-02 — Puente de servicio Mega USB ↔ USART1 ↔ ESP

28/10/2026. El usuario propone adaptación resistiva y passthrough por software
para programar el ESP con el hardware del auto. Aclara que 9600 no debe fijarse
como velocidad de carga y plantea la influencia de timeouts.

Se investiga documentación primaria Arduino, OSOYOO, Espressif y código esptool
4.5.1 instalado/oficial. [Conclusiones y procedimiento](../hardware/MEGA_ESP_PUENTE_SOFTWARE.md).
115200 queda candidato, 57600/38400 alternativas y 9600 diagnóstico; sin prueba
de robustez. Se distingue timeout por etapa del límite global del Loader y se
advierte que el puente fijo no sigue un cambio de baud PC/ESP.

Se prepara una imagen de servicio aislada en C/avr-libc, no Arduino Core:
USART0↔USART1 hardware, colas separadas, sin impresiones ni protocolo de producto,
watchdog/timers deshabilitados, salidas de motor solicitadas a cero. No usar
D13 como LED: es el servo de radar. Potencia de actuadores desconectada. Errores
UART contados sin texto inyectado y desborde de cola detiene el transporte.

Make agrega exclusivamente `esp-bridge-firmware BRIDGE_BAUD=...`; artefactos con
baud en su nombre. No altera el objetivo predeterminado ni el HEX del producto,
no carga implícitamente, no modifica bootloader/fuses/EEPROM. Cargar el servicio
sí reemplaza temporalmente el firmware ejecutado del Mega: después restaurarlo.

Conexión propuesta: RESET Mega libre; D18→E_RX y E_TX→D19 por ruta adaptada,
sin D0/D1 al ESP ni jumpers paralelos. Se analiza divisor 1k/2k=3,33V como ejemplo
de señal, no alimentación ni componente que deba agregarse al shield sin conocer
su trazado. La foto UNO/ESP01 no prueba el circuito del ejemplar ESP12S. No se
atribuyen valores de resistencias o tensiones físicas por inferencia de imagen.

| Módulo | Estado |
| --- | --- |
| atmega | Fuente de servicio y objetivo Make nuevos; producto sin cambio funcional. Sin compilar/cargar/probar. |
| esp | Sin cambios ni nueva consulta/carga. Consulta externa25 se cita como evidencia anterior del usuario. |
| web | Sin cambios/build/promoción de esta ficha. |
| Loader | Sin cambios de código/perfil; integración del transporte de software pendiente. Aviso documental de incompatibilidad con RESET→GND. |
| Contracts | Investigación, plan de conexión, límites y ficha compartida. |
| CAD | Sin cambios. |

No se ejecutaron compilaciones, tests, validaciones, lecturas COM ni flasheos.
No se hace commit/publicación por esta ficha. No se garantiza que el servicio
resuelva la causa eléctrica o de reset de la conexión anterior: aceptación
física y posterior integración al cargador permanecen pendientes.
