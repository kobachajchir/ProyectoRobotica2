# AutoFinalRobo2 - informe maestro

Hito CAD AFR2-20261024-01: [ruedas y Blender](AutoFinalRobo2-CAD/docs/RUEDAS_Y_BLENDER_20261024.md). Fusion v16, casquillos de referencia autorizados, fotos/renders y GLB 827.232 bytes. Atmega/ESP/Web sin cambios. Estados anteriores históricos.

Actualización CAD AFR2-20261023-07: [ensamble parcial en Fusion, paso a paso](AutoFinalRobo2-CAD/docs/ARMADO_FUSION_PASO_A_PASO.md), versión 15 guardada con descripciones y exportada como F3D. Chasis del usuario conservado; CADS aportados incorporados y Model Y esencial reconstruido. Ruedas correctas de 80 mm/acoples pendientes, sin generar ruedas. Atmega/ESP/Web sin cambios; sin pruebas ni validadores. Los estados de entregas anteriores de abajo conservan su contexto histórico.

Fecha: 23 de octubre de 2026. Base de trabajo y migración; no firmware completo ni puesta en marcha. Proyecto independiente: las fuentes originales y su registro AMI no se modificaron.

## Punto de entrada y cinco módulos en un Git

Continuar desde [AutoFinalRobo2-Contracts](AutoFinalRobo2-Contracts/README.md). Allí están decisiones, pinout, contratos, pendientes y trazabilidad.

| Módulo | Responsabilidad | Código e informe |
| --- | --- | --- |
| AutoFinalRobo2-Atmega | ATmega2560 C/avr-libc, cerebro y autoridad de movimiento | [Informe](AutoFinalRobo2-Atmega/INFORME_ADAPTACION.md), reference/f4/Core, src, include |
| AutoFinalRobo2-ESP | ESP12S Arduino ESP8266, Wi-Fi/HTTP/WebSocket, LittleFS | [Informe](AutoFinalRobo2-ESP/INFORME_ADAPTACION.md), reference/esp01, src, data |
| AutoFinalRobo2-Web | React + Vite, control, GOTO, pruebas, sensores y Wi-Fi | [Informe](AutoFinalRobo2-Web/INFORME_ADAPTACION.md), reference/web, src |
| AutoFinalRobo2-Contracts | Contratos, hardware, pantallas y coordinación | [Inicio](AutoFinalRobo2-Contracts/README.md), contracts, hardware, documentation, changes |
| AutoFinalRobo2-CAD | KiCad, pieza nueva y modelo completo del auto | [Inicio](AutoFinalRobo2-CAD/README.md), kicad, mecanica, reference, docs |

Un único Git en D:/ProyectoRobotica2 administra cinco módulos, incluido AutoFinalRobo2-CAD. Origin privado: [kobachajchir/ProyectoRobotica2](https://github.com/kobachajchir/ProyectoRobotica2), rama remota main. Los cuatro historiales anteriores se conservan como ancestros y etiquetas; los metadatos originales están resguardados dentro de .git/legacy-repositories. Ver [organización y exclusiones](AutoFinalRobo2-Contracts/docs/ESTRUCTURA_Y_GIT.md). No se crean dependencias Git ni escrituras hacia AutoMicro. [Estado de entrega](AutoFinalRobo2-Contracts/provenance/ENTREGA.md).

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

La skill [afr2-screen-authoring](AutoFinalRobo2-Contracts/.agents/skills/afr2-screen-authoring/SKILL.md) guía nuevas pantallas y adaptaciones desde Contracts.

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

El [plan de recursos](AutoFinalRobo2-Contracts/hardware/RECURSOS_Y_TIEMPOS.md) contempla SRAM, interrupciones, PWM, UART, atomicidad y PROGMEM. Un framebuffer 128×64 ocupa 1024 bytes. Fuentes y tablas const del F4 no deben copiarse a SRAM AVR sin revisar su almacenamiento.

El [pinout ampliado](AutoFinalRobo2-Contracts/hardware/PINOUT.md) propone SDA D20/SCL D21 y CLK D40/DT D41/SW D42. No afirma que ya estén conectados. VCC y GND externos serán comunes; identificar la tensión compatible con ambos módulos y sus pull-ups antes de cerrar el esquema.

El [shield documentado](AutoFinalRobo2-Contracts/hardware/WIFI_SHIELD.md) incorpora regulación/traducción declaradas y distingue selector E_RX/E_TX de puerto directo ESP de 3,3 V. Operación por D18/D19, sin jumpers a D4/D5 ni D0/D1. Para [carga con USB Mega](AutoFinalRobo2-Contracts/hardware/FLASHING.md), D0→RX ESP y TX ESP→D1 requieren cruce físico y niveles correctos: reset no conecta USB con USART1 ni hace válido el emparejamiento normal del selector. Flash nominal 4 MiB según ficha; chip real, SPI y LittleFS siguen pendientes.

## Continuación

1. Cerrar hardware y recursos en Contracts.
2. Implementar reloj/UART/TWI e integrar OLED/encoder.
3. Cerrar contrato mínimo, enlazar Web/ESP/Mega con diagnóstico y STOP.
4. Añadir sensores, motores, Hall, odometría, línea y GOTO.
5. Portar pantallas con la skill y verificar cuando se autorice.

[Plan detallado](AutoFinalRobo2-Contracts/docs/ROADMAP.md). No se ejecutaron builds, tests, validadores, flasheos ni pruebas físicas por instrucción del usuario. Copias e informes no acreditan compatibilidad.

## Continuación de interfaz física y navegación

El shield expone SDA/SCL. [PCB OLED/encoder](AutoFinalRobo2-Contracts/hardware/PCB_OLED_ENCODER.md): conectores 1x5 señales y 1x2 VCC/GND comunes, paso 2,54 mm; CAD diferido hasta recibir planos. [Menús objetivo](AutoFinalRobo2-Contracts/documentation/MENU_PLAN.md): diseño pendiente que conserva largo SW para menú y atrás solo por ítem.

Actualización AFR2-20261023-03: [informe del shield](AutoFinalRobo2-Contracts/hardware/WIFI_SHIELD.md) y [prompt actualizado](AutoFinalRobo2-Contracts/docs/PROMPT_CONTINUIDAD.md). Evidencia adicional con procedencia propia; código de los tres consumidores sin cambio.

AFR2-20261023-04: AutoFinalRobo2-CAD y Git único en la raíz. Los nueve insumos de CADS se trasladan con inventario; no se modifica su contenido ni se crea diseño nuevo. Fuentes de firmware/Web y documentos originales preservados. [Registro](AutoFinalRobo2-Contracts/changes/2026-10-23-04-cad-and-monorepo.md).

AFR2-20261023-05: [plan 3D en Fusion](AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md), inventario/CAD de Internet y ensamble sin cables. Pieza nueva/soporte/PCB pendientes, espacio reservado, OLED 0,96 pulgadas y encoder KY-040 sin breakout/perilla como referencias. Origin privado ProyectoRobotica2 y main para continuar en otra PC; esta entrega no inicia el modelo.
