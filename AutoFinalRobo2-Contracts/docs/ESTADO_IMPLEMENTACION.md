> **AFR2-20261030-07:** ATmega/ESP/Web adaptados en fuente EX01v10/EEPROMv8; sin compilar/probar, recursos sin regenerar, paquete ESP y hardware conservan entregas previas. [Ficha](../changes/2026-10-30-07-ui-integral.md).

> **Vigente AFR2-20261029-27:** largo >=800ms ejecutado exclusivamente al soltar: Inicio -> Controles rápidos -> Inicio, resto atrás por contexto/historial; corto reservado en Inicio. Configuración y pruebas conserva el árbol previo bajo la sexta tarjeta. Excepción Sí/No obligatoria y cancelación Hall sin perder drafts. Radar y servo comparten editor; escala/rastro se guardan con EEPROMv5. [Contrato fuente y aceptación pendiente](../contracts/QUICK_RADAR_ACTUATORS_9.md). Las reglas anteriores que describen largo global a menú/Volver obligatorio son históricas. Sin build, validadores ni carga.

> **AFR2-20261029-26, solo fuente:** batería A5 con adquisición explícita y ADC compartido IR, EX01v9/0x55, Sistema → Batería0x030039/0x03003A y copia Web; pill/icono por voltaje y confirmaciones de reinicio. Referencia5000mV nominal sin calibrar. [Contrato](../contracts/BATTERY_MEASUREMENT_9.md). Sin build/generador/navegador/tests/validadores/promoción/flash/hardware; memoria/paridad pendientes.

> **AFR2-20261029-19 compilado y ATmega cargado:** pedido humano29/10/2026. ATmega/ESP/Web esp y LittleFS completados; promoción local Web a ESP/data/dist. Carga COM8 final13:32:50,80066bytes escritos/verificados, avrdude exit0. ESP/LittleFS sin cargar. SRAM estática7085bytes; sin prueba física/dinámica. Evidencia en AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-19/README.md. Estados inferiores sin build/carga son históricos.

> **Loader AFR2-20261028-05 / 0.9.0:** casilla de backup real opcional, aviso configurable y menú Flashear Firmware ATmega / Flashear último backup. [Ficha](../changes/2026-10-28-05-loader-mega-backups.md) · [Plan A/B](../hardware/PLAN_PRUEBA_FLASHER_MEGA.md). Fuente/documentación, sin compilar/ejecutar, tests, validaciones, COM ni cargas. atmega/esp/web sin nuevo estado de firmware por05; aceptación de lectura flash/hfuse, GUI y hardware pendiente.

> **Loader AFR2-20261028-03 / 0.8.0:** instalar/restaurar flasher Mega y selector de método ESP con checklist. [Plan A/B](../hardware/PLAN_PRUEBA_FLASHER_MEGA.md), [ficha](../changes/2026-10-28-03-loader-mega-flasher.md). Fuente/documentación, sin compilar/ejecutar/validar ni COM/cargas; servicio atmega de02, firmware de producto esp/web sin cambios.

> **Loader AFR2-20261027-19 / 0.3.0:** [monitor serie y errores](../../AutoFinalRobo2-Loader/MANUAL.html#monitor), COM abrir/cerrar, baudios, captura RX, detección de arranque y etapas de escritura/verificación; consulta MCP de captura y logs de fallos. [Ficha](../changes/2026-10-27-19-loader-serial-monitor.md). Solo fuente/documentación; sin compilar, ejecutar, validar ni abrir COM/flashear. atmega/esp/web/CAD sin cambios por esta ficha.

> **Loader AFR2-20261027-16 / 0.2.0:** [MCP y revisión GUI](../../AutoFinalRobo2-Loader/MCP.md), historial, filtro de configuración y evidencias UART/ROM. [Ficha](../changes/2026-10-27-16-loader-mcp-ui.md). Solo fuente/documentación; no compilado, ejecutado, registrado ni conectado al cliente en esta entrega; sin pruebas COM ni flasheos. Atmega/ESP/Web/CAD sin cambios por esta ficha.

> **Web26-02 / VIS-038:** nueva composición del radar, encuadre/vistas y parámetros separados de la columna de medición. [Ficha](../changes/2026-10-26-02-radar-composition.md). Solo fuente, visual **No verificada**; sin tests/build/validadores/navegador/promoción ESP. ATmega/ESP/OLED sin cambios; compilación14 anterior a esta interfaz.

> **Web16 / VIS-037:** fuente de radar volumétrico y parámetros con barras/números integrada. [Ficha](../changes/2026-10-25-16-radar-volume-scan-controls.md). Sin compilación, tests, validadores, navegador ni promoción. Firmware ATmega/ESP y OLED sin modificación; evidencia14 anterior a esta interfaz. Cambios15/16 de fuente Web no quedan cubiertos por ese build.

> **Compilación AFR2-20261025-14 autorizada y completada:** firmware ATmega (producto/comunicación/biblioteca), ESP y Web full compilan sin errores ni advertencias finales. Firmware0.10.0/EX01v8, Web0.3.0. [Evidencia](../documentation/builds/2026-10-25-14/README.md). Sin tests, navegador ni flasheos; paquete Web local full generado, **ESP/data/dist conserva el paquete anterior**. La SRAM estática no certifica pila/ISR ni hardware. Estados previos inferiores históricos.

> **Fuente vigente AFR2-20261025-14:** [radar autónomo, servo/IR, pantallas hermanas y red](../contracts/SENSORS_RADAR_8.md). Firmware nominal0.10.0/EX01v8 y Web0.3.0: fuente integrada y recursos OLED copiados, **sin compilar, probar, renderizar ni flashear**. Paquete ESP anterior; estados inferiores históricos. Modelo3D conserva pose; otras escenas siguen PWM aplicado.

> **Actualización AFR2-20261029-10:** usuario confirma OLED/encoder y
> orientaciones del diagnóstico09. Configuración trasladada al producto en
> fuente (pulls1, espera500ms, OLED180, encoder invertido, refresco1s y migración
> UI EEPROMv2 que conserva calibraciones). Sin build, carga o prueba del
> producto integrado por el agente. Web solo fingerprint revisado; ESP sin
> cambios. [Detalle](../changes/2026-10-29-10-product-oled-encoder-verified-setup.md).

# Estado vigente — AFR2-20261025-11

25/10/2026. [Control directo7](../contracts/DIRECT_CONTROL_7.md),
[matriz](ALINEACION_MAESTRO.md), [Web11](../documentation/builds/2026-10-25-11/README.md) y [firmwares01](../documentation/builds/2026-10-25-01/README.md).

## Web

[Control táctil](../contracts/WEB_TOUCH_SIMULATION.md): modo local sin robot,
vista superior, dirección y flechas compartidas. Giro ideal de ruedas en
táctil/GOTO/secuencias, sin feedback físico. Reconectar no promueve sesión local.
Captura DOM sobre Canvas, foco inicial/teclado modal y flecha activa animada03.
Corregido falso fallo de Canvas.fallback; entrada local independiente de carga3D.
Chevrones09 conservados; modelo manual10 mayor por cámara60% frente52%.
Giro11 con dos dedos/Q-E local, rumbo persistente y fase de ruedas; sin yaw físico.
Web compilada/promovida11; nuevo gesto/captura sin comprobar en teléfono.

Web0.2.0 envía acciones directas: no permiso, adquisición, renovación o liberación.
Se elimina controlLease y la preparación de control en movimiento/modos/pruebas/
calibración/secuencias. Estado/STOP/visibilidad/soltar siguen supervisados.
Presencia interactiva cada500ms; no activa movimiento. Paquete compilado/promovido.

## ESP

Activos Web11 promovidos a data/dist; sin recompilar firmware en esta entrega.

Firmware0.9.0, controlMode=direct y negociación exacta EX01v7. Se elimina toda
concesión/token de acceso. WEB_PULSE0x12 retransmite edad de presencia a AVR cada500ms.
Sesión/transacción/generación/programas siguen correlacionados; desconexión pide STOP.

## ATmega

Firmware0.9.0 compilado: no Acquire, tokens de acceso o bandera de habilitación.
Acciones directas con límites/calibración/estado ocupado; operación Web caduca a1500ms.
Retiradas opciones OLED2/3/56; Control manual4ítems/Sistema7.59ScreenCodes/copia Web
conservados. Hall y memoria0; no habilita funciones físicas sin sus requisitos.

## Pendientes

Sin tests, validadores independientes, navegador, flasheo o pruebas físicas.
Calibración/cableado/Hall, memoria dinámica, flash/SPI y planos CAD siguen pendientes.
Copias anteriores en history/alignment15 y history/direct-control7 son históricas.

## AFR2-20261025-15 — Modelo 3D con componentes interactivos

Fuente Web nueva: once puntos con fichas locales de explicación y acceso a la
tarea/rueda correspondiente. La cámara mueve los puntos y la ficha queda fija;
arrastrar/zoom la conserva, X/Escape/clic exterior la cierra. Identidad0.1.9,
VIS-036. Referencias visuales inferidas del GLB, sin modificar CAD.

Estado de esta entrega: atmega sin cambios; esp sin cambios y sin promoción a
data/dist; web implementada en fuente, sin compilar ni empaquetar. El build14
previo no incluye esta interfaz. Sin tests, compilaciones, validaciones,
navegador, render ni flasheo. Documentación y límites en
[contrato del visor](../contracts/WEB_MODEL_INSPECTION.md) y
[ficha15](../changes/2026-10-25-15-model-component-hotspots.md).


## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.
