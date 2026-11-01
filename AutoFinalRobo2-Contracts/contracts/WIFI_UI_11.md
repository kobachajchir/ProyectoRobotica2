# AFR2-20261030-09 — Wi-Fi OLED/Web, EX01v11

Estado: fuentes coordinadas ATmega/ESP/Web, sin generación, compilación, pruebas, validadores, navegador, promoción ni carga. EEPROMv8 no cambia. La evidencia de compilaciones anteriores no corresponde a estas fuentes.

## Árbol, identidad y retorno

| Pantalla | ScreenCode | Contenido |
|---|---|---|
| Wi-Fi | 03001f | Estado de red / Cambiar modo / Buscar redes / AP-fallback / Volver |
| Estado de red | 030020 | Cabecera Conectado/Desconectado/Sin datos actuales; Estado de conexión / Datos de red / Configurar / Conectar o Desconectar / Reintentar si desconectado / Volver |
| Estado de conexión | 03002d conservado | Dos páginas acotadas: modo e IP elegida, luego máscara y gateway; corto/largo regresan |
| Datos de red | 030041 nuevo | SSID completo con scroll, RSSI STA o --, seguridad y candado si protegida |
| Buscar redes | 030042 nuevo | Redes encontradas / Refrescar con recurso Refresh / Volver |
| Redes encontradas | 030021 | 0..8 SSID reales más Volver; Red oculta para SSID vacío; candado al final de fila protegida |
| Ingresar credenciales | 030022 | Ingresar aquí / Ingresar en Web / Volver |
| Configurar | 030023 conservado | Modificar credenciales / Conectar o Desconectar / Volver |
| Cambiar modo | 030043 nuevo | Opciones de modos; STA / AP / AP-STA; subrayado de selección; giro limitado, corto solicita, largo regresa |
| AP / fallback | 030024 | Modo AP / Modo AP-STA / Volver |

El estado de conexión resume STA. Un AP activo se informa como AP, sin atribuirle conexión STA ni intensidad de señal STA. En AP-STA se muestra IP STA si está conectada; de lo contrario, la IP del AP activo. La Web nativa muestra por separado las direcciones STA y AP. Los datos caducan a los 5000ms. Las acciones de conectar/desconectar y reintentar no se ofrecen como estado conocido si falta información actual. La revisión de navegación cambia al variar conexión o cantidad de redes; se ajustan selección e inicio visible.

Redes listas es un aviso terminal; al cerrarlo por corto, largo o vencimiento y vaciar los avisos previos se abre automáticamente 030021. Elegir un SSID abre directamente 030022. Ingresar en Web se decide en el ESP contra sesiones usadas, handshake listo y transporte WebSocket WS_CONNECTED. Un cliente Wi-Fi/AP sin WebSocket no acredita esa condición. Sin cliente se muestra No hay clientes / Conectados y se vuelve al listado. Si desaparece el último cliente durante la espera, termina con el mismo resultado. No se aplica ninguna credencial por elegir la entrada Web.

Configurar pasa dentro de Estado de red. Revisar y Cancelar borrador desaparecen de las filas activas; cancelación/expiración internas del protocolo se conservan para no dejar credenciales en espera indefinida. Las tablas heredadas permanecen como offsets de migración, sin ofrecer esas filas. Una configuración ya preparada requiere confirmación física Conectar como antes. Modificar credenciales vuelve a elegir entrada local o Web. Elegir una red en AP propone AP-STA para conservar el acceso de provisión.

## Seguridad y origen de datos

Enum de presentación: 0 desconocida, 1 abierta, 2 WEP, 3 WPA, 4 WPA2, 5 WPA/WPA2, 6 protegida sin tipo conocido. Abierta no lleva candado. Desconocida no se interpreta como abierta ni se convierte en WPA2 por la longitud de una clave. El AP informa el modo de autenticación del SDK. Para STA se usa la seguridad anunciada en el último escaneo del BSSID exacto, conservada hasta 120000ms para como máximo16 BSSID; después se informa desconocida. No representa una lectura del cifrado negociado de la sesión. No se inicia un escaneo oculto al abrir Estado. Puede obtenerse el dato mediante Refrescar. La visualización de WEP no añade soporte nuevo para claves WEP a las reglas existentes de credenciales.

SSID hasta32 bytes UTF-8 de origen ESP; el raster conserva su fuente ASCII y fallback para caracteres no disponibles. Se envían nombres completos, sin reemplazarlos por Red1/Red2. OLED conserva las primeras8 redes; Web nativa, las primeras16 y su indicación de truncamiento. Redes ocultas pueden elegirse, pero requieren introducir su SSID. No se publican claves STA en status, scans, logs, capturas o almacenamiento del navegador. El antiguo detalle AP con clave continúa sujeto a las guardas de captura existentes y deja de ser acceso principal.

Máscara y gateway provienen de subnetMask/gatewayIP para STA y wifi_get_ip_info para AP. Un valor no disponible se presenta como desconocido; el dato actual está separado de los campos persistentes de configuración.

## Contrato UART

Se conserva ROBO2/profile2 y un único productor MegaLink. EX01 pasa10→11 una sola vez para esta entrega. ESP habilita trabajos de producto únicamente tras negociación de versión exacta11; no se interpreta el nuevo subtipo1 con un consumidor10. Los comandos nativos Wi-Fi siguen siendo locales al ESP y pueden funcionar sin producto AVR listo.

Se conservan WIFI_POLL y WIFI_UPDATE, sus IDs y el envelope de respuesta de9bytes. Todas las longitudes siguientes corresponden al body sin framing ni envelope. Enteros multibyte en little endian.

WIFI_POLL request: offset:u8,1byte. Respuesta existente: pending:u8, jobId:u32LE, op:u8, total:u8, offset:u8, count:u8, requestedMode:u8, draftToken:u32LE, data[count],14+count bytes. Modos1 STA,2 AP,3 AP-STA. Se agregan op9 Desconectar y op10 Cambiar modo; op10 toma requestedMode, ambos sin credenciales. No se agrega otro dueño del UART.

| WIFI_UPDATE subtipo | Body completo | Longitud |
|---|---|---|
| 0 estado base, conservado | subtype0, mode, connected, rssi:i16LE, ssidLen, ssid[ssidLen], staIP[4], apIP[4] | 14+ssidLen |
| 1 red encontrada, nuevo formato v11 | subtype1, index0..7, ssidLen0..32, security0..6, total1..8, jobId:u32LE, ssid[ssidLen] | 9+ssidLen, máximo41 |
| 2 resultado, ampliado | subtype2, jobId:u32LE, result0..6, draftToken:u32LE, ssidLen, ssid[ssidLen] | 11+ssidLen |
| 3 versiones, conservado | subtype3, subject0..1, length0..15, version[length] | 3+length |
| 4 datos de estado, nuevo | subtype4, mode, connected, staSecurity, apSecurity, readyWsClients, staIP[4], mask[4], gateway[4] | 18 |

Subtipo1 se acepta solo con trabajo SCAN pendiente, id exacto, índices/total/longitud válidos y sin NUL embebido. La entrada0 reinicia el listado; un escaneo vacío ya lo dejó vacío al iniciarse y no envía entradas. Los nombres y candados pertenecen al mismo paquete correlacionado. Result3 del scan se emite después de transmitir las entradas; el aviso no sustituye al listado.

Subtipo4 se envía después de recibir OK para el estado base asociado, comparte modo/conectividad/IP STA y el AVR rechaza una discrepancia. Máscara/gateway pertenecen a STA si connected=1, en otro caso a AP. Se registra vigencia propia; no completa datos desconocidos con una medición inventada.

Resultados: 1 borrador listo (solo credenciales, token no cero y SSID), 2 solicitud de conexión/desconexión aceptada y pendiente, 3 operación terminal exitosa, 4 fallida, 5 cancelada, 6 sin cliente WebSocket listo (solo WEB_DRAFT). Result0 es ausencia de resultado interno y no cierra el trabajo. AVR exige id y trabajo reclamado por ESP; ignora resultados ajenos o sin espera. Se conservan timeout AVR30s general/120s Web, borrado de claves de RAM y vencimiento de borrador.

Desconectar solicita WiFi.disconnect(false,false), preserva la configuración STA y deshabilita la reconexión automática. El ESP observa staConnected=false antes del resultado3; plazo de observación5000ms. La desconexión del usuario también inhibe el reintento periódico propio; Reintentar o aplicar credenciales revoca esa inhibición. Una interfaz STA ya desconectada puede confirmar el estado sin inventar una transición. Reintentar solo acredita conexión STA, nunca un AP activo. Conectar observa STA o AP según el modo del borrador durante hasta25000ms. Son presupuestos de software, no latencias medidas. Cambiar modo confirma el modo real del SDK y arranque del AP requerido; no escribe LittleFS ni sustituye credenciales.

Las mutaciones OLED conservan las guardas existentes de salidas detenidas y disponibilidad; las acciones nuevas y selección de modo siguen excluidas de la ejecución remota por ui.action. STOP, heartbeat y framebuffer siguen usando los canales existentes.

## JSON Web nativo

- wifi.status agrega staMask,staGateway,apMask,apGateway; securityCode/security,secure nullable; apSecurityCode/apSecurity; disconnectedByUser. wifi.info agrega además wsClients listo. Conexión perdida o datos caducados no equivalen a desconectado.
- wifi.scan conserva aceptación y evento terminal; cada candidato agrega securityCode y security; el evento distingue error/lista vacía/lista completa. No se inicia otra exploración desde la Web mientras un borrador o conexión OLED ocupa el servicio.
- wifi.disconnect {}, wifi.retry {} y wifi.mode {mode:"STA"|"AP"|"AP_STA"} se atienden localmente en ESP. Respuesta: snapshot de estado más confirmed:boolean, envelope accepted o completed. Disconnect/retry solo completed si se observa el estado esperado; mode solo confirma tras comprobar el modo real. Errores usan el envelope normal. No se persiste un cambio de modo de sesión.
- La Web nativa distingue aceptación de éxito. Tras configurar/reintentar/desconectar consulta wifi.status sin bloquear, cada700ms después de cada respuesta, durante hasta25s/5s según operación. Solo un snapshot terminal confirma éxito. Si se pierde el WebSocket, muestra resultado sin confirmar y requiere volver a consultar; no afirma que el comando falló o tuvo éxito por la mera caída del transporte.
- Buscar redes abre el listado al recibir el evento completed. Elegir una red abre el formulario Web; no simula teclas físicas. Configurar está en Estado y conserva el formulario seguro y la confirmación explícita de Conectar.

## Raster, recursos y costo pendiente

C y TypeScript comparten coordenadas, selección, páginas, clip, scroll y bytes del nuevo candado8x12 (12bytes flash, autoría AFR2 propia). Refresh reutiliza el bitmap existente. Ayudas de corto/largo usan el recurso de encoder y L completa, sin superposición; etiquetas de acción capitalizadas. También se sustituyeron ayudas textuales en editores, pruebas, radar, avisos y dashboard, conservando su comportamiento. Cambiar modo tiene copia local navegable que no modifica la radio. El visor conectado sigue mostrando el framebuffer real1024bytes.

Campos nominales agregados: Afr2Wifi24bytes (seguridades8+4, vigencia4, máscara/gateway8), Afr2Ui2bytes (destino diferido/firma Wi-Fi), Afr2UiSnapshot3bytes de seguridad por fila. Es una cuenta de campos, no un tamaño enlazado ni una aprobación de SRAM/pila. Flash suma candado12bytes, entrada de recurso, tablas/etiquetas/helper/código cuyo costo total queda sin medir. ESP agrega campos nominales118bytes al servicio y27 al broker antes de padding, además de usos temporales de JSON/String/SDK. No se declara margen suficiente ni seguridad de pila.

wifiCatalogue.ts/reformResources.ts y firmwareHints.ts mantienen la contraparte explícita hasta regeneración autorizada. sync-oled-library.mjs queda preparado para wifiReform[], sin ejecutarse; generated/firmwareLibrary.ts conserva la entrega anterior. Los hashes revisados solo se actualizan después del port manual. Una futura generación requiere comparar antes de retirar las compensaciones; no se promueve un paquete Web antiguo como si contuviera esta entrega.

Referencias de API consultadas (lectura, no prueba del firmware instalado): [ESP8266WiFiSTA.h oficial](https://raw.githubusercontent.com/esp8266/Arduino/master/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h), [ESP8266WiFiScan.h oficial](https://raw.githubusercontent.com/esp8266/Arduino/master/libraries/ESP8266WiFi/src/ESP8266WiFiScan.h). La revisión del SDK y del empaquetado efectivos requiere compilación autorizada posterior.

## Comprobaciones posteriores, no ejecutadas

Revisar compilación coordinada EX11 y rechazo EX10; generación autorizada y fidelidad raster con mismo snapshot/reloj/frame previo; flash/SRAM/pila; scan0/1/8/16/ocultas/SSID32/Unicode/truncado/paquete de otro job; seguridad abierta/protegida/desconocida/caducada; STA/AP/AP-STA, máscaras/IP sin datos; modo aceptado/rechazado y pérdida del enlace al cambiarlo; corto/largo/Volver y límites de página; cliente AP sin WebSocket, WebSocket no listo/listo/desconectado; timeout/borrador/listado automático; conectar/reintentar/fallo/desconectar confirmados y reconexión inhibida; continuidad de STOP/heartbeat; mensajes tras pérdida de transporte. Generación, build, promoción, carga y hardware deben llevar evidencia separada.
