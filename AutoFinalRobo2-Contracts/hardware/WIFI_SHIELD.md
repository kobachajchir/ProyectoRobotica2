# Shield Wi-Fi OSOYOO UART V1.3 con ESP-12S

> **Identificación aportada AFR2-20261027-25:** el usuario obtuvo ESP8266EX,
> chip `0x00c2218e`, cristal 26 MHz y flash `5e / 4016`, capacidad **4 MiB**,
> mediante USB–TTL en COM5 a 9600 con esptool 4.5.1. [Evidencia y límites](diagnostics/20261027-com5-usbttl-user-report/report.md).
> Sin nueva escritura ni verificación de firmware; modo/frecuencia SPI y layout
> siguen pendientes. No identifica el punto de fallo de la ruta Mega/selector
> ni acredita tolerancia eléctrica. Estados inferiores anteriores conservados.

Actualización documental del 2026-10-23, [AFR2-20261023-03](../changes/2026-10-23-03-wifi-shield-evidence.md). [Maestro](../docs/INFORME_MAESTRO.md), [pinout](PINOUT.md), [carga](FLASHING.md). El usuario identifica este shield mediante la página y tres imágenes de referencia; no son fotografías verificadas del cableado actual del robot. No se ejecutaron ensayos ni flasheos.

## Evidencia y alcance

- [OSOYOO: descripción del shield](https://osoyoo.com/2020/07/31/introduction-of-osoyoo-uart-wifi-shield/): ESP-12S, regulación a 3,3 V y adaptación de niveles integradas declaradas por fabricante; selectores de UART, conector directo ESP, KEY y RST. Se conserva la distinción entre declaración del fabricante y trazado concreto del ejemplar.
- [Mapa rotulado](wifi-shield/OSOYOO-Wifi-V1.3-pinout.jpg): expone SDA/SCL, pero los números D18/SDA, D19/SCL, A4/A5 y puertos PC4/PC5 pertenecen a UNO/ATmega328P. No son el mapa AVR del Mega.
- [Vista superior](wifi-shield/VistaSuperiorShield.jpg) y [mapa UART](wifi-shield/wifi-pin-map.png): separan selector `ESP URAT` con `E_RX/E_TX` y cabecera `ESP UART` con `GND, ESP TX, ESP RX, 3V3`. Bluetooth tiene conexiones propias; no constituye Bluetooth integrado en ESP8266.
- [Manual del auto, página PDF 43](evidence/manual-p43.png): prescribe D18/TX1 → E_RX y E_TX → D19/RX1, retirando Bluetooth.
- [Ficha Ai-Thinker vigente, V1.1](wifi-shield/ESP-12S-Specification-V1.1-20220807.pdf), páginas 7-10: parámetros, UART0, alimentación y arranque del módulo. [Origen oficial](https://aithinker-static.oss-cn-shenzhen.aliyuncs.com/docs/Public%20Document%20Center/WiFi/esp8266/Specification/ESP-12S%20Specification-20220807.pdf).
- El [Fritzing oficial](wifi-shield/OSOYOO-WiFI-shield-V1.3.fzz) es una pieza/representación del shield, no un esquema interno suficiente: XML con 104 conectores genéricos, sin buses ni registros `connect`; conserva incluso metadatos heredados de resistor y título v1.1. No acredita rutas del traductor, continuidad I2C ni cotas para fabricar nuestra PCB.

## Conexión de operación

| Origen | Destino | Uso |
| --- | --- | --- |
| Mega D18 / PD3 / TX1 | E_RX del selector del shield | Mega transmite al ESP |
| E_TX del selector del shield | Mega D19 / PD2 / RX1 | ESP transmite al Mega |
| GND Mega/shield | GND interfaz | Referencia común |
| Cabecera SDA del shield | SDA de nueva PCB | Mega D20 / PD1 |
| Cabecera SCL del shield | SCL de nueva PCB | Mega D21 / PD0 |

El ESP usa UART0, GPIO1/TX y GPIO3/RX. USART1 es el periférico del Mega, no UART1 del ESP. El cableado operativo se hace a `E_RX/E_TX` del selector, sin sustituirlos inadvertidamente por el puerto directo de servicio. Los jumpers de selección a D4/D5 o D0/D1 deben quedar retirados/aislados al cablear D18/D19; Bluetooth desconectado de esas redes. D5 ya pertenece a dirección del motor RR: no adoptar SoftwareSerial D4/D5 de tutoriales UNO.

El selector muestra dos emparejamientos de operación: E_RX↔D5 y E_TX↔D4, o E_RX↔D1/TX y E_TX↔D0/RX. Identificar por serigrafía y señal, no por izquierda/derecha de una imagen girada. Ninguno de esos emparejamientos debe darse por configurado en el robot a partir de una foto de producto.

Las cabeceras I2C expuestas se mantienen como punto de conexión acordado. Para firmware usar [pinout Mega](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf): D20/SDA y D21/SCL; A4 sigue siendo IR1. La fotografía no verifica continuidad eléctrica de la revisión física del shield. Esta comprobación queda para la etapa de hardware autorizada, sin cambiar ahora el pinout lógico.

## Alimentación y niveles

El fabricante declara regulador del ESP y traducción de niveles para placas de 3,3/5 V. Se actualiza la incertidumbre anterior: esas funciones sí están documentadas. Falta precisar su recorrido por conector y capacidad disponible en el montaje real. La cabecera directa `ESP UART` está rotulada `3V3` y presentada como conexión al módulo: tratarla como lógica de 3,3 V; no atribuirle tolerancia a 5 V por la compatibilidad del shield completo.

La ficha del módulo da 3,3 V nominal y recomienda una fuente capaz de entregar más de 500 mA; no significa consumo permanente de 500 mA ni capacidad libre de ese valor en el shield. No usar el pin 3,3 V de 50 mA del Mega como fuente independiente del ESP.

La página OSOYOO diferencia alimentación interna de 5 V y externa de 7-9 V. VIN/VOUT no equivalen a una salida regulada de 5 V o 3,3 V para la nueva PCB. La compatibilidad UART del shield tampoco resuelve niveles/pull-ups del OLED. Se conservan J2 VCC/GND comunes para OLED y encoder, con tensión numérica pendiente de sus módulos. No unir dos salidas de alimentación ni fijar tensión de la PCB por color de cabecera.

## Firmware, velocidad y flash

El firmware AT de fábrica es reemplazable. El producto acordado sigue siendo Arduino ESP8266 + LittleFS + HTTP/WebSocket + enlace propio al Mega, sin AT, WiFiEsp ni migración a RTOS SDK. Los paquetes AT/FOTA del proveedor son antecedentes de recuperación, no dependencias ni órdenes de instalación.

OSOYOO escribe `9600 kbps`: se registra como errata probable de unidad, interpretada como 9600 bit/s 8N1 para su configuración de fábrica, no 9,6 Mbit/s ni lectura del equipo. La ficha genérica del módulo declara 115200 por defecto. Son configuraciones de firmware distintas; ninguna fija por sí sola la velocidad del nuevo producto. Mantener 115200 8N1 como candidato del enlace, con el error AVR y alternativas documentados en [recursos](RECURSOS_Y_TIEMPOS.md). Baud de aplicación, velocidad de upload y mensajes de ROM son conceptos separados.

La ficha nominal indica 32 Mbit = 4 MiB de flash. El [catálogo Ai-Thinker](https://docs.ai-thinker.com/en/esp8266/) también distingue una variante ESP-12S/ESP8285 de 2 MB; no equipararlas. La lectura del chip/flash real, modo SPI, frecuencia y reparto firmware/LittleFS/OTA siguen pendientes. `esp12e` en PlatformIO es perfil candidato, no detección. El nombre del antiguo archivo AT contiene DOUT; esto no prueba que el nuevo firmware deba usar DOUT ni que DIO/QIO estén comprobados.

LittleFS almacena el bundle Web y datos del firmware dentro de la flash del ESP. Definir layout único antes de crear/cargar imágenes; no importar offsets AT ni el linker ESP01. Firmware y filesystem se preparan por separado. Un fallo de montaje no debe provocar un formateo implícito que borre preferencias: política por implementar, conforme a las [opciones del Core ESP8266](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html).

## Pendientes concretos

1. Revisión real del shield, posiciones de jumpers y recorrido de adaptación de nivel entre selector y módulo; puerto directo identificado separadamente.
2. Ruta para aprovechar USB del Mega: D0 conduce desde el puente hacia ESP RX; D1 recibe ESP TX. No es el emparejamiento normal de operación del selector. Ver [FLASHING.md](FLASHING.md).
3. KEY hacia GPIO0, RST hacia reset ESP, aislamiento del Mega y secuencia manual; documentación funcional disponible, continuidad del ejemplar pendiente.
4. Chip/flash real, modo/frecuencia SPI y layout LittleFS; alimentación/pull-ups de OLED/encoder siguen abiertos.

Hallazgos vinculados: [AFRDOC-007](../documentation/review/FINDINGS.md#afrdoc-007), [011](../documentation/review/FINDINGS.md#afrdoc-011), [012](../documentation/review/FINDINGS.md#afrdoc-012), [013](../documentation/review/FINDINGS.md#afrdoc-013). No modifican IDs wire ni convierten una referencia en hardware probado.
