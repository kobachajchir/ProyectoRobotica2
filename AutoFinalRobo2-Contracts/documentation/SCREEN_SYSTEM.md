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
