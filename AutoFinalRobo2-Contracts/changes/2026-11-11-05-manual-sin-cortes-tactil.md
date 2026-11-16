# AFR2-20261111-05 · Conducción manual sin cortes por demoras y panel táctil que no se cierra

Versión: 1.26.2 (subversión mínima: corrección, sin función nueva).
Estado: **atmega** fuente editada, sin compilar ni cargar; **web** fuente editada, sin `tsc` ni navegador; **esp** solo versión.

## Informe del usuario y captura

Al mover el auto con el control táctil aparecía el error de lazo (F1), STOP a cada rato y el panel táctil se cerraba. Con las ruedas en el
aire funciona bien; apoyado en el suelo, con vibración, falla. El usuario sospecha ruido en los Hall u otros sensores.

Monitor serie de COM8 (Loader, modo `tap=log`, 115200), del segundo 1720 al 1885 del ATmega, con el control táctil en uso:
- Sin `FAULT`, `LOOP slow` ni `LINK lost`. Lazo máximo de 16 a 36 ms, entre 200 y 250 órdenes del ESP cada 5 s, sin rechazos ni fallas de UART.
- Un corte del WebSocket (`ws-close`/`ws-open` en 1782,8 s) **sin** reinicio del ESP. `espRst=3` desde el arranque del ATmega, con los tres reinicios anteriores a la captura.
- Pila mínima libre del ATmega: 366 B.

En esa ventana no se reprodujo el fallo con el auto apoyado: la causa física no quedó medida.

## Cambios

- **ATmega** (`afr2_product.c`), modo PWM manual (Conducir, táctil o botones; lazo abierto):
  - una demora del lazo de más de 100 ms ya no corta las salidas ni da F1: se registra `LATE manual gap= loop= render= (sin corte)` en el log y sigue;
  - un salto de cuenta Hall por ruido (más de 100 000 cuentas en un ciclo) solo invalida la pose, sin F2.
  - Las salidas las siguen cortando STOP, el TTL de 400 ms de cada consigna (si la Web deja de mandar, el auto para) y el corte de 1,5 s sin
    pulso Web. F3 y F4 ya no aplicaban al PWM manual.
  - El resto de los modos (GOTO, línea, secuencias, pruebas) conserva las fallas como estaban.
- **Web** (`ControlPage.tsx`, `ManualDriveOverlay.tsx`):
  - una consigna rechazada por el auto (`CAPABILITY_UNAVAILABLE`, `SUPERSEDED` o `BAD_REQUEST`) se muestra en un toast y ya no lanza STOP;
    el panel táctil sigue abierto; ante CAPABILITY_UNAVAILABLE/BAD_REQUEST se conserva la consigna y se reintenta mientras se mantiene el gesto. SUPERSEDED termina la consigna local para no reanudar una orden anterior a STOP;
  - si se pierde la sesión táctil con el auto, el panel ya no se cierra: avisa con un toast y se reconecta solo.
  - El error de presentación OLED no suelta el gesto. Los errores de consigna se muestran también en un aviso dentro del modal, visible sobre su capa nativa; no cancelan el gesto por cambiar el texto del error.
  - Un estado `fault` no cancela por sí mismo el gesto táctil; el permiso real de PWM sigue viniendo del controlador. Misiones y pruebas de rueda conservan la cancelación por toma de control.
  - Un rechazo de la consigna cero avisa sin encadenar un STOP. Los errores no recuperables terminan el envío local, sin generar STOP repetidos. STOP explícito, pérdida de foco/enlace, TTL y supervisión de comunicación siguen vigentes.
  - Los giros por ángulo con Hall, si el usuario los activa, siguen siendo GOTO en lazo cerrado; el desplazamiento manual y el giro sostenido usan PWM abierto.

## Riesgo aceptado por el usuario

Con el auto en PWM manual, un lazo trabado más de 100 ms ya no corta los motores al instante. El corte queda a cargo del TTL de la consigna
y del corte de 1,5 s sin pulso Web, y dependen de que el lazo vuelva a correr.

## Pendiente

Compilar y cargar, y repetir la conducción con el auto apoyado. Si vuelve a fallar, buscar `LATE manual` y `NOISE irq storm` en el log para
confirmar que el origen es el ruido de los Hall.

## Continuación de la misma corrección

Se completaron las rutas Web pendientes de esta ficha, conservando 1.26.2. Autorías anteriores preservadas. Adaptaciones por archivo:

- `AutoFinalRobo2-Web/src/features/control/ControlPage.tsx`: reintento de rechazos PWM con gesto sostenido, diagnóstico compartido con el táctil y eliminación del STOP encadenado por errores de consigna/cero.
- `AutoFinalRobo2-Web/src/features/control/manual/ManualDriveOverlay.tsx`: aviso local sobre el modal; errores de presentación/consigna y estado fault sin cancelar el gesto. El cierre explícito conserva su comportamiento.
- `AutoFinalRobo2-Contracts/CHANGELOG.md` y `docs/VERSIONADO.md`: registro de 1.26.2 que faltaba; esta ficha describe la continuación sin nueva versión.

La hipótesis de ruido por vibración no está confirmada. No se ejecutaron tests, compilaciones, validadores, navegador, puertos ni cargas en esta continuación. ATmega conserva la fuente previa de esta ficha; ESP conserva solo la versión compartida.

La corrección lateral de AFR2-20261111-02 ya está en fuente: `AFR2_LATERAL_ODOM_PERMILLE=775`, aplicada solo a `dy` en ATmega. No acredita que el firmware instalado incluya ese cambio ni que una repetición física dé 1 m.
