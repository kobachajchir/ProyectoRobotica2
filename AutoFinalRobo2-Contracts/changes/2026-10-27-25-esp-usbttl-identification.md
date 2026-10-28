# AFR2-20261027-25 — ESP identificado por USB–TTL, flash de 4 MiB

27/10/2026. Continuación del diagnóstico ESP solicitado por el usuario. Se
registran sus ejecuciones exitosas de `chip_id` y `flash_id` con adaptador
USB–TTL en COM5, esptool 4.5.1 a 9600, sin stub ni reset automático.

[Informe y salidas aportadas](../hardware/diagnostics/20261027-com5-usbttl-user-report/report.md),
[procedencia por archivo](../provenance/esp-usbttl-diagnosis-20261027.json).

Antes: capacidad nominal/candidata de 4 MiB y consultas por COM8 sin sincronizar.
Ahora: ESP8266EX, chip `0x00c2218e`, cristal informado 26 MHz; flash ID fabricante
`0x5e`, dispositivo `0x4016`, capacidad informada `4MB` (4 MiB). Es evidencia
positiva aportada por el usuario, no una repetición ejecutada por el agente
para registrarla. No se inventan hora ni exit code numérico de estas consultas.

No hay cambios de ROBO2/EX01, ScreenCodes, protocolo de producto, firmware,
layout, perfiles ni fuente del Loader. El retorno USB del Mega informado como
correcto y el éxito directo no localizan la causa del fallo de la ruta COM8.
El usuario aclaró después que usó exactamente los mismos dos pines del shield
en ambas pruebas; el éxito no acredita un cambio o bypass del selector. Se
conserva esta aclaración en el informe y queda pendiente identificar la cabecera
física y comparar niveles/carga o manejo de puerto/reset.

| Módulo | Estado de esta ficha |
| --- | --- |
| esp | Identificación de chip/flash por consulta externa aportada; sin nueva carga ni verificación de contenido. Firmware sin cambios. |
| atmega | Sin cambios, compilación ni carga; retorno D0–D1 informado por usuario, no aceptación del firmware. |
| web | Sin cambios, build ni promoción. |
| Loader | Sin cambios de fuente/perfil ni ejecución nueva de esta ficha. |
| Contracts | Evidencia y referencias actualizadas, conservando estados históricos. |
| CAD | Sin cambios. |

Modo/frecuencia SPI, layout de la imagen instalada, programación/verificación
de nuestras imágenes y funcionamiento del robot siguen pendientes. No hay tests,
builds, validadores ni apertura nueva de puertos por el agente para este registro.
Las salidas aportadas son evidencia; no autorización para escribir o borrar flash.
