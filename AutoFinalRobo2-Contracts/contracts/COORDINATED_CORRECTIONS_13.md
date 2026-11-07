# Contrato de correcciones coordinadas — EX01v13

AFR2-20261106-01. Especificación de las fuentes actuales, sin compilación/despliegue/prueba física de esta revisión. ROBO2/perfil2, comandos existentes y transporte framebuffer se conservan. `AFR2_EXT_VERSION=13`; ESP exige coincidencia con ATmega. No se declara compatibilidad con firmware EX01v12 para estos cambios.

## IR digital y EEPROM

`AFR2_CONFIG_VERSION=9`, dos slots de 128 bytes, payload de 63 bytes como v8, CRC y commit conservados. Se leen versiones 1..9. Antes de validar, el valor antiguo `line_kind=2` pasa a 1 e invalida calibración; 0 permanece apagado y 1 digital. Se eliminan las migraciones históricas que forzaban ADC o habilitaban perfiles apagados. Guardar EEPROM continúa siendo explícito/asíncrono. Se preservan umbrales antiguos en el payload por compatibilidad de almacenamiento, pero no se usan como adquisición ni se editan en el producto. Downgrade no garantizado.

El tipo público admite solo 0/1. La batería sigue usando A5/ADC5. IR1..IR5 son A4..A0/PF4..PF0, sin cambio de cableado ni activación física acreditada. Se conservan filtro mayoritario e inversión. `raw` expresa nivel eléctrico; `bits` expresa actividad después de polaridad/filtro. Lectura actual no equivale a calibración válida para seguimiento.

`robot.line.calibrate` conserva `confirm:boolean` y admite `sample:"background"|"line"`. Usa el mismo `AFR2_EXT_LINE_CAL` y token de sesión de cuatro bytes; el quinto byte es 0 confirmar, 1 invalidar, 2 tomar fondo, 3 tomar línea. Respuesta existente: un byte calibrado 0/1. Las muestras exigen lectura actual, IR habilitado y salidas detenidas. Confirmar exige ambas muestras y XOR 0x1f; la polaridad resultante es la máscara del fondo. Los IDs locales 27/28 son estas muestras, ya no canal/umbral numérico; no son parámetros públicos de `robot.calibrate`.

## Wi-Fi persistente

El JSON privado `/config/wifi.json` pasa a esquema `afr2-wifi-2`: `mode` y objetos independientes `sta` y `ap`, cada uno con `ssid` y `password`. Una función todavía no configurada puede carecer de su objeto. SSID hasta 32 bytes; clave vacía para abierto o frase de 8..63 bytes. No se guardan claves en EEPROM ATmega ni se devuelven por HTTP/WS.

La lectura de v1 asigna sus credenciales a AP si `mode=AP`, o a STA para STA/AP_STA. No reescribe el archivo por leerlo; el próximo guardado escribe v2 y conserva el otro rol. AP+STA usa las credenciales STA y el AP independiente guardado; si no existe AP propio, conserva el nombre de recuperación generado por dispositivo.

`wifi.configure` y `wifi.mode` conservan sus nombres. Devuelven aceptación, `persisted:true` y `restartScheduled:true`; `wifi.configure` además anuncia reconexión esperada. No afirman conectado. No se acepta `persist:false`. El cambio de modo requiere configuración válida de su rol; si falta se solicita configuración. Guardado fallido no programa reinicio. El servicio guarda primero y el broker espera al menos un segundo y drenaje de respuestas antes de programar el reinicio. Los mandos quedan sujetos a salidas detenidas y frescura de estado.

Los trabajos OLED de conectar/cambiar modo anuncian reinicio solicitado, no conexión concluida. El estado tras el nuevo boot es la evidencia de resultado. Reintentar STA no implica un nuevo guardado ni reinicio. Se conserva fallback AP tras quince segundos sin STA. La elección de red desde AP prepara STA salvo que se haya elegido AP+STA; no convierte automáticamente cualquier conexión en AP+STA.

Nuevo arranque ESP: invalidar credenciales efímeras, escaneo y job anterior; salir del contexto Wi-Fi al menú principal. Si hay ajustes o medidas pendientes, mantener Sí/No antes de abandonar sus cambios. El aviso/identidad de arranque repetido no reinicia el flujo otra vez.

## Catálogo paginado

Capacidad ESP: dieciséis entradas de mayor RSSI. La lista puede incluir redes con el mismo SSID/BSSID distinto; no se fusionan credenciales distintas. `truncated` significa que el SDK obtuvo más resultados que el catálogo. El ATmega conserva ocho cadenas, no dieciséis. Se mantienen los IDs de pantalla.

`wifi.scan` responde `accepted` con `scanId`; `wifi.scan.result` devuelve ese identificador, estado, redes y truncamiento. El resultado terminal se retiene para su sesión bajo presión de salida. Un nuevo escaneo no comienza mientras su resultado anterior espera entrega. Desconexión cancela la espera Web y evita reutilizar resultados de otra sesión.

### EX01 OLED

- `WIFI_POLL`: nueva operación 12 = pedir página. Su cuerpo fragmentable tiene cinco bytes: `scanId:u32LE` y página 0/1. Los campos existentes de job y offset se mantienen.
- `WIFI_UPDATE`, subtipo 6, trece bytes: `[6, jobId:u32LE, scanId:u32LE, page:u8, total:u8, truncated:u8, pageCount:u8]`. Límites: total 0..16, pageCount 0..8 y conteo exacto para la página. ScanId no nulo; página 1 exige más de ocho entradas. Una petición de página debe coincidir con la identidad vigente.
- Subtipo 1 conserva su forma: índice local 0..7, longitud, seguridad, cantidad de esta página, jobId y SSID. Exige metadatos previos, job vigente y conteo coherente. Los SSID no entran por una operación de otro tipo.
- El resultado terminal exitoso solo se acepta tras metadatos y máscara completa de recepción, incluso con cero entradas. Un fallo no expone una página parcial como lista lista.

El menú agrega anterior/siguiente cuando corresponde, refrescar y volver. La búsqueda muestra solo «Buscando redes» mientras está pendiente. Vacío y fallo no se rotulan como redes encontradas. Un resultado truncado lleva advertencia explícita de dieciséis redes máximas y marcador en el encabezado.

## WS y salida limitada

Una sesión y cuatro mensajes de transporte, conservando las restricciones anteriores de entrada. Los snapshots opcionales solo se envían con `queueLen<2`. Una respuesta serializada diferida de hasta 8192 bytes se reintenta desde `tick`; dos segundos sin poder enviarla cierran por consumidor lento. STOP no queda detrás del despacho ordinario. Se conserva una ranura acotada de metadatos para el error de una petición cancelada mientras otra respuesta espera salida. Exceder la concurrencia admitida sigue cerrando explícitamente por política; no se acumulan colas sin límite.

`outputBackpressure` y `replyDeferred` se agregan al diagnóstico. El cierre observado por cliente, el ordenado por ESP, bootId y estado UART son datos distintos. Una Web nueva no amplía permisos de movimiento por interpretar estas métricas.

## OLED y copia Web

Se conserva corto/largo al soltar, umbral 800..3000 ms y Sí/No obligatorio. Icono L y anillo de dieciséis sectores en caja inferior derecha; datos y opciones reservan ese espacio, incluyendo scroll de líneas largas. QR excluye el overlay para conservar la matriz/zona de silencio. No se agregan fuentes ni hardware I2C.

Rápidas invalida por entrada, estados, confirmación y progreso; no por un reloj fijo para contenido estático. El diagnóstico de pantalla distingue tiempo del raster de la última transferencia de datos I2C, con resolución en ms; no representa latencia total de un frame ni una prueba ya ejecutada.

Renderers C y TS cambian juntos. Los hashes se basan en UTF-8/LF tras revisar las adaptaciones; copiar catálogo o cambiar hashes no sustituye el port. El archivo generado preexistente se conserva hasta la regeneración autorizada.
