> **AFR2-20261102-14:** [dashboard y familia OLED](../contracts/OLED_DASHBOARD_STATUS_20261102.md): tarjeta informativa en reposo, dibujo por regiones, modo real, iconos y cabeceras compartidos AVR/Web. Fuente sin compilar ni probar.

> **AFR2-20261101-02:** [Arbol y gestos del C vigente](UI_SCREEN_FLOW.md), cotejados con el Simulador UI. Esta revision distingue el flujo actual del diseno e hitos historicos inferiores; corto en IR/servo no retorna, largo vuelve por contexto. Solo fuente, sin ejecucion.

> AFR2-20261030-18: selector común030044 para motores FL/FR/RL/RR, giro circular, corto confirma y largo retorna; misma raster Web. Dashboard010001: modo y accesos corto Menú/largo Rápidas. Solo fuente.

> AFR2-20261030-15: largo desde presentacion tactil detiene y sale al contexto previo, cerrando Web al rechazo de reactivacion. Wi-Fi ingreso agrega consulta local de credenciales persistidas ESP; variante privada030041, clave borrada al salir/30s. EX01v12, solo fuente.

> **AFR2-20261030-13:** etiquetas de Mecanum030009 Girar CCW/CW vinculadas AVR/Web, con las mismas identidades y recursos. Referencia física de mezcla/odometría documentada en [MOTION_REFERENCE_13](../contracts/MOTION_REFERENCE_13.md). Sin generación/build/comprobación visual o física.

> **AFR2-20261030-09:** 68 nodos en fuente, nuevas identidades030041 Datos de red/030042 Buscar redes/030043 Cambiar modo;03002d pasa a Conexión en dos páginas y030023 a Configurar. Enum de seguridad, candado propio12bytes, ayudas de gestos comunes y copia raster Web incluidos. [Contrato](../contracts/WIFI_UI_11.md). Generado anterior, memoria/paridad/hardware sin comprobar.

> **Vigente fuente07:** pruebas03003e/03003f y componente030040, capacidades6páginas, catálogo reformado y recuento dinámico. [Ficha](../changes/2026-10-30-07-ui-integral.md). Recursos generados pendientes.

> **AFR2-20261030-06:** nueva pantalla 03003d y séptima tarjeta rápida; AVR/Web en fuente, catálogo generado pendiente. [Ficha](../changes/2026-10-30-06-quick-mode.md).

> **AFR2-20261029-29:** desde dashboard, corto abre menu completo010002; largo al soltar abre pantallas rapidas010003. Ambos accesos conservan retorno al origen por historial. Sustituye el acceso comun de28. Fuente sin compilar/cargar.

> **AFR2-20261029-28:** clic corto desde dashboard abre Controles rapidos (010003); largo conserva su comportamiento al soltar. Fuente AVR y Web, sin compilar ni cargar. Sustituye la reserva anterior del corto en Inicio.

> **Vigente AFR2-20261029-27:** largo >=800ms ejecutado exclusivamente al soltar: Inicio -> Controles rápidos -> Inicio, resto atrás por contexto/historial; corto reservado en Inicio. Configuración y pruebas conserva el árbol previo bajo la sexta tarjeta. Excepción Sí/No obligatoria y cancelación Hall sin perder drafts. Radar y servo comparten editor; escala/rastro se guardan con EEPROMv5. [Contrato fuente y aceptación pendiente](../contracts/QUICK_RADAR_ACTUATORS_9.md). Las reglas anteriores que describen largo global a menú/Volver obligatorio son históricas. Sin build, validadores ni carga.

> **AFR2-20261029-26, fuente sin compilar:** Sistema → Batería0x030039 y Medición0x03003A, acción69, renderer genérico AVR/TS y Volver explícito. Sistema8ítems, lookup vinculado. Catálogo generado pendiente de ejecución autorizada; no paridad ni hardware verificados. [Contrato9](../contracts/BATTERY_MEASUREMENT_9.md).

> **AFR2-20261029-20:** Hall una vuelta conserva Iniciar/Guardar/Descartar/Volver, sin Finalizar vuelta separado. Iniciar elige rueda1FL/2FR/3RL/4RR → corto comienza → corto termina. Largo en selector/conteo cancela al menú Hall con aviso, sin borrar drafts previos. Estado muestra4CPR y pendientes. Volver con drafts pregunta Si/No sin Volver; largo no evade esa decisión. [Contrato y excepciones contextuales](../contracts/HALL_CALIBRATION_WIZARD.md). Fuente OLED/Web vinculada, sin prueba física; no flashear.

> **Actualización de compilación14:** pedido humano posterior autoriza compilar/corregir. ATmega/ESP/Web full: exit0, sin errores ni advertencias finales. [Informe](builds/2026-10-25-14/README.md). Sin tests, validadores independientes, navegador, montaje físico ni flasheos. Paquete local full generado; no promovido a ESP/data/dist. El texto de entrega sin compilar inferior es histórico.

> **AFR2-20261025-14:**53nodos/64identidades. Radar/numérica e IR gráfica/valores son hermanas con códigos propios, giro alterna y corto retorna al listado; pantalla angular con control local. Renderer entero C y TypeScript,16iconos y LUT trigonométrica copiados a biblioteca Web. Visor conectado conserva framebuffer1024bytes; metadatos por comando69 no congelan render. Fuente EX01v8 sin compilar ni comparar paridad. [Contrato8](../contracts/SENSORS_RADAR_8.md).

# Sistema de pantallas AutoFinalRobo2

AFR2-20261024-09: catálogo compartido de59entradas consumido directamente por
Web, copia TypeScript de layouts/primitivas AVR y recursos actuales generados del C.
Los builds detectan códigos sin registrar y cambios de layout pendientes de port.
Vista local de biblioteca identificada y visor remoto con framebuffer obligatorio;
retirado fallback de4pantallas antiguas. La skill requiere contraparte Web en cada
alta/modificación. Sin comparación diferencial, render de navegador ni OLED física.
[Ficha09](../changes/2026-10-24-09-linked-oled-library.md).

Vigente AFR2-20261024-08:48nodos y11tipos de aviso de red. IP/AP/Reinicio,
14XBM F4 en PROGMEM, etiquetas32bytes y scroll recortado seleccionado.
Snapshot con strings prestados estables en main; renderers sin hardware/I2C.
Captura remota no disponible mientras hay clave AP visible; clave STA excluida.
EX01v5 compilado; [contrato](../contracts/EX01_NETWORK_UI_5.md) y
[evidencia/memoria](builds/2026-10-24-08/README.md). No equivale a paridad F4
ni prueba de placas. El estado07 e inferiores conserva su evidencia histórica.

Vigente AFR2-20261024-07: catálogo ampliado con pantalla0x03002c de ejecución de
secuencias (barra segmentada, iconos/nombres PROGMEM y tiempo), snapshot en main y
renderer puro. EX01v4 compilado en las tres partes; OLED físico no probado y
secuencias mantienen gate de memoria0. [Estado por módulo](../docs/ESTADO_IMPLEMENTACION.md).
El estado02 e inferiores se conserva como historia.

Estado vigente AFR2-20261024-02: productor AVR y puente ESP/consumidor Web
compilados para EX01v3; el framebuffer ampliado ya tiene traducción y consumo.
Captura temporalmente ocupada es reintentable y no bloquea el visor para siempre.
Sin prueba física ni revisión de navegador. Los pendientes de puente del texto
histórico inferior quedan sustituidos por contracts/EX01_ALIGNMENT_3.md.

Estado19,24/10:44 nodos en catálogo PROGMEM, historial8, editores, teclado y avisos.
Backend TWI/Manager ya integrado; snapshot de presentación separado del renderer.
EX01 produce captura1024bytes/revisión/selección, retenida600ms; puente ESP pendiente.
Web debe usar framebuffer para el árbol ampliado, sin reconstruirlo con los cuatro
renderers antiguos. No se acredita paridad de íconos F4 ni pantalla física atómica.
[Implementación](../docs/ATMEGA_IMPLEMENTACION_19.md) · [Contrato](../contracts/ROBO2_PRODUCT_EXTENSION.md).
El texto inicial que sigue es histórico respecto de cantidad de vistas/backend.


[Maestro](../docs/INFORME_MAESTRO.md), [navegación](NAVIGATION.md), [catálogo](screens.json), [skill](../.agents/skills/afr2-screen-authoring/SKILL.md).

## Capas

1. Datos: snapshot pequeño y coherente; cada sensor tiene validez. Productor es adquisición/control, no pantalla.
2. Wrapper: elige vista/variante, prepara snapshot y aplica acciones semánticas. No conoce registros TWI.
3. Renderer: escribe pixels/texto en framebuffer prestado, sin GPIO/I2C, delay ni consultas a servicios.
4. OLED service: dirty pages y snapshot TX estable; inicialización/páginas asíncronas. No interpreta menú ni mueve motores.
5. I2C Manager: serializa una transferencia, deadline y callback en main. Backend AVR start/poll/abort pendiente.
6. Navegación: giro/corto/largo y opciones Volver; SW largo es menú, no atrás.

Código nuevo en Atmega/include/afr2_*.h y src/{display,services,input,ui}. La biblioteca previa completa está en Atmega/reference/f4/Core, incluida composición/menús/OSK. Los wrappers originales dependen de System/puertos y deben recortarse antes del port.

## Identidad y fidelidad

Los cuatro IDs iniciales son locales a UI; no tienen payload remoto implementado. Para cada pantalla conservar identidad, rol (vista/acción/invocador), renderer, campos/validez, acciones, navegación, variante y recursos. No asignar renderer a una acción solo por tener ID.

Home muestra el estado de base sin control activo; menú ofrece Sensores, Estado OLED y Volver; las dos vistas de diagnóstico solo tienen Volver seleccionable. Sensores muestran '--' sin dato válido. No hay GOTO/motores/Wi-Fi falsos en el prototipo. La pantalla física no se observó y no se generó un preview presentado como resultado de hardware.

Framebuffer: 128x64, bit vertical LSB por página, 1024 bytes. Snapshot TX 129 bytes permite actualizar framebuffer mientras se envía una página; cualquier redibujo marca dirty nuevamente. Puede haber mezcla temporal de páginas, no snapshot atómico de pantalla completa. La API nueva INVERT significa XOR; el F4 histórico tenía semánticas inconsistentes, por lo que no se declara paridad. Font7x10 conserva glifos y usa PROGMEM/pgm_read_byte en AVR, ASCII 32..126.

## Backend y extensión

Falta implementar TWI y conectar main/GPIO/clock. Módulo supuesto SSD1306 128x64 a dirección confirmada, no SH1106. Bloques start/poll/abort no deben bloquear ni llamar callbacks desde ISR. Reconfiguración/reinicialización solo con bus y servicio inactivos.

OSK, iconos, fuentes grandes, menús completos, canvas remoto, animaciones y réplicas Web quedan por portar mediante la skill. Los builders/SVG existentes son referencia y no acreditan igualdad pixel a pixel. Registrar capability/reporte nuevo en AFR2 antes de integrar consumidores.

## Menús y placa de interfaz

Ver [árbol objetivo](MENU_PLAN.md) y [requisitos de PCB/soporte](../hardware/PCB_OLED_ENCODER.md). La UI de producto y los conectores están especificados; su implementación y el diseño CAD siguen pendientes. Los IDs de screens.json describen únicamente las vistas iniciales en fuente.


## Estado vigente AFR2-20261023-15

Backend TWI IRQ y composición main/GPIO/Timer0 implementados en fuente; esto
sustituye la condición inicial de backend pendiente. Manager mantiene propiedad,
deadline y callback en main. OLED conserva raster/dirty pages/snapshot y PROGMEM.
Las mismas cuatro vistas se integran con encoder físico; no cambia catálogo ni
Web ni se añade UI remota. Hardware, memoria, tiempos y funcionamiento pendientes
de comprobación. No se ejecutaron builds/tests/validadores/flasheos.


## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.


## AFR2-20261030-01 — Dashboard de AutoMicro adaptado

ScreenCode010001: cabecera con Wi-Fi/STA o AP, SSID/IP con scroll y batería por voltaje válido; centro con modo real y estado de motores; pie con encoder corto → Menú010002 y encoder con barra de duración → Rápidas010003 (largo >=800ms al soltar). Sin cambios de navegación ni activación de actuadores. Iconos PROGMEM y copia raster Web vinculados; no se añade framebuffer. Batería vacía/1/2/3/4 barras por bandas0/<6/<7/<8/>=8V, no porcentaje ni autonomía; inválida/deshabilitada/caducada muestra ? y --V. Medición sigue explícita desde Sistema → Batería. Red desconocida se distingue de desconexión confirmada. Fuente sin compilar, validar, probar, promover ni cargar. [Ficha](../changes/2026-10-30-01-dashboard-automicro-icons.md).


## AFR2-20261030-02 — Cabecera de red

Dashboard010001: icono Wi-Fi/AP izquierdo, SSID arriba e IP local STA/AP debajo; batería derecha. Ventanas21..90 con scroll independiente ida/vuelta, pausa800ms en extremos y paso40ms. Solo bandas de texto se actualizan en scroll; iconos fijos. Fuente OLED/Web vinculada sin build ni carga. [Ficha](../changes/2026-10-30-02-dashboard-network-scroll.md).
