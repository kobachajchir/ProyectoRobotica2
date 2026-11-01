> **AFR2-20261101-02:** [Arbol y gestos del C vigente](UI_SCREEN_FLOW.md), cotejados con el Simulador UI. Esta revision distingue el flujo actual del diseno e hitos historicos inferiores; corto en IR/servo no retorna, largo vuelve por contexto. Solo fuente, sin ejecucion.

> AFR2-20261030-18: selector común030044 para motores FL/FR/RL/RR, giro circular, corto confirma y largo retorna; misma raster Web. Dashboard010001: modo y accesos corto Menú/largo Rápidas. Solo fuente.

> AFR2-20261030-15: largo desde presentacion tactil detiene y sale al contexto previo, cerrando Web al rechazo de reactivacion. Wi-Fi ingreso agrega consulta local de credenciales persistidas ESP; variante privada030041, clave borrada al salir/30s. EX01v12, solo fuente.

> **Movimiento13:** Mecanum030009 conserva filas/acciones; arg8 Girar CCW, arg9 Girar CW, vistos desde arriba. Copia raster Web vinculada; no cambia retorno/gestos. [Referencia de movimiento](../contracts/MOTION_REFERENCE_13.md). Solo fuente.

> **Vigente Wi-Fi09:** Wi-Fi → Estado de red / Cambiar modo / Buscar redes / AP-fallback / Volver. Estado contiene Conexión, Datos y Configurar; Buscar contiene Redes encontradas/Refrescar/Volver. Aviso Redes listas abre el listado al cerrarse; falta de cliente Web vuelve al selector. Conectar/Desconectar y Reintentar dependen del estado actual; corto/largo usan iconografía y largo vuelve al contexto. [Contrato11](../contracts/WIFI_UI_11.md). Fuente sin generación/build/pruebas.

> **Vigente fuente07:** largo configurable800..3000ms al soltar; prueba encoder tiene primer largo de detección y siguiente de salida; Sí/No obligatorio; filas dinámicas invalidan captura. [Contrato](../contracts/UI_REFORM_10.md). Sin ejecución.

> **AFR2-20261030-06:** selector rápido 03003d: Reposo/Manual, clic físico y aviso de resultado; retorno contextual. [Ficha](../changes/2026-10-30-06-quick-mode.md). Fuente sin compilar ni probar.

> **AFR2-20261030-05 parcial:** menú y Sistema reorganizados; Capacidades tiene cuatro páginas, carrusel y hermanas de sensores usan indicadores direccionales y giro limitado. Selección rápida de modos pendiente. Fuente sin compilar ni probar. [Ficha](../changes/2026-10-30-05-oled-ajustes-parciales.md).

> **AFR2-20261029-29:** desde dashboard, corto abre menu completo010002; largo al soltar abre pantallas rapidas010003. Ambos accesos conservan retorno al origen por historial. Sustituye el acceso comun de28. Fuente sin compilar/cargar.

> **AFR2-20261029-28:** clic corto desde dashboard abre Controles rapidos (010003); largo conserva su comportamiento al soltar. Fuente AVR y Web, sin compilar ni cargar. Sustituye la reserva anterior del corto en Inicio.

> **Vigente AFR2-20261029-27:** largo >=800ms ejecutado exclusivamente al soltar: Inicio -> Controles rápidos -> Inicio, resto atrás por contexto/historial; corto reservado en Inicio. Configuración y pruebas conserva el árbol previo bajo la sexta tarjeta. Excepción Sí/No obligatoria y cancelación Hall sin perder drafts. Radar y servo comparten editor; escala/rastro se guardan con EEPROMv5. [Contrato fuente y aceptación pendiente](../contracts/QUICK_RADAR_ACTUATORS_9.md). Las reglas anteriores que describen largo global a menú/Volver obligatorio son históricas. Sin build, validadores ni carga.

> **AFR2-20261029-26:** Sistema → Batería → Realizar medición / Ver medición / Volver. Medir inicia adquisición periódica y abre detalle con Volver al submenú. Giro selecciona, corto confirma, largo abre principal. Web: reinicios solo tras modal Confirmar y reiniciar; Cancelar/Escape/cierre no envían. Fuente vinculada sin build/generador/pruebas. [Contrato9](../contracts/BATTERY_MEASUREMENT_9.md).

> **AFR2-20261029-20:** Hall una vuelta conserva Iniciar/Guardar/Descartar/Volver, sin Finalizar vuelta separado. Iniciar elige rueda1FL/2FR/3RL/4RR → corto comienza → corto termina. Largo en selector/conteo cancela al menú Hall con aviso, sin borrar drafts previos. Estado muestra4CPR y pendientes. Volver con drafts pregunta Si/No sin Volver; largo no evade esa decisión. [Contrato y excepciones contextuales](../contracts/HALL_CALIBRATION_WIZARD.md). Fuente OLED/Web vinculada, sin prueba física; no flashear.

## Vigente AFR2-20261025-14 — Vistas hermanas

Por pedido humano actual, Radar/Numérica e IR Gráfica/Valores alternan con giro en ambos sentidos sin apilar historial; corto vuelve al listado restaurando selección/scroll. Ángulo del servo: giro controla, corto vuelve. Es una excepción contextual explícita al requisito anterior de Volver por ítem. Largo800ms abre principal y nunca genera corto al liberar. Los demás menús conservan Volver por ítem. Cada hermana tiene ScreenCode distinto y navegación revisionada; la captura anterior queda inválida.

UI_ACTION permite navegar en estas vistas y volver, pero no mover servo por giro remoto; ese control usa robot.servo.set. Fuente C y biblioteca Web se entregan juntas, incluyendo recursos y comportamiento. [Contrato8](../contracts/SENSORS_RADAR_8.md). Sin ensayo de navegación/paridad.

# Navegación de un único encoder

AFR2-20261024-08: todos los submenús conservan Volver, incluidos IP/AP y
Sistema → Reinicio. Nuevas acciones solo por clic físico; ui.action no las
invoca ni abre la vista de clave AP. Avisos: corto cierra el aviso, largo
abre principal. Iconos fijos, solo texto seleccionado con pausa800ms/scroll40ms;
el giro reinicia la pausa. Sin delays ni callbacks de bus desde renderer.
Compilado; navegación/scroll físicos no probados.

AFR2-20261024-07: la nueva pantalla automática de secuencias conserva largo SW
para principal, corto confirma y Volver explícito a Control. No captura la
navegación después de que el usuario sale. Compilada, sin ensayo físico.
[Contrato](../contracts/EX01_SYSTEM_SEQUENCE_4.md).

Estado vigente AFR2-20261024-02: firmware y consumidor Web compilados. Largo SW
abre principal también desde teclado, descartando credenciales sin enviarlas.
Esto sustituye la excepción histórica de la ficha19 que aparece al final.
No se ejecutaron ensayos físicos ni de navegador en esta entrega.

Decisión del usuario: no hay botón de menú separado. Las entradas son CLK, DT y SW del encoder.

| Entrada | Evento semántico | Resultado |
| --- | --- | --- |
| Giro | rotation +/- | Cambia selección o valor en contexto de edición |
| SW corto | click al soltar | Activa opción/confirma; no significa atrás |
| SW largo | menu una vez mientras está pulsado | Abre menú principal desde cualquier pantalla |
| Soltar después de largo | ninguno | Nunca confirma una opción accidentalmente |
| Ítem Volver + corto | acción back del ítem | Retorna al padre definido |

Atrás solo por opción de submenú. No mapear largo, doble clic, giro de extremo ni botón remoto genérico al retorno automático. Un control Web de menú puede representar la misma acción semántica sin simular duración eléctrica, una vez exista contrato remoto.

Defaults fuente: muestreo 1 ms, debounce20 ms, largo800 ms, cuatro transiciones por detent; son parametrización inicial, no resultado medido. La dirección y dos/cuatro transiciones dependen del encoder real. Si SW arranca pulsado se espera la liberación antes de armar gestos. Transiciones cuadratura inválidas reinician acumulación.

En código inicial Afr2Ui_Apply da prioridad a menu, resetea selección del principal y no emite click a la vez. Principal: Sensores/Estado OLED/Volver a inicio. Sensores y Estado OLED: único ítem Volver al principal. No hay edición implementada todavía.

Para nuevas ediciones: mantener borrador separado del estado confirmado; largo abre menú sin guardar ni ejecutar acciones. Definir si se conserva el borrador y cómo se descarta explícitamente. Volver no debe mover el robot ni reanudar una misión por efecto lateral. STOP/control de seguridad son independientes de navegación.

## Árbol objetivo

El [plan de menús](MENU_PLAN.md) extiende la navegación a Estado, Control, Modos/GOTO/línea, Pruebas, Sensores, Calibración, Wi-Fi, Pantalla y Sistema. Es diseño pendiente; no amplía las cuatro pantallas del prototipo por sí solo. Conservar Volver como única navegación al padre y largo SW como menú.


## Integración AVR AFR2-20261023-15

Polling físico desde main integrado, sin DMA ni botón menú. Debounce20ms y
largo800ms desde flanco crudo que supera debounce; liberación al umbral largo
emite menú y suprime corto. Mantiene Volver por ítem y supresión al arrancar
pulsado. 2/4transiciones y sentido configurables en composición; valores reales
y cadencia no probados. Wire de acciones remotas continúa pendiente.

## Excepción de teclado y captura remota —19

Pedido expreso del usuario: en teclado SSID/password, SW largo cancela el borrador
y vuelve al contexto anterior; OK termina el campo. Fuera del teclado, largo abre
principal, descarta edición numérica y nunca confirma. Volver restaura el historial
de selección/scroll (hasta8 niveles). No hay botón menú añadido.

Entrada física cancela captura remota. Navegación y avisos asíncronos invalidan la
referencia de acción anterior. UI_ACTION no introduce editores ni privilegios;
Conectar un borrador Wi-Fi requiere clic físico. [Permisos/validación](../contracts/ROBO2_PRODUCT_EXTENSION.md).


## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.


## AFR2-20261030-01 — Dashboard de AutoMicro adaptado

ScreenCode010001: cabecera con Wi-Fi/STA o AP, SSID/IP con scroll y batería por voltaje válido; centro con modo real y estado de motores; pie con encoder corto → Menú010002 y encoder con barra de duración → Rápidas010003 (largo >=800ms al soltar). Sin cambios de navegación ni activación de actuadores. Iconos PROGMEM y copia raster Web vinculados; no se añade framebuffer. Batería vacía/1/2/3/4 barras por bandas0/<6/<7/<8/>=8V, no porcentaje ni autonomía; inválida/deshabilitada/caducada muestra ? y --V. Medición sigue explícita desde Sistema → Batería. Red desconocida se distingue de desconexión confirmada. Fuente sin compilar, validar, probar, promover ni cargar. [Ficha](../changes/2026-10-30-01-dashboard-automicro-icons.md).


## AFR2-20261030-02 — Cabecera de red

Dashboard010001: icono Wi-Fi/AP izquierdo, SSID arriba e IP local STA/AP debajo; batería derecha. Ventanas21..90 con scroll independiente ida/vuelta, pausa800ms en extremos y paso40ms. Solo bandas de texto se actualizan en scroll; iconos fijos. Fuente OLED/Web vinculada sin build ni carga. [Ficha](../changes/2026-10-30-02-dashboard-network-scroll.md).
