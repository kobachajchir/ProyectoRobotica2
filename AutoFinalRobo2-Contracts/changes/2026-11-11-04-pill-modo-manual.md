# AFR2-20261111-04 · La pill de modo cambia el modo y Manual salta los permisos

Versión: 1.26.1 (subversión mínima: corrección y ajuste de permisos, sin función nueva).
Estado: **atmega** fuente editada, sin compilar ni cargar; **esp** fuente editada (solo texto del motivo), sin compilar;
**web** fuente editada, sin `tsc` ni navegador. Sin CMD nuevo: `EX01 MODE` (0x15) conserva su formato.

## Informe del usuario

Desde la pill de modo de la cabecera, elegir Manual o Línea no cambia el modo. A veces la placa se reinicia o se desconecta el
cliente WebSocket. En Conducir no deja manejar («Habilitar motores; revisar fallo, radar, calibración, secuencia, tarea local o
enlace»). Pidió que en Manual se pueda manejar igual, saltando todos los controles.

## Causas encontradas (lectura de fuente)

1. **Manual no se quedaba.** `EX01 MODE` ponía `mode=MANUAL` con el auto quieto, pero `Afr2Product_Task` detenía por vencimiento de
   consigna también a un auto **ya detenido** (`command_ms`/`test_started_ms` viejos). `HaltMotion` lo devolvía a Reposo en el mismo ciclo.
2. **La Web no dejaba elegir Manual** si el ESP no informaba `manual_pwm`, y `manual_pwm` exige motores habilitados, sin fallo, sin
   radar, sin secuencia y sin medición Hall. Era un círculo: sin elegir Manual no se habilitaba, y sin habilitar no se podía elegir.
3. **Manual con el auto en marcha** respondía `BAD_REQUEST` (solo se aceptaba con el auto quieto).
4. **Línea** fallaba con los motores deshabilitados o con el radar barriendo.

## Cambios

- ATmega: nuevo `manual_selected` en `Afr2Product`.
  - Elegir Manual (pill o Controles rápidos del OLED) corta lo que esté andando (tarea local, radar, secuencia, medición Hall),
    limpia el fallo, habilita motores y deja el modo en **Manual**. El modo se conserva al detenerse.
  - En Manual, `Afr2Product_PwmStatus` da OK: Conducir y el control táctil quedan habilitados. Cada consigna PWM vuelve a anular
    lo que estorbe.
  - **Siguen vigentes** STOP, el TTL de cada consigna y el corte de 1,5 s sin pulso Web.
  - Reposo, Línea, GOTO, prueba de rueda o secuencia borran la elección de Manual.
  - El vencimiento de consigna solo actúa con el auto en marcha.
- ATmega, Línea: habilita motores, detiene el radar y una tarea local en marcha, y después inicia el seguimiento (sigue exigiendo
  sensores IR legibles y Hall calibrado; si faltan, la Web muestra el motivo).
- Web: la opción Manual de la pill ya no depende de `manual_pwm`. El motivo de bloqueo de Conducir sugiere elegir «Manual» en la
  pill. ESP: el mismo agregado en el motivo de `manual_pwm`.

## Reinicios y desconexiones

**No se encontró la causa por lectura de fuente.** Hipótesis por revisar con una captura:

- En el ESP, `robot.mode` a Reposo publica el estado completo a todas las sesiones al mismo tiempo que la respuesta (`publishStatus()`).
  Con poca memoria libre, el ESP8266 puede cerrar al cliente WebSocket por cola llena, o reiniciarse (antecedente AFR2-20261109-08).
- En el ATmega, el ciclo «Manual → Reposo» por el error 1 se repetía en cada intento y cortaba el radar y el servo cada vez.

Hace falta el monitor serie del Loader (líneas `ESP BOOT`, `LINK lost`, `FAULT`) mientras se cambia de modo, para confirmar.

## Archivos

ATmega: `include/afr2_product.h`, `src/services/afr2_product.c`, `src/services/afr2_app_wire.c`, `src/services/afr2_app.c`,
`src/services/afr2_sequence.c`. ESP: `src/broker.cpp`. Web: `src/features/connection/RobotModePill.tsx`,
`src/features/control/ControlPage.tsx`, `package.json`/`package-lock.json`. Contracts: `contracts/afr2_build_info.h`, `docs/VERSIONADO.md`, CHANGELOG.
