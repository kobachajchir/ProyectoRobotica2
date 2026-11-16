# AFR2-20261111-07 · Un ensayo GOTO enviado sigue aunque se corte el WebSocket

Versión: 1.27.1 (subversión mínima: corrección, sin función nueva).
Estado: **atmega** fuente editada, sin compilar ni cargar; **esp** y **web** solo versión. Sin CMD nuevo ni cambio de trama.

## Pedido

Si un ensayo ya se envió al auto y se desconecta el WebSocket, el ensayo tiene que seguir ejecutándose.

## Causa

- El ESP, al cerrarse una sesión WebSocket (`WS_EVT_DISCONNECT` → `stopWebOperations()`), manda `EX01 MOTION_HALT` (0x6A) al ATmega,
  que detenía todo movimiento.
- El GOTO aceptado de la Web quedaba con origen Web: sin sesión, el ESP deja de mandar el pulso y el ATmega lo cortaba a los 1,5 s.
- El mismo `MOTION_HALT` también cortaba los ensayos locales del OLED (Pruebas examen, prueba de distancia) al cerrarse la Web.

## Cambios (ATmega)

- `afr2_app_wire.c`, `EX01 GOTO`: un GOTO aceptado pasa a **origen local**, con tope de 62 s, así no corre el corte de 1,5 s sin
  pulso Web. Lo terminan la llegada, el límite propio de 60 s del GOTO, STOP, `robot.goto.cancel` o una falla. Mientras corre, el
  ATmega rechaza otras órdenes de movimiento de la Web, como cualquier tarea local; STOP y cancelar siguen aceptándose.
- `afr2_sensor_commands.c`, `MOTION_HALT`: no corta un GOTO en curso con origen local, sea un ensayo de la Web o del OLED. El resto de
  la conducción Web (PWM manual, rueda, secuencia) se detiene como antes. El acuse sigue en 1, como exige la validación del ESP.
- Sin cambio: si se cae el **enlace UART** entre ESP y ATmega (por ejemplo, un reinicio del ESP), el ATmega sigue deteniendo el
  movimiento por supervisión del enlace.

## Web (sin cambios en esta ficha)

El ejecutor de tramos (`useGotoRouteRunner`) ya tolera lecturas perdidas de `robot.state`: sigue consultando y, al volver la
conexión, ve el tramo terminado. En las trayectorias de varios tramos, el tramo en curso termina en el auto y el siguiente se envía
cuando la Web reconecta (dentro del tope de 65 s por tramo). Perder el foco de la ventana sigue lanzando STOP como antes.

## Pendiente

Compilar, cargar y probar con el auto: enviar un ensayo, cortar la Wi-Fi del teléfono y comprobar que el auto termina el
desplazamiento.
