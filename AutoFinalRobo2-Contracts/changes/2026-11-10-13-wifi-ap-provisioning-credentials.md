# AFR2-20261110-13 · Wi-Fi desde AP y credenciales

Versión: 1.21.0 (subversión).

Pedido: revisar búsqueda desde AP, carga de credenciales desde Web, conexión y cambio de modo, configuración/reinicio AP y visualización de la última red conectada.

Hallazgos: bloqueo AP duplicado en Web/ESP; formulario AP sembrado con identidad STA; cambio STA sin credenciales que anunciaba un reinicio inexistente; ausencia de consulta privada de última conexión y archivo con límite de lectura menor que lo que el escritor podía guardar.

Fuentes corregidas con escaneo AP temporal mediante AP-STA, configuración AP/STA separada, nuevo comando ESP nativo sensible wifi.credentials.get, registro de conexión real y comprobación Web por boot/modo/SSID. Archivo Wi-Fi acotado a 1024 bytes en lectura y escritura. [Contrato, mapa de archivos y comprobación física pendiente](../contracts/WIFI_AP_PROVISIONING_20261110.md).

Estados: atmega=solo versión; esp=fuente modificada sin compilar; web=fuente modificada sin compilar. Sin cambios de UART, EX01, EEPROM, Loader o CAD. No se ejecutaron tests, builds, validaciones, generadores, navegador, puertos o flasheos. La revisión estática no confirma funcionamiento físico. Se conserva la instrucción de trabajar solo en fuentes. Sin commit ni push.
