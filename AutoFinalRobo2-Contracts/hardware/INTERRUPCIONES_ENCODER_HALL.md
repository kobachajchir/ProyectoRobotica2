# Encoder UI y Hall por interrupciones — AFR2-20261024-11

24/10/2026. Implementación en fuente, sin compilar, validar, flashear ni probar.
El cambio de CLK/DT requiere recablear; no describe un montaje ya realizado.

## Asignación actual

| Entrada | Mega | AVR / interrupción | Uso |
| --- | --- | --- | --- |
| Encoder CLK | D2 | PE4 / INT4 | Cualquier cambio de nivel |
| Encoder DT | D3 | PE5 / INT5 | Cualquier cambio de nivel |
| Encoder SW | D42 | PL7, sin INT/PCINT | Muestreo main objetivo1ms |
| Hall FL A/B | A8/A9 (D62/D63) | PK0/PK1 / PCINT16/17 | A interrumpe; B da sentido |
| Hall FR A/B | A10/A11 (D64/D65) | PK2/PK3 / PCINT18/19 | A interrumpe; B da sentido |
| Hall RL A/B | A12/A13 (D66/D67) | PK4/PK5 / PCINT20/21 | A interrumpe; B da sentido |
| Hall RR A/B | A14/A15 (D68/D69) | PK6/PK7 / PCINT22/23 | A interrumpe; B da sentido |

Las ocho señales Hall soportan PCINT2. Se conserva x1: máscara PCMSK2=0x55,
interrupción por cambios de A y cuenta únicamente en subida; lectura simultánea
PINK para los cuatro pares y dirección desde B. No se habilita IRQ por B porque
no se utiliza x2/x4. Se conservan unidades de calibración, orden FL/FR/RL/RR y
signos configurables. No asignar11pulsos ni3300cuentas como medición real.

D2/D3 están libres según el mapa documental y el código activos. Comparten
función alternativa OC3B/OC3C, pero Timer3 no habilita esas salidas. A8–A15 no
colisionan con línea A0–A4, PWM/direcciones, servo, UART1 o TWI en ese mapa.
Esto es una revisión documental; acceso físico del shield, continuidad, niveles,
pull-ups y correspondencia real S1/S2 con A/B siguen sin comprobar.

Fuente: [pinout oficial Mega, páginas1/2](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf).
SW, SDA/SCL y conectores1x5/1x2 conservan sus posiciones; D40/D41 quedan liberados.

## Captura y consumo

INT4/INT5 leen CLK/DT en una sola captura PINE y publican estados AB en una cola
circular estática de32bytes,31entradas útiles. Índices/flags compartidos son
volatile. Se descarta la segunda entrada si ambos vectores corresponden al
mismo estado ya capturado. No hay decodificación, navegación, I2C, PID ni acciones
motoras dentro de esas ISR. Una bandera booleana sola perdería multiplicidad.

Main consume hasta32entradas por vuelta, decodifica cuadratura conservando
transiciones2/4 e inversión configuradas y aplica los pasos en orden. Cada retiro
es atómico y breve. Ante overflow deja de publicar, devuelve discontinuidad,
vacía pendientes y resincroniza AB/partial sin fabricar pasos. Esta señal queda
interna: no se añade telemetría ni contador público de pérdidas. La cola no es
ilimitada ni garantiza capturar todos los flancos bajo cualquier carga.

El pulsador conserva muestreo en main, antirrebote20ms, largo800ms exclusivo,
supresión de gesto al arrancar pulsado, corto confirma y largo abre principal.
Main posterga el pulsador si quedaron transiciones anteriores por consumir.
No se reconstruyen muestras de SW que no se observaron. No cambian renderers,
ScreenCodes, Web ni semántica de navegación. Poll queda como API de compatibilidad;
el producto usa Transition y Button por separado.

## Preferencia de Hall y límite del ATmega

El ATmega2560 tiene prioridades de vectores fijas: INT4/INT5 preceden a PCINT2.
No se configura una prioridad numérica ni preempción selectiva. Ver
[Microchip AVR1200, tablas1-1/1-3 y sección1.4](https://ww1.microchip.com/downloads/en/AppNotes/Atmel-8468-Using-External-Interrupts-for-megaAVR-Devices_ApplicationNote_AVR1200.pdf).

Para dar preferencia de servicio a Hall conservando este pinout, al entrar por
INT4/INT5 se comprueba PCIF2 y se captura primero Hall si está pendiente y
habilitado. Luego se guarda el estado UI. PCINT2 usa la misma captura corta,
sin bucles variables: cuatro comprobaciones desenrolladas y acumuladores32bits.
PCIF2 no se borra desde UI: un flanco posterior debe conservar su solicitud.
La entrada posterior por PCINT2 compara contra PINK previo y no vuelve a contar
una transición ya atendida. No hay sei dentro de ISR ni ISR_NOBLOCK.

Esto da preferencia al Hall ya pendiente al entrar en UI; NO cambia la prioridad
hardware ni permite a Hall interrumpir una ISR en curso. Si Hall llega después
de la comprobación espera el resto de esa ISR. Frecuencia máxima sin pérdidas,
ciclos, secciones atómicas y latencias deben medirse antes de habilitar potencia.
Si se exige prioridad estricta/preempción, este pinout necesita otro diseño;
no se acredita con esta adaptación.

## Estados reales

AFR2_HALL_CONNECTED=0 y AFR2_SEQUENCE_MEMORY_APPROVED=0 se conservan. Hall no se
habilita por actualizar pines/documentos. Captura Hall y snapshot atómico se
centralizan en afr2_board.c; afr2_sensors.c consume el snapshot. La composición
UART sola conserva el enlace sin depender del módulo sensores; no habilita UI
ni Hall sin llamar a sus inicializadores.

ATmega: fuente nueva sin compilar ni probar; build08 es evidencia anterior y
sus cifras de SRAM/pila no describen esta fuente. ESP conserva firmware08 y
activosWeb10; Web conserva10. Wire ROBO2/EX01v5 y configuración EEPROM intactos.
CAD no se edita: solo requisitos de conexión en Contracts. No tests, compilación,
validadores, flasheo ni ensayo físico en11.
