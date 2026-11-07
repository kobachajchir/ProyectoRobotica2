# AFR2-20261106-33 - Loader 2.0.0 y control táctil con paneles y modelo fijo

Fecha: 06/11/2026. Parte del lote de 15 cambios; solo lo que figura abajo está hecho.

## Hecho (fuente modificada)
- **Cambio 9 Loader 2.0.0**: `JobState.cs`, `launcher/Launcher.cs`, `MANUAL.html`, `README.md`, `MCP.md`, `loaderFlow.ts` (Web); nuevo `Compilar-Loader-2.cmd`. `Preparar-Distribucion.ps1` toma la versión de `JobState.cs`. Loader compilado a pedido del usuario (sin errores, FileVersion 2.0.0.0).
- **Cambios 1 y 2**: «Información» y «Parámetros» (Hall + velocidad) del control táctil son `CollapsiblePanel`, estado recordado en localStorage (`afr2.touch.panels`).
- **Cambio 3**: el modelo 3D queda fijo; el giro se muestra con `ChassisSpinGraphic` animado (respeta prefers-reduced-motion), rotulado «orden enviada, no medida». Las ruedas siguen rodando según el giro ordenado.
- **Cambio 11 (parcial)**: el rumbo del control táctil se rotula «de la vista (no medido)». No hay rumbo estimado por Hall.
- Ya existentes antes de este lote: «Más opciones» (10), `resetReason` del ESP (7, parcial), servo.set/radar.configure extremo a extremo, seguimiento por `pointerId` (4).

## No hecho (pendiente)
- 4: renovación periódica y descarte de `pointercancel` espurios (solo existe el latido de ControlPage).
- 5, 6, 8, 11, 12 (ACK separado), 13: requieren leer y editar firmware ATmega sin compilar; no tocados.
- 7: causa de reinicios del ESP no confirmada (necesita captura física).
- 14: radar con barrido y 15 muestreo: el ATmega no expone hoy modos frecuencia fija/cantidad/variable (solo `intervalMs` 70..1000 en `robot.ultrasonic/radar.configure`), así que falta definir ese contrato antes de la Web.

Estado: contracts/atmega/esp=sin cambios; web=fuente editada, sin compilar; loader=compilado, no probado con hardware.

## Segunda tanda (ATmega, fuente editada, sin compilar ni cargar) - AFR2-20261106-34
- **Cambio 5 (pulsos por vuelta)**: causa probable, no reproducida: `Afr2Product_HaltMotion` borraba `hall_measuring/hall_delta` en cada detención de enlace (`Afr2Link_RxFault`, caducidad de 1500 ms) aunque el auto ya estuviera detenido; con la rueda girada a mano la pantalla quedaba en 0. Ahora solo aborta la medición si había movimiento (`was_stopped==0`). `src/services/afr2_product.c`.
- **Cambio 8 (OLED)**: (a) la secuencia de arranque/recuperación usa la rotación y el contraste configurados en vez de 0xA1/0xC8/0xCF fijos, evitando un cuadro girado tras un reinicio del bus (`afr2_oled.c`); (b) tope de refresco de 40 ms entre renders (`main.c`). Causa raíz de «titila con ultrasónico/notificación ESP» sin confirmar.
- **Cambios 11/12/13/14/15 y 4**: sin cambios. 13 necesita saber qué giro físico del encoder debe aumentar el ángulo; 12 (ACK separado) y 15 requieren definir contrato; 11: la odometría Hall ya integra `yaw_mrad`, falta exponerlo como «estimado (Hall)» en protocolo y Web.

## Compilación a pedido (06/11/2026)
- Web: `tsc -b` PASS (tras corregir un comentario JSX mal ubicado en `ManualDriveModel.tsx`). No se ejecutó `vite build`, ni empaquetado, ni promoción al ESP.
- ATmega: `mingw32-make -B firmware` con avr-gcc 7.3.0 (Atmel Studio 7), PASS sin advertencias de compilación; flash 126472 B (48,2 %), SRAM estática 5641 B (68,9 %, +10 B frente a 5631 B). No cargado; sin prueba física.
- ESP: no compilado (sin cambios de fuente en esta tanda).

## Tercera tanda - AFR2-20261106-35 (cierre del lote)
- **13 Sentido del servo (ATmega)**: a pedido del usuario, «el contrario del actual»: en la pantalla Servo/ángulo el encoder ahora resta (`angle - rotation`) en `afr2_screens.c`. La simulación Web (`uiSimulation.ts`) replica el mismo sentido. No cambia `pulse_for`, la opción «Invertir servo» ni el comando Web (0..180°).
- **12 Servo desde la Web**: `SensorsPage.tsx` muestra estados separados «Enviada → Recibida (ATmega aceptó) → Aplicada (telemetría con `pulseUs == targetUs`)» o «Error» con motivo (ocupado/barrido, fuera de rango, sin enlace, sin respuesta, o PWM no observado en 1,5 s). Sin cambio de protocolo: usa la respuesta de `robot.servo.set` y `headReading`.
- **11 Rumbo estimado por Hall**: con el auto conectado, el control táctil muestra `pose.thetaMrad` de `robot.state` (odometría Hall del ATmega, edad ≤ 2 s) como «Rumbo estimado (Hall)»; sin pose válida, «Rumbo no disponible · sin odometría Hall». Ya no se muestra la copia del giro táctil.
- **14 Radar con barrido**: `RadarScene.tsx` dibuja una estela detrás del haz, en el sentido de avance del ángulo real reportado, solo con barrido automático y telemetría fresca (sin datos, sin barrido; desactivada con prefers-reduced-motion). Los ecos se desvanecen hasta 30 % en 8 s según su recepción.
- **15 Muestreo HC-SR04**: nuevo `SamplingPanel.tsx` con modos período fijo, por cantidad y variable. Sobrescribe el intervalo del ATmega con `robot.ultrasonic.configure` (70..1000 ms, en RAM) y registra las muestras observadas en la telemetría con validez y estado; cuenta como «no observadas» las que salta el sondeo. El modo variable alterna mínimo/máximo según el cambio de distancia (reconfigura cada ≥500 ms). Se bloquea durante el barrido automático. Aclaración: el ATmega no tiene modos propios de cantidad/variable; la Web los arma sobre el intervalo existente. No hubo cambio de protocolo.
- **4 Dedos**: sin cambios adicionales; el seguimiento por `pointerId` y el latido de 180 ms de ControlPage ya existían.
- OLED: revisados los hashes de `afr2_oled.c` (solo transporte) y `afr2_screens.c` (entrada del encoder) en `firmwareRenderer.sources.json`.

Compilación a pedido: ATmega PASS (avr-gcc 7.3.0), flash 126480 B (48,2 %), SRAM 5641 B (68,9 %). Web `tsc -b` y `vite build` PASS. Web revisada en el navegador sin robot: paneles colapsables con estado recordado, panel de muestreo bloqueado sin conexión y estado «Orden de servo». Sin carga al ESP/ATmega ni prueba física.
Estado: atmega=fuente editada y compilada, sin cargar; esp=sin cambios; web=fuente editada y compilada, sin empaquetar al ESP.

## Cuarta tanda - AFR2-20261106-36 (dedos, cambio 4)
`useManualDrivePointers.ts` + `ManualDriveOverlay.tsx`:
- Un `pointercancel` táctil sobre el único dedo de arrastre abre una gracia de 250 ms: la orden actual no se suelta y un toque nuevo dentro de ese tiempo continúa el mismo vector (el inicio se desplaza para conservar la dirección). Vencida la gracia, se detiene como antes. El cancel durante un giro o con mouse/lápiz sigue deteniendo de inmediato.
- Al levantar un dedo de un giro de dos, el que queda pasa a arrastrar desde su posición actual (sin salto ni movimiento hasta superar el umbral) en lugar de vaciar el gesto.
- Seguimiento por `pointerId` y renovación de la orden con el latido de 180 ms de ControlPage (watchdog ATmega 1500 ms) sin cambios.
Verificación: `tsc -b` PASS; el hook se ejercitó en el navegador con eventos táctiles sintéticos (sonda temporal ya borrada): arrastre, cancel con gracia y reanudación (vector conservado), vencimiento de la gracia, giro y dedo restante arrastrando. No probado en un teléfono real.
Estado: web=fuente editada y compilada; atmega/esp=sin cambios.
