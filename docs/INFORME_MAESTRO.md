# AutoFinalRobo2 - informe maestro

Fecha: 23 de octubre de 2026. Base de trabajo y migración; no firmware completo ni puesta en marcha. Proyecto independiente: las fuentes originales y su registro AMI no se modificaron.

## Punto de entrada y cuatro repositorios

Continuar desde [AutoFinalRobo2-Contracts](../../AutoFinalRobo2-Contracts/README.md). Allí están decisiones, pinout, contratos, pendientes y trazabilidad.

| Repositorio | Responsabilidad | Código e informe |
| --- | --- | --- |
| AutoFinalRobo2-Atmega | ATmega2560 C/avr-libc, cerebro y autoridad de movimiento | [Informe](../../AutoFinalRobo2-Atmega/INFORME_ADAPTACION.md), reference/f4/Core, src, include |
| AutoFinalRobo2-ESP | ESP12S Arduino ESP8266, Wi-Fi/HTTP/WebSocket, LittleFS | [Informe](../../AutoFinalRobo2-ESP/INFORME_ADAPTACION.md), reference/esp01, src, data |
| AutoFinalRobo2-Web | React + Vite, control, GOTO, pruebas, sensores y Wi-Fi | [Informe](../../AutoFinalRobo2-Web/INFORME_ADAPTACION.md), reference/web, src |
| AutoFinalRobo2-Contracts | Contratos, hardware, pantallas y coordinación | [Inicio](../../AutoFinalRobo2-Contracts/README.md), contracts, hardware, documentation, changes |

Cuatro Git independientes, sin remotos. La raíz es contenedor, no un quinto repo. No se crean dependencias Git ni escrituras hacia AutoMicro. [Estado de entrega](../../AutoFinalRobo2-Contracts/provenance/ENTREGA.md).

## Arquitectura acordada

Navegador React → JSON WebSocket /ws → ESP12S → UNER candidato por UART → ATmega2560. El Mega posee movimiento, sensores, odometría, seguridad y UI; ESP posee radio, archivos Web, traducción y correlación.

- ATmega C sin Arduino ni DMA; interrupciones breves y máquinas de estados.
- ESP con firmware propio Arduino y LittleFS, sin comandos AT ni WiFiEsp.
- Cuatro motores Mecanum, cinco canales de línea, HC-SR04, OLED I2C y encoder de interfaz añadido. Sin MPU ni control de balance de dos ruedas.
- I2C Manager como acceso único al bus; OLED es su único cliente.
- Giro selecciona/ajusta, clic corto confirma, largo de SW abre menú; atrás solo por opción explícita de submenú. No existe botón menú separado.

## Entrega y procedencia

Se copiaron fuentes propias F4/ESP/Web, recursos y corpus documental de Contracts a reference/. Los ocho adjuntos están en Contracts/hardware/supplied; Documentacion/ original queda intacta. Extracciones PDF y figuras consultadas quedan como evidencia auxiliar.

Hay informes junto al código y guías por subsistema. provenance/files.json registra hashes de origen/copia y transformaciones; origins.json distingue HEAD y checkout modificado. F4 tenía modificaciones locales previas: la copia no se presenta como un HEAD limpio. No se copiaron .git, dependencias instaladas, compilados ni preferencias Wi-Fi persistidas. Los defaults de credenciales identificados se vaciaron en las copias.

La base fuente nueva incluye dibujo OLED, fuente 7x10 extraída del F4 en PROGMEM para AVR, envío por páginas con I2C Manager asíncrono, wrappers y eventos de encoder. Es parcial: faltan backend TWI AVR y composición real. ESP y Web incluyen esqueletos independientes que todavía no controlan el vehículo.

La skill [afr2-screen-authoring](../../AutoFinalRobo2-Contracts/.agents/skills/afr2-screen-authoring/SKILL.md) guía nuevas pantallas y adaptaciones desde Contracts.

## Reformas necesarias

| Área | Evidencia de origen | Reforma |
| --- | --- | --- |
| UART/UNER | Parser separado; transporte F4 DMA | RX/TX por IRQ USART1, buffers estables, parser/dispatch en main, colas y timeout |
| I2C/OLED | Define DMA existente; rama sin DMA bloquea | Backend TWI asíncrono, deadlines/NACK/STOP/recovery; no basta cambiar el define |
| Pantallas | Wrappers, renderers, menús, OSK, permisos | Snapshots explícitos, navegación sin botón menú; quitar MPU/balance; portar por prioridad |
| Encoder UI | encoder.c es mando de usuario | Adaptar CLK/DT/SW, largo exclusivo, Volver por ítem; no confundir con encoders de rueda |
| Línea | Modelo F4 de ocho muestras y emisores | Cinco entradas OSOYOO A4..A0; confirmar tipo/polaridad; retirar tres IR de objetos |
| HC-SR04 | Driver equivalente no localizado | TRIG/espera subida/espera bajada/timeout; D31 exige decisión de captura |
| Motores | F4 diferencial de dos motores | Cuatro PWM, signos, rampas, inversión y cinemática Mecanum |
| GOTO | Requiere MPU, balance y yaw | Nuevo planificador relativo por odometría; no simular MPU listo; deshabilitado hasta posición válida |
| Hall por rueda | Sin cableado en manual; 11 PPR es hipótesis | Elegir pines/decodificación y comprobar carga IRQ, resolución/signos/niveles |
| ESP/Web | Catálogo stm.*, balance, MPU | Capabilities, nuevos codecs de cuatro ruedas/ultrasonido y propietario robot |

## Límites y conexión

El [plan de recursos](../../AutoFinalRobo2-Contracts/hardware/RECURSOS_Y_TIEMPOS.md) contempla SRAM, interrupciones, PWM, UART, atomicidad y PROGMEM. Un framebuffer 128×64 ocupa 1024 bytes. Fuentes y tablas const del F4 no deben copiarse a SRAM AVR sin revisar su almacenamiento.

El [pinout ampliado](../../AutoFinalRobo2-Contracts/hardware/PINOUT.md) propone SDA D20/SCL D21 y CLK D40/DT D41/SW D42. No afirma que ya estén conectados. VCC y GND externos serán comunes; identificar la tensión compatible con ambos módulos y sus pull-ups antes de cerrar el esquema.

El [shield documentado](../hardware/WIFI_SHIELD.md) incorpora regulación/traducción declaradas y distingue selector E_RX/E_TX de puerto directo ESP de 3,3 V. Operación por D18/D19, sin jumpers a D4/D5 ni D0/D1. Para [carga con USB Mega](../hardware/FLASHING.md), D0→RX ESP y TX ESP→D1 requieren cruce físico y niveles correctos: reset no conecta USB con USART1 ni hace válido el emparejamiento normal del selector. Flash nominal 4 MiB según ficha; chip real, SPI y LittleFS siguen pendientes.

## Continuación

1. Cerrar hardware y recursos en Contracts.
2. Implementar reloj/UART/TWI e integrar OLED/encoder.
3. Cerrar contrato mínimo, enlazar Web/ESP/Mega con diagnóstico y STOP.
4. Añadir sensores, motores, Hall, odometría, línea y GOTO.
5. Portar pantallas con la skill y verificar cuando se autorice.

[Plan detallado](../../AutoFinalRobo2-Contracts/docs/ROADMAP.md). No se ejecutaron builds, tests, validadores, flasheos ni pruebas físicas por instrucción del usuario. Copias e informes no acreditan compatibilidad.

## Actualización de contexto: interfaz física y menús

El usuario confirma SDA/SCL disponibles con esos rótulos en el shield. La nueva PCB usa un conector 1x5 para CLK/DT/SW/SDA/SCL y otro 1x2 para VCC/GND comunes, paso 2,54 mm; VCC numérica por identificar. La especificación vigente está en [PCB OLED/encoder](../hardware/PCB_OLED_ENCODER.md), y el [árbol de menús](../documentation/MENU_PLAN.md) amplía el diseño futuro sin cambiar el prototipo. KiCad/soporte3D se abordarán al recibir planos; fijación con dos tornillos y tuercas.

Actualización AFR2-20261023-03: [evidencia Wi-Fi](../hardware/wifi-shield/INFORME_CARPETA.md) incorpora tres imágenes aportadas, ficha actual y Fritzing; [prompt de continuidad](PROMPT_CONTINUIDAD.md) consolida arquitectura, navegación y conexiones. El mapa UNO del proveedor no cambia SDA/SCL Mega D20/D21. Código de atmega/esp/web sin cambios.
