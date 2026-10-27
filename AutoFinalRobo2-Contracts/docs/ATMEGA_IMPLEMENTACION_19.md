# ATmega 0.4.0: implementación y límites

AFR2-20261023-19. Trabajo iniciado23/10 y cerrado documentalmente24/10/2026.
Estado: fuente de producto integrada, sin compilar ni ejecutar tests, validadores,
flasheos o pruebas físicas. Las compilaciones ESP/Web de otras fichas no prueban
este firmware ni EX01. [Plan/matriz](PLAN_ATMEGA.md) ·
[Complemento ESP/Web](../contracts/ROBO2_PRODUCT_EXTENSION.md).
[Guía guardada en firmware ESP](../../AutoFinalRobo2-ESP/docs/ROBO2_EX01_ATMEGA.md).

## Organización del código

| Archivos en ATmega | Responsabilidad |
| --- | --- |
| include/afr2_menu_catalog.h, src/ui/afr2_menu_catalog.c |44 menús/vistas e ítems en PROGMEM, identidades propias |
| include/afr2_screens.h, src/ui/afr2_screens.c | Navegación, historial, editores, teclado y notificaciones; renderer puro |
| include/afr2_app.h, src/services/afr2_app.c | Wrappers, requisitos, snapshots y acciones hacia servicios |
| include/afr2_product.h, src/services/afr2_product.c | Estado, seguridad, motores, control, Hall, odometría, línea y GOTO |
| src/platform/afr2_motor.c | PWM/dirección de cuatro ruedas, desconexión de compare al parar |
| src/platform/afr2_sensors.c | Línea5 digital/ADC, HC-SR04, Hall PCINT y servo opcional |
| src/platform/afr2_preferences.c | EEPROM versionada, CRC y dos registros recuperables |
| src/platform/afr2_watchdog.c | Motivo de reset y watchdog desde main |
| src/services/afr2_app_wire.c | Endpoint EX01, framebuffer retenido y solicitudes Wi-Fi |
| src/services/afr2_link.c | Perfil2 conservado, callbacks de extensión/revocación y capabilities |
| src/display/afr2_oled.c | Contraste/orientación asíncronos por I2C Manager |
| src/main.c, Makefile | Composición de producto y enlace libm; alternativa UART sola |

Se conservan las referencias F4/autores. No se portan System/globales, HAL, DMA,
MPU/balance, NRF ni AT. Es código nuevo basado en conceptos de esas bibliotecas.

## Interfaz local

Árbol completo del plan: Estado, Control, Modos/misiones, Pruebas, Sensores,
Calibración, Wi-Fi, Pantalla/interfaz y Sistema. Se mantienen los cuatro IDs
originales; las nuevas vistas03xxxx están en screens.json. Hay tres filas de
menú y un historial de8 contextos para regresar también desde accesos cruzados.

Giro selecciona/edita; clic aplica un borrador en RAM o activa una opción.
Guardar EEPROM es separado; Descartar restaura la configuración guardada.
Largo abre principal y descarta la edición numérica sin aplicar. Volver nunca
inicia movimiento. Salir de una maniobra local la detiene. Los ítems no disponibles
se identifican y explican los requisitos al seleccionarlos.

Excepción expresa del usuario: en teclado, SW largo cancela y vuelve al contexto
anterior. OK termina SSID y después password. Teclado ASCII32..126, borrar y OK;
SSID hasta32bytes, password vacío o8..63. La contraseña se enmascara y se borra
al terminar/cancelar o perder la sesión. No hay edición UTF-8 local; Web sí puede
usar el soporte UTF-8 del servicio ESP. La selección de red precarga el SSID.

Notificaciones: cola estática4, deduplicación, barra temporal y3000ms por defecto.
Cerrar con clic consume ese gesto. Al terminar se restaura el contexto sin
reejecutar acciones. Aceptación, connecting y resultado son estados diferentes;
la barra no es porcentaje real de conexión. No se declara paridad gráfica con
todos los íconos/renderers de F4. No se edita texto oculto bajo una notificación.

Wi-Fi ofrece Ingresar aquí o Ingresar en Web. El segundo publica una solicitud
para ESP/modal Web; en ambos casos el auto muestra revisión con Conectar,
Cancelar borrador y Volver. El endpoint y los borradores del Mega están escritos;
el puente ESP/modal específico requiere implementar el contrato EX01. Un único
WebSocket es requisito de ese complemento, no un cambio ya hecho en ESP aquí.

## Movimiento y configuración inicial

Ruedas0FL,1FR,2RL,3RR. Motores cableados según el usuario; signos físicos aún por
comprobar. PWM8 Timer1/2, prescaler64, candidato~976Hz. STOP desacopla compare,
apaga GPIO/consignas e integrales sin esperar rampas; no mide rueda inmóvil.
Límite inicial80/255, rampa4/tick10ms y paso por cero antes de invertir.

Prueba individual100..2000ms, default500ms, independiente de odometría. Acción
física Ejecutar adquiere control local si está libre; control Web exige toma local
explícita que detiene/revoca al anterior. Manual local500ms por clic, línea local
máximo30s, GOTO local60s. Remoto se habilita explícitamente en Sistema, no persiste,
exige token no cero y lease500..5000ms; DRIVE añade TTL200..1000ms. Los ticks
internos de línea/GOTO no renuevan el lease remoto.

Watchdog500ms alimentado al final de main. Control retrasado>100ms durante marcha
genera fallo/STOP. Consigna sin flancos Hall>500ms con PWM>=40 genera fallo.
Perder sesión o RX revoca control/extensión/borradores; reconectar no reanuda.

Velocidad inicial150mm/s, rueda nominal80mm, Lx+Ly0 sin configurar, motor_ready0,
Hall desconectados y CPR0. Movimiento físico/PID/GOTO quedan indisponibles hasta
configuración/calibración. No convertir PWM en una velocidad medida inventada.
PID Q8 inicial Kp128/Ki8/Kd0, integrales independientes/saturadas; no ajustado en
el vehículo. Odometría en main con libm, nunca float en ISR. GOTO relativo al
rumbo inicial, cada eje±5m, tolerancia software10mm, timeout60s; no es precisión
física acreditada. Pose solo desde Hall, con marco/secuencia propios y sin IMU.

## Sensores y calibración Hall

Línea:0sin configurar/1digital/2ADC10bits. IR1..5=PF4..PF0, snapshots completos,
mayoría de3 scans, umbrales512 iniciales y máscara de inversión. Confirmar
polaridad/calibración es explícito; sin ello las lecturas normalizadas son
inválidas. Init libera JTAG de PF4 en software sin escribir fuses. La calibración
actual es manual por umbral/polaridad, no el procedimiento óptico/MPU de F4.

HC-SR04 D30/D31: Timer3 cada25us solo durante captura, trigger25us, espera de
subida/bajada y timeout32ms; intervalo70ms y caducidad200ms. Sin delay/pulseIn,
no depende del renderer. Eco ausente=inválido, no camino libre. Control de marcha
exige distancia frontal válida>=150mm; no acredita cobertura lateral/trasera.

Servo D13 opcional1000..2000us mediante Timer5 sin tomar Timer1. No barrido
automático. Fuera del centro nominal1450..1550us o durante300ms tras moverlo se
invalida lectura frontal. Desactivar pulso no centra ni mide orientación.

Hall: AFR2_HALL_CONNECTED=0 por defecto, más hall_wired en configuración. Propuesta
vigente AFR2-20261027-07: FR A8/A12, FL A9/A13, RR A10/A14, RL A11/A15.
Headers A8-A11 para A y A12-A15 para B, orden físico FR/FL/RR/RL; contadores
lógicos FL/FR/RL/RR conservados. PCINT2 máscara0x0F: x1 por flanco ascendente A,
B aporta signo. Niveles/pull-ups y funcionamiento pendientes; no polling del encoder UI.

Test: seleccionar rueda → Iniciar → girar UNA vuelta hacia adelante → Finalizar
→ revisar → Guardar o Descartar. Salidas apagadas; rechaza cero/inversiones y
cuentas>1.000.000. Guardar actualiza CPR/signo por rueda, invalida origen y agenda
EEPROM. El sistema no puede saber si el usuario giró exactamente una vuelta.
No se usa11 ni75:1 como medición. Se conserva x1 en calibración y funcionamiento.
Cambiar de rueda descarta el resultado pendiente; el endpoint exige la misma
rueda al finalizar/guardar. Un resultado fuera de rango queda marcado inválido.
Hall local libera el lease local para medir sin el límite de5s; el Hall remoto
conserva su token/lease y exige renovación.

## Persistencia, memoria y aceptación pendiente

EEPROM0..255: dos registros128bytes, cabeceraAFR2, versión1, payload58, secuencia
u32LE, CRC16-CCITT polinomio1021/initFFFF y marcadorA5 escrito al final. CRC
cubre0..126 excepto14/15. Serialización campo por campo; último registro válido
manda, ambos inválidos cargan defaults. Escritura incremental solo con periférico
libre; no guardar por giro ni incluir credenciales.

Presupuesto por inspección: alrededor3,3KiB estáticos explícitos más literales,
temporales/runtime/pila. NO es mapa ni memoria libre medida. Framebuffer1024,
TX129, cola4 e historial8; captura remota retiene el framebuffer hasta600ms sin
duplicarlo. Confirmar SRAM/pila, ubicación PROGMEM<64KiB y objetivo de margen2KiB
antes de declarar suficiente memoria.

Pendientes bajo pedido: build AVR/linker/libm, mapa/pila, tiempos IRQ/eco/servo/
Hall/UART/render, polaridades, PID, recuperación EEPROM y aceptación de contratos.
No hay binario ni exactitud física medida por esta ficha.

La ficha21 de secuencias agrega un diseño de perfil3 posterior al alcance acordado.
Sus IDs10..15 colisionan numéricamente con EX01 del perfil2 y NO deben mezclarse.
El ejecutor de secuencias perfil3 no se incorpora ni se anuncia con esta entrega.
