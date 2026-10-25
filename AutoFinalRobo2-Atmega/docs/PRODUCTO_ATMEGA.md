> AFR2-20261025-01: [control directo7](../../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso de acceso. Firmware0.9.0 compilado,se conservan59ScreenCodes. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](../../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](../../AutoFinalRobo2-Contracts/changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> **AFR2-20261024-08:** IP/AP y avisos con iconos F4, scroll seleccionado y reinicio local por Sistema → Reinicio, compilados. [EX01v5](../../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md) · [Estado real](../../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). APIs nuevas FillRect/TextWindow, iconos y snapshot de red separados del transporte/I2C. Sin prueba de placa ni flasheo; estados inferiores históricos.

# Producto ATmega: entrega19

**Estado vigente AFR2-20261024-07:** firmware/comunicación/biblioteca compilados,
pantalla de secuencias0x03002c y servicios info/boot/reinicio EX01v4 integrados.
Memoria/Hall0 y calibración siguen pendientes, sin prueba de placa ni flasheo.
[Estado actual](../../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md#atmega)
y [contrato v4](../../AutoFinalRobo2-Contracts/contracts/EX01_SYSTEM_SEQUENCE_4.md).
La entrega19 que sigue describe su evidencia histórica, anterior a esos builds.

24/10/2026. Fuente C/avr-libc, sin Arduino/DMA; OLED único cliente I2C Manager.
No se ejecutaron tests, compiladores, validadores, puertos, flasheos ni hardware.

- [Implementación, archivos, pines/timers, defaults, memoria y límites](../../AutoFinalRobo2-Contracts/docs/ATMEGA_IMPLEMENTACION_19.md).
- [Matriz M01..M19 y continuación](../../AutoFinalRobo2-Contracts/docs/PLAN_ATMEGA.md).
- [EX01: qué recibe/espera/valida cada extremo](../../AutoFinalRobo2-Contracts/contracts/ROBO2_PRODUCT_EXTENSION.md).
- [CONFIG: IDs explícitos/rangos/unidades](../../AutoFinalRobo2-Contracts/contracts/ATMEGA_CONFIG_ACTIONS.json).
- [Procedencia por archivo](../../AutoFinalRobo2-Contracts/provenance/atmega-product.json).

main integra reloj/UART1/TWI/OLED/encoder, App/Product y sensores/potencia/EEPROM.
communication-firmware conserva UART sola; no incluye producto/EX01. Makefile no
se ejecutó. libm necesaria para odometría/GOTO en main, nunca desde ISR.

Defaults: salidas apagadas, remoto deshabilitado, motores sin habilitar, Hall sin
conectar y macro AFR2_HALL_CONNECTED=0, CPR0, tipo/calibración línea0, geometría sin
confirmar. Motor individual es una prueba PWM acotada que no acredita velocidad;
marchas cerradas se rechazan hasta disponer de requisitos reales/calibrados.

Hall propone A8..A15 x1 PCINT2. Antes de habilitar macro/cableado confirmar niveles
y pull-ups. Test local permite girar manualmente UNA vuelta hacia adelante y
revisar/guardar CPR/signo por rueda. No usa11/75:1 como calibración. El test local
de medición no vence por el lease local de5s; el remoto exige renovar su lease.

Giro selecciona, clic confirma, largo abre principal; Volver es ítem explícito.
Excepción del usuario: largo en teclado cancela y vuelve sin enviar datos.
Teclado y modal Web preparan borrador: Conectar requiere clic físico en revisión
OLED. El flujo Web solicitado por el auto aún necesita puente/modal ESP/Web.
Capturas UI remotas no sirven para escribir preferencias/calibrar/tomar control.

No confundir EX01/perfil2 con perfil3/secuencias diseñado en la ficha21. El firmware
actual no anuncia ni ejecuta secuencias. Build/mapas/IRQ/calibración y aceptación
física quedan pendientes de pedido expreso, no equivalen a implementación fuente.
