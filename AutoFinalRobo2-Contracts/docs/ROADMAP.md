# Plan de implementación

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
