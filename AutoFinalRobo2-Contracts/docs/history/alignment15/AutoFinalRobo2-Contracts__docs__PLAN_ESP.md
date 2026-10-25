> **Continuidad AFR2-20261024-08:** [estado por módulo](ESTADO_IMPLEMENTACION.md) y [EX01v5](../contracts/EX01_NETWORK_UI_5.md). Firmware0.7.0,48nodos,11avisos, IP/AP local, iconos/scroll y Sistema → Reinicio compilados; Web actualizada/promovida. Clave AP excluida de HTTP/WS/captura protegida; clave STA oculta. SRAM estática4797bytes no acredita pila; memoria/Hall0 se conservan. No pruebas físicas/flash ni nuevo acceso de navegador. CAD/pinout intactos. Estados inferiores históricos.

> **Avance AFR2-20261024-07:** EX01v4/info/reinicio/boot y puente de etiquetas compilados; Web promovida. [Matriz actual](ESTADO_IMPLEMENTACION.md#esp) · [Contrato v4](../contracts/EX01_SYSTEM_SEQUENCE_4.md). Sin verificación UART/radio/reset en placa ni flasheo; flash4MiB/LittleFS3MiB candidato. Los estados previos inferiores son históricos.

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../contracts/EX01_ALIGNMENT_3.md) y [evidencia](../changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Implementación ESP y enlace Mega

## Pendientes independientes del Mega — AFR2-20261023-20

Completado y compilado el flujo Wi-Fi ESP/Web: scan espera resultado terminal, errores/timeout/red vacía separados, status automático sin Mega, validación UTF-8 y elección de persistencia. ESP informa aplicación STA/AP y fallback. [Ficha y builds](../changes/2026-10-23-20-wifi-esp-web.md). No se editó UART/ROBO2, broker, Mega ni lector robot.state.

Queda para continuar sin intervenir ese enlace:

- Identificar flash/SPI con placa y script preparados; después elegir layout LittleFS con firmware, overhead y preferencias. Sin medición todavía.
- Verificar radio en ejecución autorizada: arranque sin preferencias, STA incorrecta/desconectada, AP/AP_STA/fallback, scan vacío/fallido, reconexión, persistencia RAM/flash y montaje fallido sin borrar datos. El build no lo acredita.
- Diagnóstico de filesystem/versión de paquete en la Web mediante servicios locales ESP.
- Robustecer promoción Web→ESP/data/dist y promover paquete coherente. Imagen FS solo con layout identificado; preservar preferencias antes de una carga completa.
- Verificar HTTP gzip/MIME/cache, assets faltantes/paquete inválido y límites de clientes. Renderizado e interacción Wi-Fi en navegador/dispositivo pendientes de pedido.

OLED, sensores/pose, STOP, lease y movimiento dependen del trabajo Mega; quedan fuera de esta línea. El resto del documento conserva la historia y sus estados originales.

AFR2-20261023-11, 23/10/2026. Continuación del [roadmap](ROADMAP.md) desde Web AFR2-20261023-09, las guías de módulos ESP y la referencia AutoMicro/ESP01 conservada. Plan de código fuente; no autorización de pruebas ni carga.

| Etapa | Resultado | Estado |
| --- | --- | --- |
| Revisar consumidores | Web afr2-ws-1, manifest LittleFS y árbol funcional; localizar servidor/transporte/Wi-Fi ESP01 | Leído, sin modificar referencias |
| Filesystem/HTTP | Un dueño del montaje, no autoformat, whitelist manifest, gzip/MIME/cache, diagnóstico sin bundle | Escrito en ESP/src/storage.* y main.cpp |
| Wi-Fi | STA/AP/AP_STA, preferencias temporales+rename, scan asíncrono, fallback/reintentos, sin secretos en fuentes | Escrito en ESP/src/wifi_service.* |
| Broker Web | /ws mismo origen, sesiones/colas acotadas, hello/alive, requestId, capacidades dinámicas, errores explícitos | Escrito en ESP/src/broker.* |
| Enlace Mega | ROBO2/perfil 2 (sustituye UNER/perfil1), sesión/transacción, ruta por nibbles, longitudes/validación/timeout, STOP prioritario | Fuente compartida Contracts + ESP/src/mega_link.* + AVR endpoint, AFR2-20261023-15; sin compilar/probar |
| Plataforma AVR | USART1 IRQ, Timer0 1 ms, TWI/Manager/OLED/encoder local con potencia inactiva; opción UART sola | Integración escrita en AFR2-20261023-15; sin build/hardware ni productores de sensores/potencia |
| Productores del robot | Estado/potencia, STOP autoritativo, sensores5/HC-SR04, luego lease y control por rueda | Pendiente; no habilitado en capabilities |
| Servicios UI/Wi-Fi hacia Mega | Estado de radio/preferencias visibles en OLED y configuración solicitada desde menús | Pendiente de contrato específico y skill de pantallas; no se importan IDs Wi-Fi F4 |
| Verificación y entrega física | Build ESP/AVR, mapa memoria, pruebas de correlación/fragmentación/timeout/LittleFS, luego carga/banco | Pendiente de pedido expreso |

La composición nueva sirve la Web aun con el Mega ausente, siempre que LittleFS tenga un paquete coherente. Una sesión WS lista no concede mando. Los snapshots por UART empiezan inválidos/unknown; callbacks conectarán los productores cuando existan. La composición de comunicaciones no anuncia STOP efectivo ni acceso remoto a OLED/encoder.

## Continuación concreta

AFR2-20261023-15: build ESP y Web/postbuild ya ejecutados y corregidos, con [evidencia](../documentation/builds/2026-10-23-15-esp-web/README.md). Esto completa la compilación inicial y el paquete Web local, sin identificar hardware ni verificar runtime. No se compiló AVR en esta ficha; esperar la documentación específica del nuevo protocolo ATmega antes de adaptar el enlace. Falta promover Web a ESP/data/dist cuando se acuerde; no se generó imagen FS ni se flasheó.

Herramienta preparada en AFR2-20261023-12: [diagnóstico ESP de una ejecución](../hardware/ESP_DIAGNOSTICO.md). Cuando esté conectada la placa, primero sincroniza; después consulta flash y cabecera y guarda `hardware/diagnostics/<intento>/report.md`. Todavía no se ejecutó. Usar ese informe para resolver flash física y discrepancias con cabecera; la partición se elegirá/confirmará con linker y firmware conocido.

1. Revisar fuente/contratos de esta ficha. Identificar flash/SPI reales y elegir un layout común firmware/LittleFS con margen para 2.925.405 bytes de payload comprimido, metadata/overhead y configuración. OTA no se presume disponible. No tomar esp12e ni la ficha nominal como medición.
2. Cuando se solicite: instalar/resolver las versiones declaradas, compilar ESP y la composición AVR opcional, obtener mapas/margen de SRAM/pila/heap, corregir APIs o memoria según resultados. Los parches TCP preservados se aplicarán solo al compilar y fallan si cambian sus anclas.
3. Cuando se solicite: comprobar parser con ruido/longitud/ruta/XOR, ACK tardío de otra sesión/transacción, STOP durante TX parcial, RX overflow, cambio de sesión, timeout y rollover; WS con dos clientes, límites/consumidor lento; archivos ausentes, MIME/gzip/406/404, mount fallido sin borrar preferencias y persistencia interrumpida.
4. Recompilar/empaquetar/promover Web para incluir la fuente ajustada en AFR2-20261023-11. El bundle previo todavía puede saludar/consultar Wi-Fi, pero no contiene el nuevo consumo de system.status ni el guard adicional de confirmación STOP.
5. Antes de cargar: preservar preferencias; separar imagen firmware e imagen FS, cruzar USB Mega D0/D1 solo por la ruta documentada y mantener operación D18/D19 aislada de los selectores D4/D5 y D0/D1. No abrir puertos ni flashear por el solo hecho de completar el plan.
6. Integrar servicios reales AVR mediante Afr2LinkOps y extender los tres consumidores con fichas AFR2 antes de asignar nuevos IDs, lease, consignas, streams o acciones OLED. Motores/GOTO requieren sus productores y condiciones reales.

## Referencias técnicas utilizadas

Referencia local conservada: ESP/reference/esp01/src/webserver_ws.cpp, ws_bridge, uart_transaction_manager.c, uner_core.c, uner_v2.h, wifi_service.cpp, nvs_littlefs.cpp y tools/websocket_copy_buffers.py. [Inventario por archivo](../provenance/esp-firmware.json). No se ejecutaron los manifests ni scripts heredados.

APIs consultadas: [LittleFS ESP8266 y autoformat](https://arduino-esp8266.readthedocs.io/en/stable/filesystem.html), [scan asíncrono](https://arduino-esp8266.readthedocs.io/en/stable/esp8266wifi/scan-class.html), [ESPAsyncWebServer 3.10.3](https://github.com/ESP32Async/ESPAsyncWebServer/tree/v3.10.3), [PlatformIO ESP8266](https://docs.platformio.org/en/latest/platforms/espressif8266.html). Estas fuentes justifican las APIs elegidas, no sustituyen compilación ni hardware probado.


## ROBO2/perfil2 — AFR2-20261023-15

El framing anterior se sustituye por [perfil2](../contracts/UART_PROFILE_2.md)
y validación compartida con nibble emisor/receptor. Receptor ESP y endpoint AVR
adaptados en fuente; TWI/OLED/encoder integrados localmente en AVR. Documento
para retomar desde ESP: [PROTOCOLO_ROBO2.md](../../AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md).
La verificación sigue pendiente bajo pedido expreso. Sensores/potencia/STOP,
lease/Hall/GOTO/UI remota y estado Wi-Fi hacia OLED permanecen pendientes.


## Integración vigente — AFR2-20261024-01

Revisadas fichas19/20/21. La Web21 ya contiene los cambios visuales/editor/simulador;
Wi-Fi20 se conserva. La integración de firmware pendiente se adapta en fuente:
ROBO2/perfil2 + EX01 v2, estado/pose, mando supervisado, framebuffer OLED y
secuencias0x40..0x45 sin colisión con comandos de producto19.
Sin nuevo build, validador, tests, flasheo o promoción LittleFS. Secuencias físicas
permanecen bloqueadas por defecto hasta aprobar SRAM/pila y calibración real.
Contrato en AutoFinalRobo2-Contracts/contracts/EX01_WEB_INTEGRATION.md y ficha
AutoFinalRobo2-Contracts/changes/2026-10-24-01-firmware-web-integration.md.



