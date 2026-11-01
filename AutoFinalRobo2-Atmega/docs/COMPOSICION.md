> **AFR2-20261029-16 compilado, NO cargado por indicación del usuario:** producto/comunicación/biblioteca exit0, sin errores/advertencias, final29/10/2026 12:15:28. Flash75518bytes; SRAM6918bytes, margen1274bytes; main259 y renderer73bytes según .su individuales, sin certificación dinámica de pila. COM8 conserva15 (cargada12:02:57). [Evidencia](../../AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-16/README.md). El usuario pidió continuar con otros cambios y posponer la carga; no se solicitará flasheo hasta nueva instrucción.

> **Fuente AFR2-20261029-16:** OLED sin esperas artificiales entre etapas, radar con grilla punteada, cono visual nominal15grados por PWM aplicado y discos rellenos por eco válido. Copia Web vinculada; sin build Web/promoción ESP. [Ficha](../../AutoFinalRobo2-Contracts/changes/2026-10-29-16-oled-latency-radar-cone.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota. Se conserva corrección del watchdog y migración Hall de15, ya cargada y verificada enCOM8 a12:02:57.

> **Fuente AFR2-20261029-15:** corregido bucle Bresenham del radar que bloqueaba main y podía disparar watchdog al girar desde HC-SR04; productos trigonométricos AVR en32bits y contraparte Web vinculada. Hall conectado por montaje confirmado, migración local EEPROMv3 preserva calibraciones y habilitación de control. [Ficha](../../AutoFinalRobo2-Contracts/changes/2026-10-29-15-radar-watchdog-hall-wiring.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota; sin prueba física del cambio.

> **AFR2-20261029-14 compilado y cargado:** 29/10/2026 11:37:12, COM8, Loader1.1.2,74516bytes escritos y verificados, exit0. Producto/comunicación/biblioteca compilados sin errores/advertencias. SRAM estática6889bytes, margen1303bytes; main259bytes según .su individual, sin certificación de máximo dinámico. [Evidencia](../../AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-14/README.md). La entrega sin compilar indicada debajo es histórica; sin ensayo físico posterior de UI.

> **Fuente AFR2-20261029-14:** recuperación OLED sin apagar ante error aislado, conserva páginas confirmadas y acota la postergación por trabajo pendiente. Reconfiguración sin0xAE tras fallos persistentes. Sin nueva compilación, tests, validadores, carga ni prueba física; COM8 conserva11. SRAM/pila y causa del negro permanente pendientes. [Investigación y cambios](../../AutoFinalRobo2-Contracts/changes/2026-10-29-14-oled-fault-recovery.md).

> AFR2-20261024-11: captura IRQ/cola UI y Hall centralizadas en board. [Pines y límites de prioridad](../../AutoFinalRobo2-Contracts/hardware/INTERRUPCIONES_ENCODER_HALL.md). Fuente actual sin compilar ni probar; los hitos inferiores conservan su contexto histórico.

# Composición AVR integrada

AFR2-20261023-19: composición de producto y EX01 ya escritos, sin compilar/probar.
[Guía vigente](PRODUCTO_ATMEGA.md), [detalle por archivo](../../AutoFinalRobo2-Contracts/docs/ATMEGA_IMPLEMENTACION_19.md),
[API/complemento](../../AutoFinalRobo2-Contracts/contracts/ROBO2_PRODUCT_EXTENSION.md).
Las restricciones y estados de las fichas anteriores abajo son históricos.


> **Fuente AFR2-20261029-11:** OLED al final de main, sin refresh completo
> periódico. Se pausa ante input/respuesta pendiente y reanuda estado/snapshot.
> Diferencias exactas por páginas/tramos16. Sin nuevo build/carga; presupuesto
> SRAM/pila pendiente. [Ficha](../../AutoFinalRobo2-Contracts/changes/2026-10-29-11-oled-delta-cooperative.md).

> **AFR2-20261029-10:** composición de producto incorpora configuración de
> OLED/encoder confirmada en diagnóstico09, sin introducir scanner/debug UART0.
> [Migración UI EEPROM](../../AutoFinalRobo2-Contracts/contracts/EEPROM_UI_MIGRATION_2.md).
> Solo fuente; nuevo build/carga de producto pendientes.

AFR2-20261023-15. Estado: fuente escrita, sin compilación ni ejecución en placa.
Lectura previa: README/INFORME_ADAPTACION, maestro y guías internas de módulos;
referencias F4 conservadas solo como evidencia, nunca como build activo.

## Arranque y ciclo

`src/main.c` integra handles estáticos Afr2Link, Afr2I2cManager, Afr2Oled,
Afr2EncoderUi y Afr2Ui. `src/platform/afr2_board.c` detiene compare outputs
Timer1/2/3/4, pone PWM/direcciones documentados inactivos y configura Timer0
CTC/1 ms (16 MHz, prescaler64/OCR249). No confirma estado físico ni implementa
el driver de cuatro motores. Luego UART1, TWI, Manager, OLED y niveles reales
del encoder; habilita interrupciones al final. Sin Arduino ni DMA.

Ciclo cooperativo: atender fault/supervisión/UART con presupuesto64 bytes;
consumir hasta32transiciones del encoder por vuelta desde FIFO IRQ; SW por tick; TWI recovery/Manager Tick;
actualizar snapshot y redibujar si cambia; OLED Task envía páginas pendientes.
No sintetiza muestras GPIO perdidas. Tick multibyte se lee atómicamente. No hay
dibujo/handlers en ISR, delay ni malloc en el flujo nuevo. La cadencia efectiva
de 1 ms y tiempos de render deben medirse, no se garantizan por el tick.

Renderer puro y wrappers originales de la base adaptada se conservan: Inicio,
menú reducido, Sensores y Estado OLED. No se importan MPU/balance ni catálogo
de acciones F4. Sensores sin productores muestran `--`. OLED inicializado/error
provienen del servicio local. No hay reporte/acciones remotos implementados.
El árbol completo MENU_PLAN.md sigue pendiente.

Encoder CLK D2/PE4/INT4 y DT D3/PE5/INT5 se captura con una lectura PINE en IRQ. FIFO32bytes/31útiles, retiro atómico y decodificación en main. SW D42/PL7 activo bajo conserva muestreo. Hall pendiente se atiende primero al entrar por UI; no cambia prioridad hardware ni se anidan ISR. Overflow resincroniza sin pasos ficticios.
Debounce20 ms, largo800 ms desde flanco crudo que se estabiliza. Si se suelta
justo al umbral largo también emite menú, nunca corto; soltar después de largo
no confirma. Arranque pulsado exige soltar. Atrás solo por ítem Volver.

## Configuración candidata, no detección

| Parámetro fuente | Valor inicial | Pendiente |
| --- | --- | --- |
| AFR2_OLED_ADDRESS7 | 0x3C | Controlador SSD1306 128×64 y dirección reales |
| TWI | 100 kHz, TWBR72/prescaler1 | VCC, pull-ups externos, cableado; no pull-ups internos I2C |
| AFR2_ENCODER_TRANSITIONS | 4 (permite2) | Detents reales |
| AFR2_ENCODER_INVERT | 0 | Sentido de montaje |
| AFR2_ENCODER_INTERNAL_PULLUPS | 1 | Switches pasivos a GND y compatibilidad; 0 exige pull-ups externos |
| USART1 | 1152008N1 candidato | Error nominal +2,12 %, robustez sin medir |

Macros configurables en fuente/CPPFLAGS; no cambian automáticamente hardware,
VCC ni tensión de J2. J1 señales 1x5 y J2 VCC/GND comunes 1x2 de2,54 mm siguen
siendo requisitos CAD, no placa creada. Leer hardware/PCB_OLED_ENCODER.md.

## TWI/I2C y memoria

TWI es un único master para OLED, atendido por TWI_vect. Manager es dueño de
transferencias/timeout40 ms y callbacks solo en main. El backend retiene buffer
hasta STOP completo o abort; nunca conserva datos de stack efímeros. Error
deshabilita hardware/IRQ antes de soltar puntero y agenda recovery en main,
hasta9 pulsos SCL open drain y STOP, <=50 ms; si sigue atascado queda fallido y
OLED vuelve a intentar tras1000 ms. Sin espera activa ni segunda pila I2C.
No soporta otros masters/clientes ni garantiza recuperación eléctrica.

Framebuffer1024 B, TX página129 B, comandos4 B; parser88 B de campos explícitos,
reply máximo76 B, UART256 B de arrays. En torno a1,7 KiB de estructuras/rings
estáticos por suma de campos AVR, más constantes residentes, pila, runtime y
temporales de handlers/render. Es una estimación de diseño, NO mapa ni margen
certificado. Reservar al menos2 KiB de pila/margen del presupuesto previo y
obtener mapa/medición cuando se autorice. Fuentes/textos OLED en PROGMEM;
comprobar ubicación por debajo de64 KiB o accesores far antes de ampliar recursos.

## Objetivos preparados

Makefile mantiene help por defecto. `library` crea biblioteca;
`firmware` genera afr2.elf/hex/map de composición integrada;
`communication-firmware` genera composición UART sola, sin TWI/UI. Se comparten
board/UART/link sin duplicar vectores ni main en un mismo target. No hay upload.
Dependencias .d incluyen el header ROBO2 compartido. Nada de esto se ejecutó.

Faltan servicios sensores/potencia/STOP/lease/Hall/GOTO, UI remota y menús de
producto; también validación de recursos, hardware y todas las pruebas.
