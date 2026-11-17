# AFR2-20261117-12 - Visor de seguimiento a pantalla completa

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.0 (subversión).

Estados:
- web = en fuente;
- atmega = solo versión;
- esp = solo versión.

Sin cambio de contrato: usa `robot.mode` con `start` de AFR2-20261117-11, `robot.stop`, `robot.calibrate` 6/7 y `robot.line.calibrate`.

## Pedido

Un visor de seguimiento que se abra como el control táctil, con la misma interfaz inmersiva y la misma disposición:
- la animación del auto sobre la cinta, con los sensores, a pantalla completa;
- abajo, los botones Pausar, Detener e Iniciar movimiento;
- botones laterales como los del control táctil donde hagan falta.

## Cambios (Web)

- `src/features/lineFollower/LineFollowerOverlay.tsx` (nuevo). Usa la misma estructura y las mismas clases del control táctil (`ManualDriveOverlay`):
  - `FloatingModal` a pantalla completa con antetítulo «Visor de seguimiento», título «Seguimiento de línea» e indicador Conectado/Desconectado;
  - escenario `afr2-manual-stage` con `LineFollowerLiveView` (modo `fill`);
  - pie `afr2-manual-footer` con el estado grande y los mandos;
  - riel lateral `afr2-manual-rail` con hojas `FloatingSheet` contenidas.
  - **Mandos:**
    - **Iniciar movimiento / Reanudar:** `robot.mode { line-follow, start: true }`.
    - **Pausar:** STOP confirmado y `robot.mode { line-follow }` sin `start`. Queda armado y quieto, con velocidad y Kp en RAM.
    - **Detener:** STOP confirmado y `robot.mode idle`.
    - Mismos requisitos que el panel para iniciar: enlace, seguidor y Mecanum implementados, odometría calibrada y motores habilitados. Si falta alguno, lo informa.
  - **Estado:**
    - «Siguiendo · <posición>», «En pausa · listo para reanudar», «Seguidor armado», «Detenido» o «Vista previa · simulación local»;
    - orientación de la línea cuando hay lectura IR;
    - lecturas IR cada 250 ms con el visor abierto.
  - **Riel:**
    - Información;
    - Parámetros: velocidad 10–500 mm/s, Kp 0–1000 y ancho de la cinta (`LineTapeField`), con Consultar y Aplicar en RAM;
    - Calibrar: fondo, línea y confirmar.
    - Los parámetros se bloquean con el auto en marcha.
  - Sin auto, la vista es la simulación local y los mandos quedan deshabilitados.
- `src/features/control/LineFollowerLiveView.tsx`: prop `fill`, que ocupa todo el contenedor.
- `src/styles/line-follower.css`, modo `fill`:
  - sin borde y con altura 100 %;
  - el HUD baja bajo el encabezado;
  - se oculta la leyenda inferior, porque el pie ya muestra el estado;
  - el escenario del visor no usa el cursor de arrastre;
  - mandos centrados, en tres columnas en celular.
- `src/features/lineFollower/LineFollowerPage.tsx`: botón «Abrir visor de seguimiento» debajo de la vista.
- `.claude/launch.json`: configuración `afr2-web-vite` (Vite en el puerto 5175 sin `predev`), para revisar la Web mientras el control de huellas del OLED está bloqueado por cambios en curso de otra sesión en `afr2_screens.c`.

## Estado de validación

- Revisado en Vite (5175), sin el auto, en escritorio y a 375 px:
  - abre a pantalla completa con la disposición del control táctil;
  - el auto zigzaguea sobre la cinta con IR1…IR5;
  - la hoja de Parámetros se abre desde el riel;
  - los mandos quedan deshabilitados sin auto;
  - sin errores en consola ni desplazamiento horizontal.
- `npm run dev` no arranca por el control de huellas del OLED (cambios en curso de AFR2-20261117-10/11 en `afr2_screens.c`, ajenos a este cambio).
- Sin compilar.
- No probado con el auto: pausa, reanudación ni detención reales.

## Pendiente

- Compilar la Web cuando la otra sesión cierre la revisión de huellas del OLED.
- Probar con el auto:
  - Iniciar → Pausar: queda armado y quieto.
  - Reanudar → Detener: pasa a Reposo.
