> **Loader AFR2-20261027-16 / 0.2.0:** [servidor MCP y guía Codex](AutoFinalRobo2-Loader/MCP.md), revisión de solicitudes en GUI, historial y refuerzos de interfaz/errores. [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-27-16-loader-mcp-ui.md). Solo fuente/documentación, sin compilación, ejecución, registro global, pruebas MCP/COM ni flasheo.

> **Organización AFR2-20261027-14:** [AutoFinalRobo2-Loader](AutoFinalRobo2-Loader/README.md) integrado como sexto módulo en la raíz. Código, lanzadores y manual propios; único Git global y registros compartidos en Contracts. Solo traslado/documentación; sin compilación, ejecución, tests ni flasheo.

> **Herramienta AFR2-20261027-13:** [AFR2 Loader para Windows](AutoFinalRobo2-Loader/README.md), aplicación de compilación/carga con rutas y perfiles portables, diagnóstico UART y verificación posterior. [Manual](AutoFinalRobo2-Loader/MANUAL.html). Fuente/lanzador entregados, sin compilar, ejecutar, validar ni flashear en esta ficha; firmware/Web/CAD sin intervención.

> **Build/carga AFR2-20261027-10:** [ATmega/ESP/Web compilados y LittleFS creado](AutoFinalRobo2-Contracts/documentation/builds/2026-10-27-10/README.md). Hall activo en build por pedido humano, mapa07, hall_wired/EEPROM conservados. Web esp promovida,8activos/1571104bytes gzip; littlefs.bin3121152bytes. [Guía USB-B/COM16](AutoFinalRobo2-Contracts/hardware/CARGA_USB_COM16.md). Sin apertura serie ni flasheo; montaje de jumpers/flash física sin identificar. Estados inferiores históricos.

> **Pinout AFR2-20261027-07:** [hoja completa y PDF](AutoFinalRobo2-Contracts/hardware/PINOUT.md), Hecho por Koba Chajchir. Hall agrupados A8-A11=A y A12-A15=B, orden DEL DER / DEL IZQ / TRAS DER / TRAS IZQ en ambos; fuente ATmega/máscara0x0F actualizadas. Panel OLED/encoder a 5 V reportado; CAD +3V3 sin editar. Hall deshabilitado, sin compilación/flasheo/tests/validaciones de esta revisión. [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-27-07-complete-pinout-grouped-hall.md).

> **Radar Web AFR2-20261026-02:** composición/escena rehechas con manual0.1.9 y VIS-038. [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-26-02-radar-composition.md). Fuente implementada; **sin compilación, tests, validadores, navegador ni promoción ESP**. Firmware/OLED/CAD y Modelo3D sin intervención en esta revisión. Presentación16 sustituida, controles conservados.

> **Modelo3D AFR2-20261026-01:** botón Mostrar/Ocultar referencias, cierre de fichas al ocultar y una única rueda representativa (ocho referencias en total). [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-26-01-model-reference-visibility.md). Solo fuente Web/documentación, sin tests/build/validaciones ni promoción ESP. Sustituye la cantidad de once referencias del hito15.

> **Interfaz Web AFR2-20261025-16:** radar volumétrico con sector de escaneo editable, barras y entradas numéricas vinculadas. [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-25-16-radar-volume-scan-controls.md). Fuente implementada; **sin tests/builds/validadores/navegador/promoción ESP**. Compilación14 anterior a estos cambios; ATmega/ESP/OLED/CAD sin intervención en esta revisión.

> **Fuente Web AFR2-20261025-15:** once puntos interactivos de componentes en Modelo3D, ficha fija local y accesos a las tareas/ruedas. Sin tests, compilación, validaciones, navegador ni promoción ESP; firmware/CAD sin cambios. [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-25-15-model-component-hotspots.md). El build14 inferior no incluye este cambio.

> **Compilación AFR2-20261025-14 autorizada y completada:** firmware ATmega (producto/comunicación/biblioteca), ESP y Web full compilan sin errores ni advertencias finales. Firmware0.10.0/EX01v8, Web0.3.0. [Evidencia](AutoFinalRobo2-Contracts/documentation/builds/2026-10-25-14/README.md). Sin tests, navegador ni flasheos; paquete Web local full generado, **ESP/data/dist conserva el paquete anterior**. La SRAM estática no certifica pila/ISR ni hardware. Estados previos inferiores históricos.

> **Sensores y radar autónomo AFR2-20261025-14:** [radar autónomo, servo/IR, pantallas hermanas y red](AutoFinalRobo2-Contracts/contracts/SENSORS_RADAR_8.md). Firmware nominal0.10.0/EX01v8 y Web0.3.0: fuente integrada y recursos OLED copiados, **sin compilar, probar, renderizar ni flashear**. Paquete ESP anterior; estados inferiores históricos. Modelo3D conserva pose; otras escenas siguen PWM aplicado.

> **Vigente AFR2-20261025-01:** [control directo7](AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso ni opciones de adquisición. Los tres módulos compilan con firmware0.9.0/Web0.2.0. Estado anterior inferior histórico.

> **Interfaz AFR2-20261027-05:** ancho util global ampliado; simulaciones en una tarjeta con matematica lateral plegable y controles inferiores. Inputs compactos, modos en pills y Control agrupado en Movimiento / Pruebas y calibracion / GOTO. Funciones fisicas y tactiles conservadas. [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-27-05-wide-workspaces-control-cards.md). Sin tests/build/validaciones/navegador/promocion ESP; modelo final del brazo y burbuja de referencia pendientes.

> **Interfaz AFR2-20261027-06:** calibracion con ayuda contextual derecha e interpretacion del valor; matematica destacada y uniforme en Auto/Brazo/Conjunto; pad de movimiento, boton mantenido y numero/barra compartidos con Control. [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-27-06-calibration-live-math-shared-controls.md). Solo fuente; sin tests/build/validaciones/navegador/promocion ESP. Adaptadores fisico y simulado separados.

# AutoFinalRobo2 — informe maestro vigente

> **Brazo AFR2-20261027-18:** [informe integrado de esfuerzos e iteración](AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/INFORME_ITERACION_DISENO.md). Historial v01–v06, resultados estáticos con 200 g y plan v07: selección por eje, frenada finita, cargas combinadas y prioridades base/primer eslabón. Solo documentación; nuevos motores, dinámica, refuerzos, FEA y capacidad física siguen pendientes.

> **Fuente Web AFR2-20261027-11:** [puntos y trayectorias independientes del modelo](AutoFinalRobo2-Contracts/contracts/WEB_ARM_POINTS.md).
> Posicion/orientacion respecto del montaje conservadas al cambiar dimensiones;
> biblioteca local compartida, IK en Worker, vista previa y reproduccion cancelable.
> Perfil nominal v03, sin modificar CAD v04. Esta entrega no ejecuta tests/builds,
> validadores, navegador ni promocion ESP; el paquete de10 no acredita la fuente
> final de11. Envolvente definitiva y laminas del pendiente08 sin ejecutar.

**Modelo final AFR2-20261025-13:** [PCB/soporte del usuario y conjunto Web/ESP](AutoFinalRobo2-CAD/docs/MODELO_FINAL_WEB_20261025.md). KiCad, Fusion y Blender guardados; GLB **871.756 bytes**, idéntico al descomprimido de ESP. Web compilada y siete activos promovidos, paquete 1.505.042 bytes gzip. Comprobación CAD/Draco y render Blender; sin ERC/DRC, firmware nuevo ni flasheo. Sustituye el estado CAD pendiente inferior.

**CAD AFR2-20261025-12:** [Panel OLED/encoder](AutoFinalRobo2-CAD/docs/PANEL_UI_20261025.md): fuentes KiCad 20×35 mm ruteadas en B.Cu, modelos STEP de Fusion y soporte con bisel/torretas M3×15 creados. Sin cambio de firmware/ESP/Web, ERC/DRC, interferencias ni montaje físico; JST comercial, niveles I2C y ranura M3 pendientes. Reemplaza únicamente el estado histórico de PCB/soporte pendientes.

24/10/2026, AFR2-20261024-15. Un único Git en D:/ProyectoRobotica2 con cinco
módulos. Proyecto independiente; D:/AutoProyecto es referencia de solo lectura.

La [matriz del plan maestro](AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md)
concentra implementación, productores/consumidores y pendientes reales. Se
revisaron los33comandos Web/ESP/AVR y las59identidades OLED. Se completaron
diagnóstico de LittleFS/paquete en Web y adquisición de control en Calibración.
Código presente y compilación no acreditan funcionamiento físico.

| Módulo | Alcance vigente |
| --- | --- |
| [Atmega](AutoFinalRobo2-Atmega/README.md) | C/avr-libc sin Arduino/DMA; servicios de robot y autoridad de salidas. Fuente12/build12, firmware0.8.0; Hall/memoria0 |
| [ESP](AutoFinalRobo2-ESP/README.md) | Arduino ESP8266 propio, sin AT; Wi-Fi/HTTP/WS/LittleFS y broker; diagnóstico ampliado15 |
| [Web](AutoFinalRobo2-Web/README.md) | React/Vite; control, simuladores, sensores/red, OLED, desarrollador y guía; calibración alineada15 |
| [Contracts](AutoFinalRobo2-Contracts/README.md) | Contratos, hardware, evidencias, planes y cambios compartidos |
| [CAD](AutoFinalRobo2-CAD/README.md) | Auto base Fusionv16/Blender y GLB final; PCB/pieza nueva diferidas |

Navegador → JSON afr2-ws-1 /ws del mismo origen → ESP → UART ROBO2/perfil2,
EX01v6 → USART1 Mega D18/D19. ESP UART directo y selector E_RX/E_TX son
conexiones distintas. La programación temporal por puente USB usa D0/D1 según
[FLASHING](AutoFinalRobo2-Contracts/hardware/FLASHING.md), sin trasladar mapa UNO.

OLED único dispositivo I2C por Manager. Encoder UI CLK D2/DT D3 por IRQ, SW D42
según cambio11; largo800ms abre principal, corto confirma, atrás solo Volver.
Hall de ruedas separado, A8–A15, pendiente de montaje/calibración.

No quedan brechas de implementación identificadas en el alcance software
definido tras esta revisión por fuente. **El proyecto global no está cerrado:**
Hall/CPR/signos/geometría, pila/tiempos, flash/SPI y puesta en marcha requieren
datos/pruebas; no se habilitan macros por compilar. PCB/soporte/cubierta esperan
planos y componentes. J1 cinco señales y J2 VCC/GND común,2,54mm, conservados.

[Roadmap](AutoFinalRobo2-Contracts/docs/ROADMAP.md),
[cambio15](AutoFinalRobo2-Contracts/changes/2026-10-24-15-master-alignment.md),
[evidencia15](AutoFinalRobo2-Contracts/documentation/builds/2026-10-24-15/README.md).
Pruebas, validadores independientes, navegador, flasheo y CAD no se ejecutan
por esta revisión. [Informe anterior conservado](AutoFinalRobo2-Contracts/docs/history/alignment15/INFORME_MAESTRO.md).
