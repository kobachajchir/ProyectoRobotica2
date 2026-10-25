> AFR2-20261025-01: [control directo7](../../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso de acceso. Firmware0.9.0 compilado,se conservan59ScreenCodes. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](../../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **AFR2-20261024-08:** IP/AP y avisos con iconos F4, scroll seleccionado y reinicio local por Sistema → Reinicio, compilados. [EX01v5](../../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md) · [Estado real](../../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). En ATmega, FillRect/TextWindow, iconos y snapshot de red quedan separados del transporte/I2C. Sin prueba de placa ni flasheo; estados inferiores históricos.

# Entrega ATmega EX01 para continuar ESP

**Vigente AFR2-20261024-07:** el complemento ESP/Web está implementado y
compilado con EX01v4; info/boot/reinicio y etiquetas de segmento añadidos.
Consultar [formato actual](../../AutoFinalRobo2-Contracts/contracts/EX01_SYSTEM_SEQUENCE_4.md)
y [estado real](../../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md).
Mantiene framing/perfil2. No se probaron placas; las limitaciones de memoria y
calibración siguen activas. La entrega19 inferior es histórica.

AFR2-20261023-19,24/10/2026. Guía de continuación; no cambia fuente ESP ni duplica
el contrato normativo. ATmega tiene endpoint escrito sin compilar/probar. ESP/Web
deben implementar el complemento antes de anunciarlo; build20/21 no lo acredita.

Leer [ROBO2_PRODUCT_EXTENSION.md](../../AutoFinalRobo2-Contracts/contracts/ROBO2_PRODUCT_EXTENSION.md)
completo: cuerpos y offsets, unidades, errores, validación en recepción/respuesta,
sesión/token/lease, captura OLED, pose y outbox Wi-Fi. Usar
[afr2_product_wire.h](../../AutoFinalRobo2-Contracts/contracts/afr2_product_wire.h) y
[CONFIG IDs](../../AutoFinalRobo2-Contracts/contracts/ATMEGA_CONFIG_ACTIONS.json).

Orden: mantener hello01/perfil2 → negociar10/EX01 → codecs/resultados y revocación
→ estado/lease/STOP/servicios → pose y captura/framebuffer → polling outbox/credenciales
→ modal Web solicitado por el auto y único WS. No iniciar conexiones por simple ACK;
la contraseña Web queda en RAM ESP y el auto confirma Conectar con clic físico.

No reutilizar el diseño perfil3/secuencias de21 para interpretar estos IDs:10 aquí
es EX_HELLO y los comandos10..15 tienen otra semántica en perfil3. Secuencias sigue
pendiente en ambos firmwares. La migración/coexistencia requiere acuerdo explícito.
Los consumidores Web, mapas y menús deben usar datos/capacidades/validez reales.
No modificar afr2_wire.h o anunciar capacidades como atajo de compatibilidad.

[Matriz y pendientes de aceptación](../../AutoFinalRobo2-Contracts/docs/PLAN_ATMEGA.md).
Sin tests, build, flasheo ni ejecución de hardware en esta entrega.
