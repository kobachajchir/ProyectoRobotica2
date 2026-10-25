# AFR2-20261023-15 — Compilaciones ESP/Web y correcciones

23/10/2026. Pedido expreso del usuario de compilar el trabajo anterior y corregir errores. AVR se está desarrollando en paralelo; no se compiló ni modificó ese módulo en esta ficha. El nuevo protocolo se adaptará después de recibir su documentación ATmega; compilar el perfil provisional no acredita interoperabilidad ni cierra ese contrato.

## Resultados

| Área | Comando | Resultado final |
| --- | --- | --- |
| ESP | Python PlatformIO `-m platformio run -e esp12s_candidate` | SUCCESS; cero warnings en recompilación final; ELF/BIN generados |
| Web | `npm.cmd run build` | TypeScript, Vite y postbuild LittleFS completos; exit 0 |
| Artefactos Web | Descomprimir cada gzip y cotejar tamaño/hash contra manifiesto | 6/6 correctos; totales coincidentes |

ESP: RAM estática 39.980/81.920 bytes (48,8 %); programa 385.481/1.044.464 bytes (36,9 % según perfil). BIN 389.632 bytes. No es medición de heap/pila en ejecución, flash física ni partición real. Web: `dist/index.html` 1.477,14 kB y 404,30 kB gzip informado por Vite; paquete total 2.925.630 bytes gzip, seis assets. Paquete generado solo en Web/artifacts/littlefs/dist, sin promover a ESP/data/dist.

## Errores y correcciones por archivo

- `ESP/src/broker.cpp`: advertencias Wclass-memaccess por memset sobre Session, BrokerRequest y WifiConfig con inicializadores C++. Sustituidos por reinicialización tipada; conserva limpieza de campos y defaults propios. No se silenció el warning.
- `ESP/src/wifi_service.cpp`: mismo ajuste para config local/pending. Sin cambio de wire ni comandos.
- `Web/tools/package-web.mjs`: tsc/Vite pasaban, pero postbuild fallaba con EPERM en rename de dist.staging→dist. Se reprodujo el fallo. Ahora, si Windows rechaza rename con EPERM/EACCES/EBUSY, publica por archivos y escribe el manifiesto al final. Verifica rutas absolutas/directorios antes de cada limpieza recursiva y rechaza enlaces. No afirma atomicidad de intercambio de directorios en el fallback. Esta ruta alternativa se ejecutó exitosamente en la compilación final; los seis archivos se descomprimieron y cotejaron por SHA-256.

Procedencia: adaptaciones propias sobre AFR2-20261023-11 (ESP) y AFR2-20261023-09 (empaquetador). No se tocaron referencias ni AutoMicro. El middleware TCP adaptado declarado en ESP se ejecutó durante la compilación sobre dependencias instaladas, sin ejecutar manifests históricos.

## Evidencia y límites

[Resultados y hashes](../documentation/builds/2026-10-23-15-esp-web/results.json), [ESP final](../documentation/builds/2026-10-23-15-esp-web/esp-final.txt), [Web final](../documentation/builds/2026-10-23-15-esp-web/web-final.txt), intentos iniciales esp-attempt-1.txt/web-attempt-1.txt en la misma carpeta. Hashes de fuentes capturados después de compilar; el BIN se conserva como salida regenerable ignorada por Git. [Resumen de compilación](../documentation/builds/2026-10-23-15-esp-web/README.md).

Stack: Node 22.13.0, Vite 7.3.7 resuelto por lockfile; espressif8266 4.2.1, Core ESP8266 3.1.2, ESPAsyncTCP 2.0.0, ESPAsyncWebServer 3.10.3 y ArduinoJson 7.4.2. El build usa el perfil candidato esp12e; sus límites nominales no identifican el hardware conectado.

Estado: atmega sin cambios en esta ficha; esp compilada con perfil candidato, sin verificar en hardware; web compilada/empaquetada localmente; shared con evidencia de compilación y actualización documental. Sin buildfs, imagen LittleFS, upload, apertura de puertos ni pruebas físicas. El enlace nuevo permanece pendiente de documentación del equipo ATmega. No se hizo push ni commit.
