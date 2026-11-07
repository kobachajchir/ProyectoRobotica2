# AFR2-20261106-12 — Hall por motor y mando manual continuo

Estado: ATmega/ESP/Web adaptados en fuente. Sin tests, validadores,
compilaciones, generadores, promoción LittleFS, carga ni pruebas físicas.
EX01v13 y ROBO2/perfil2 conservados. Persistencia AVR pasa a EEPROMv10.

## Hall y navegación

Hall 030014 contiene Estado, Calibrar motor y Volver. Calibrar abre el
selector compartido 030044 (contexto Hall 030036), FL/FR/RL/RR. Corto elige
motor e inicia; se gira **a mano exactamente una vuelta CW**, vista desde
el exterior de esa rueda, con HC-SR04 hacia el frente del auto. No acciona
el motor durante la medición. Se cuenta por flanco ascendente A, leyendo B
para el sentido (x1); CPR significa cuentas medidas por vuelta de rueda,
incluida su reducción, sin asumir pulsos nominales del motor.

030037 muestra conteo firmado y Finalizar por corto. Finalizar abre la
variante Hall de 030038: cabecera «Sensor Hall FL/FR/RL/RR», cantidad de
pulsos y Confirmar/Descartar con iconos de guardar/retorno y foco invertido.
La cabecera compacta conserva legibilidad en 128x64. Confirmar persiste y
espera finalizar el journal EEPROM antes de regresar a Hall, con el foco
en Calibrar motor; permite repetir con otra rueda. Descartar elimina el
borrador de esa rueda. Largo no saltea esta decisión. Largo durante
selector/conteo cancela el intento actual y regresa por contexto.

Estado 030015 reúne las cuatro ruedas: CPR y OK, Sin calibrar o borrador
marcado *. Una nueva calibración invalida y programa en EEPROM **solo** el
CPR de la rueda elegida; el resto conserva sus valores y polaridades.
Descartar/cancelar ese intento no restaura su calibración anterior. No hay
flag global que invalide las otras tres ruedas. CPR no nulo representa la
calibración guardada; esto no certifica recepción sin pérdida de pulsos ni
exactitud mecánica. La invalidación queda durable al completar el journal,
no instantáneamente frente a corte de alimentación.

Al confirmar se normaliza CW positivo/CCW negativo. Antes de la primera
calibración, el signo bruto depende de A/B; por eso la vuelta de referencia
debe ser CW aunque el contador inicial muestre negativo. La detección de
cambio de sentido y el rechazo de cero/>1000000 cuentas se conservan.

EEPROMv10 mantiene dos registros de 128 bytes, CRC y marcador final; payload
LE de 64 bytes, frente a 63 de v8/v9. Agrega `hall_cw_mask` al final: un bit
por rueda que ya usa la convención exterior. Lectura de v1..v9 conserva
CPR/signos anteriores con máscara0; recalibrar migra una rueda a la vez.
La odometría transforma el signo de las ruedas derechas ya migradas a la
base cinemática existente (avance usa velocidades internas negativas).
La telemetría Hall conserva la convención exterior de la rueda calibrada.
No se transforma dos veces el mando de motores ni se cambian sus trims.

Los datos sobreviven al reinicio. Regrabar firmware **puede conservar
EEPROM**: borrar flash de programa no garantiza borrar calibraciones.
Restablecerlas exige limpiar también los datos persistentes o recalibrar.
La escritura sigue siendo de la configuración completa existente: guardar
Hall también incorpora los ajustes que estén actualmente en esa configuración.

## PWM manual/táctil

`robot.drive.pwm` conserva porcentajes enteros -100..100 por eje y TTL
200..1000ms (Web usa400ms; renueva cada180ms). El ESP usa un único slot
reciente, reemplazable, con caducidad200ms desde recepción; no acumula FIFO
de consignas. Responde WS `status=accepted`, `payload.queued=true`,
`payload.confirmed=false`: aceptación local del ESP, **no** ejecución AVR.

Se transmite el mismo AFR2_EXT_DRIVE_PWM, cuerpo12 bytes: token0 u32 LE,
forward/strafe/turn i16 LE, TTL u16 LE, bajo epoch/sequence habituales.
El ESP no abre una espera de resultado para ese PWM. El AVR conserva su
respuesta normal; el parser ESP la drena y la ignora al no corresponder a
una transacción activa. Otros comandos, ALIVE, WEB_PULSE y STOP conservan
correlación/confirmaciones. PWM no actualiza artificialmente lastGood del
enlace. Las respuestas tardías no se usan como confirmación de otra orden.

El slot tiene dueño de sesión y generación; se descarta por expiración,
pérdida de enlace/sesión, revocación o STOP. Otra acción de movimiento
explícita elimina el PWM pendiente anterior. WEB_PULSE debido mantiene
prioridad y PWM reciente precede consultas ordinarias. No se transmite si
hay otra transacción o TX pendiente; nunca se mezclan bytes de dos tramas.

BUSY/TIMEOUT transitorios ya no fabrican una parada de emergencia desde
el bucle Web. Si dejan de llegar consignas, el TTL AVR sigue deteniendo
las salidas; pérdida de sesión, foco/visibilidad y STOP mantienen su flujo.
Errores no transitorios siguen cancelando el gesto. La presentación OLED
táctil se limita a una actualización cada200ms y refresco700ms sin cambios.
No es un mando de motor ni una medida del ángulo real.

El giro continuo con dos dedos envía turnPct; no requiere Hall. Pasa a ser
la opción predeterminada para usuarios sin preferencia guardada. La opción
explícita de giro por ángulo con Hall conserva GOTO x=y=0 al soltar y sus
requisitos de odometría. Preferencias existentes se respetan. No afirmar
que el auto giró por ver variar el dibujo del OLED.

## Restricciones y trabajo pendiente

- PWM manual conserva el bypass existente solicitado: habilita motor_ready,
  limpia el fallo y cancela cabeza/calibración/secuencia que interfieran.
  **Pendiente restituir esos permisos/interlocks**, sin reintroducir esperas
  por ACK de cada PWM. No se toma esta excepción como aprobación del resto.
- Prueba individual: motor habilitado, sin fallo/calibración/cabeza ocupada,
  PWM dentro del límite y duración100..2000ms. No necesita odometría.
- Control de velocidad, GOTO y seguimiento: cuatro CPR válidos, Hall
  disponible, geometría, motores y ausencia de fallo. GOTO también pose
  válida; línea necesita IR válido/calibrado. **AFR2-20261106-13:** ningún
  modo físico vigente requiere HC-SR04 ni distancia frontal mínima para
  iniciar o continuar. Lectura, telemetría y radar se conservan. Aproximación
  física al área del brazo queda pendiente de un modo futuro explícito;
  el comportamiento simulado no habilita ese requisito en el auto.
- Geometría nominal actual: rueda80mm, Lx+Ly170mm. Circunferencia
  `80*pi = 251,327412 mm`; distancia por cuenta de rueda i = `251,327412/CPR_i`.
  Un diámetro permite distancia por rueda, no determinar por sí solo el giro
  del chasis: hacen falta geometría efectiva, orientación Mecanum y signos.
- No hay MPU. Pose/giro se estiman por ruedas: deslizamiento, rodillos,
  carga y diámetro efectivo pueden introducir deriva. Calibrar CPR no
  calibra PID ni comprueba sentido físico del driver o montaje Mecanum.
- Conservados: límite de velocidad/PWM, rampa, fallo por lazo>100ms,
  ausencia de pulsos500ms en control cerrado, GOTO<=60s y límites de
  objetivo. Secuencias mantienen su gate explícito de memoria aprobada.
- Sin ejecución autorizada: presupuesto SRAM/pila, migración EEPROM real,
  integridad de pulsos, paridad de píxeles y fluidez USART quedan pendientes.
  No se actualizaron artefactos generados ni paquete del ESP.

Procedencia por archivo: `provenance/hall-motion-20261106-12.json`.
