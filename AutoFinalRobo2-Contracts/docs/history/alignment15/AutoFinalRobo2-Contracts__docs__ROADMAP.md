> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](../changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../contracts/EX01_ALIGNMENT_3.md) y [evidencia](../changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Plan de implementación

Actualización ATmega19,24/10/2026: [fuente y límites](ATMEGA_IMPLEMENTACION_19.md),
[matriz M01..M19](PLAN_ATMEGA.md) y [EX01 para ESP/Web](../contracts/ROBO2_PRODUCT_EXTENSION.md).
M01..M18 cubiertos en fuente, sin build/ejecución; Hall/calibración/hardware y
complemento ESP/Web pendientes. Perfil3/secuencias de21 permanece diseñado.
Las menciones previas a menús/motores/sensores como fuente pendiente son históricas.


Estado histórico AFR2-20261023-15: ROBO2/perfil2 sustituye UNER/perfil1 en fuente
de ambos extremos; framing único compartido, ruta alto=emisor/bajo=receptor.
AVR integra UART1/Timer0/TWI/I2C Manager/OLED/encoder en main.c. Backend ya
escrito; faltan compilación, recursos reales, tiempos y hardware. Motores,
sensores, STOP autoritativo, lease, Hall/GOTO, UI remota y menús completos siguen
pendientes. No se ejecutaron builds/tests/validadores/flasheos. El texto anterior
se conserva como evolución del proyecto. Leer [ficha15](../changes/2026-10-23-15-robo2-atmega-ui.md),
[perfil2](../contracts/UART_PROFILE_2.md), [criterios comunes](../contracts/ROBO2_VALIDACION.md)
y **[ESP/docs/PROTOCOLO_ROBO2.md](../../AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md)**
para retomar firmware ESP sin duplicar lo ya escrito.


Actualización AFR2-20261023-11: los puntos de servidor ESP, LittleFS/Wi-Fi, broker WS/UART y backend USART1 AVR están escritos en fuente, junto con composición opcional de comunicaciones. [Plan ESP detallado](PLAN_ESP.md), [perfil UART 1](../contracts/UART_PROFILE_1.md) y [ficha](../changes/2026-10-23-11-esp-server-and-mega-link.md). No se ejecutaron compilaciones/pruebas/cargas de esta implementación. TWI y productores del robot siguen pendientes; la Web fuente ajustada aún requiere reconstruir el paquete.

## 0. Hardware

El módulo [AutoFinalRobo2-CAD](../../AutoFinalRobo2-CAD/README.md) ya reúne la ubicación de KiCad, pieza nueva y modelo completo del auto bajo el Git global. Sus insumos existentes no son un diseño adaptado. Seguir el [plan de Fusion](../../AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md) para reconstruir el auto base sin cables, con documentación/CAD e inferencias explícitas. Pieza nueva, soporte y PCB pendientes de planos; [organización y pendientes](../../AutoFinalRobo2-CAD/docs/ALCANCE_Y_PENDIENTES.md).

Identificar OLED/controlador/tensión, pull-ups y encoder UI. La documentación del shield UART V1.3 y regulación/traducción declaradas están en [WIFI_SHIELD](../hardware/WIFI_SHIELD.md); faltan revisión física, recorrido por selector/puerto directo, flash real y configuración SPI/LittleFS. Confirmar rueda/canal/polaridad. Elegir captura HC-SR04 en D31 o documentar recableado. Definir Hall de cuatro ruedas sin ocupar la nueva UI/TWI.

## 1. AVR y UI

Reloj monotónico, UART1 por IRQ y TWI no bloqueante para el Manager preparado, con NACK/STOP/timeout/recovery. Main cooperativo y wrappers/encoder. Leer CLK/DT conjuntamente desde PING, SW desde PINL. Largo abre menú sin corto posterior; Volver solo por ítem. Migrar una pantalla completa antes del resto.

## 2. Comunicación

Cerrar hello/capabilities, STOP, estado, versión, sensores y lease. Asignar IDs/longitudes en ficha AFR2 y adaptar tres consumidores. Parser UNER, transacciones ESP y cliente Web reutilizables. No habilitar motores por tener WebSocket conectado ni exponer catálogo F4 completo.

## 3. Sensores y potencia

HC-SR04 con validez/caducidad y línea5 con polaridad real. Cuatro ruedas PWM/rampas/inversión, timeout y watchdog. Pruebas por rueda antes de mezclar Mecanum, solo tras pedido expreso.

## 4. Movimiento

Confirmar PPR/reducción/signo, carga IRQ y geometría. Captura Hall, control por rueda, cinemática/odometría, línea y GOTO relativo. Ultrasonido frontal no implica cobertura lateral/trasera. No publicar exactitud no medida.

## 5. Web/pantallas

Portar Control/Test/GOTO/Wi-Fi/sensores desde reference/web. UI por capabilities, unknown/stale/resultados reales. Modelo Mecanum nuevo; GLB anterior solo referencia. Portar wrappers sin MPU/balance. Separar identidad, estado y raster; SVG anterior no acredita fidelidad.

## 6. Ejecución futura autorizada

Build AVR y mapa SRAM/flash/pila; ESP/LittleFS; Web/tamaño gzip. Tests de parser/correlación/timeout, largo/corto y recovery TWI; luego banco y robot. No se ejecutó ninguna de estas actividades.
