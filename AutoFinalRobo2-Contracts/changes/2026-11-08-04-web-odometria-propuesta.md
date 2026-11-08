# AFR2-20261108-04 - Prueba de odometría propuesta desde la Web y confirmada en el auto

Pedido del usuario: en la Web, parte de odometría, un selector de prueba con dirección y distancia; al enviar el comando
aparece la pantalla en el auto; con el encoder se inicia (clic) o se descarta (pulsación larga), y cualquiera de las dos
avisa a la Web si la prueba se lanzó o no. Solo fuente: sin compilar, flashear ni probar con el auto.

## Diseño (sin trama nueva)

Reutiliza `EX01 CONFIG` (0x1A, op 0 leer / op 1 escribir), así que no cambian `AFR2_EXT_VERSION`, el formato de tramas ni
las validaciones de respuesta del ESP (13 bytes). Solo se amplía el rango de acciones del broker del ESP de 0..87 a 0..103
y el ATmega sigue siendo quien valida cada acción (`calibration_action`).

| Acción | Operación | Valor |
|---|---|---|
| 102 `AFR2_A_ODOM_PROPOSE` | escribir (`robot.calibrate`) | `(token<<16) \| (dirección<<8) \| cm`; token 1..32767, dirección 0..7 (códigos de `AFR2_A_MOVE`), cm 5..100 de a 5 |
| 103 `AFR2_A_ODOM_STATE` | leer (`robot.config.get`) | `(token<<8) \| estado` |
| 104 `AFR2_A_ODOM_ANSWER` | solo local (OLED) | 1 iniciar, 0 descartar; no accesible por CONFIG ni por la UI remota |

Estados: 0 ninguna, 1 esperando, 2 lanzada, 3 descartada, 4 no se pudo lanzar, 5 vencida (60 s sin respuesta).
El estado 3 también cubre "se abandonó la pantalla" y "se perdió el enlace con la Web".

## Flujo

1. La Web (Control > Calibración > *Prueba de odometría*) envía la acción 102. El ATmega la acepta solo con salidas
   apagadas, sin fallo, sin EEPROM ni medición Hall en curso, sin teclado/edición/control táctil/Sí-No/prueba Hall/encoder ni
   "Prueba en curso" abiertos, y con los mismos requisitos de GOTO (motores habilitados, Hall, geometría, pose válida, sin radar).
   Si no, devuelve no disponible/ocupado y la Web lo muestra. Si acepta, guarda dirección y distancia y abre
   `AFR2_N_ODOM_ASK` (nodo 73, código 0x03004A) sobre lo que estaba en pantalla.
2. La pantalla del auto muestra dirección y distancia, «Iniciar» (clic) y «Descartar» (largo).
   - Clic: inicia el GOTO relativo (estado 2), cierra la confirmación y abre «Prueba en curso» (0x030049). Si el inicio
     falla, avisa en el auto el motivo y el estado queda en 4.
   - Largo al soltar: estado 3, aviso «Prueba descartada» y vuelve a la pantalla anterior.
   - Sin respuesta en 60 s: estado 5, aviso «Prueba vencida».
   La rotación del encoder no hace nada en esta pantalla. El clic remoto de la vista OLED de la Web está bloqueado: solo
   inicia el encoder físico.
3. La Web consulta la acción 103 cada 700 ms (hasta 75 s) y compara el token; al llegar un estado distinto de 1 muestra el
   resultado. No hay evento push desde el auto.

## Archivos

- ATmega: `afr2_menu_catalog.h/.c` (nodo 73, acciones 102–104), `afr2_app.h/.c` (estado, `odom_start` compartido con el menú,
  propuesta/respuesta, vencimiento, retiro del enlace, líneas de la vista), `afr2_screens.c/.h` (pantalla, clic, largo,
  nombres de dirección compartidos), `afr2_app_wire.c` (acciones permitidas por CONFIG, UI remota bloqueada en 0x03004A).
- ESP: `broker_product.cpp` (rango de `robot.calibrate`/`robot.config.get` hasta 103).
- Web: `features/control/OdometryTestPanel.tsx` (selector, envío, consulta), `ControlPage.tsx` (en la pestaña Calibración),
  `firmwareRenderer.ts` (pantalla 0x03004A; el nodo virtual del selector de motor pasa de 73 a 74),
  `firmwareLibrary.ts` regenerado con `sync-oled-library.mjs --copy-only` y hashes de `firmwareRenderer.sources.json`.
- Contracts: `ATMEGA_CONFIG_ACTIONS.json` (99, 100, 102, 103 con `config_access: false`), `screens.json` (0x03004A), CHANGELOG.

## Estado y límites

atmega=fuente editada, sin compilar ni cargar; esp=fuente editada, sin compilar ni flashear; web=fuente editada, sin
build ni navegador. Un ESP o ATmega con firmware anterior rechaza las acciones 102/103 (la Web mostrará el error): hay que
actualizar ambos. SRAM/flash sin medir (+9 bytes en `Afr2App`). El simulador del desarrollador no modela estas pantallas.

## Compilación (08/11/2026, sin cargar ni probar en el auto)

Cubre también AFR2-20261108-02 y -03, que están en el mismo árbol.

- ATmega (avr-gcc 7.3.0, `make firmware`): compila sin errores; un único aviso de estilo ya existente
  (`-Wmisleading-indentation` en `afr2_product.c`, `odometry`). Flash 143 792 B (.text + .data), SRAM estática 6 754 B
  (.data 344 + .bss 6 410), unos 1 438 B libres para la pila; sin certificación dinámica de pila. `build/afr2.hex` de 404 475 B.
- ESP (PlatformIO `esp12s_candidate`): SUCCESS. RAM 62,0 %, flash 43,6 % (455 323 B de 1 044 464 B). `firmware.bin` generado, sin flashear.
- Web: `sync-oled-library.mjs` en modo estricto, `tsc -b` y `vite build` sin errores. No se ejecutaron `postbuild`
  ni `build:esp`, así que no se empaquetó ni se copió al ESP. Sin prueba en navegador.
- Hashes de `firmwareRenderer.sources.json`: la última actualización fue por un cambio solo de comentario en `afr2_screens.c`.

## Seguimiento AFR2-20261108-05 (informe de uso: «Interrumpido», motores bloqueados y Sí/No bloqueado)

El usuario probó la prueba y reportó: se mueve un poco y se detiene con «Interrumpido», los motores vuelven a quedar
bloqueados y, al volver atrás, el diálogo «Guardar cambios» no deja elegir ni salir. **Causas no confirmadas.** Cambios:

- El aviso al terminar una prueba con `fault` distinto de 0 ahora dice la causa («Fallo de marcha» / `F4 signo PWM/Hall`, `F3 sin pulsos`,
  `F2 odometria`, otro) y se escribe `ODOM interrupted fault=.. hp=..` en el log USART0. Hipótesis principal: fallo 4
  (PWM/Hall de signo cruzado, que detiene el lazo cerrado a los ~300 ms). Los 370 pulsos de fábrica no fijan el sentido CW
  por rueda (`hall_cw_mask` sigue en 0 si nunca se calibró), así que conviene revisar «HP:» en Motores y velocidad y calibrar el sentido en Calibración > Hall.
- Sí/No: un aviso encolado ya no traga el giro (giro o clic lo cierran), «Guardando…» vence a los 5 s con «EEPROM / No guardado»
  y el log registra cada entrada y resultado (`CONFIRM in …`, `CONFIRM save|discard result=…`).
- Compilado de nuevo el ATmega: sin errores ni avisos nuevos; SRAM estática 6 772 B. Sin cargar ni probar.

## Corrección del fallo 4 (AFR2-20261108-05): referencia Hall de fábrica

El usuario confirmó la causa: en crudo, al girar hacia adelante las ruedas derechas cuentan **negativo** y las izquierdas
**positivo**; una vuelta CW exterior da −370 en las cuatro. El fallo 4 era correcto: con `hall_signs = 0` y `hall_cw_mask = 0`
las izquierdas daban avance positivo, contrario a la base cinemática (avance con velocidades internas negativas).

- Valores de fábrica: `AFR2_DEFAULT_CPR` = 370 y `hall_signs = hall_cw_mask = 0x0F` (`AFR2_FACTORY_HALL_MASK`), que es lo que dejaría la
  calibración Hall completa en cada rueda con esa referencia (signo invertido para que CW sea positivo, derechas transformadas por la odometría).
- Migración en RAM al cargar la EEPROM, solo para ruedas sin calibrar (CPR 0 y sin bit CW): CPR 370 y los dos bits. Las
  ruedas ya calibradas no cambian. No se autoguarda; al guardar en Preferencias queda persistida.
- Consecuencia visible: tras este ajuste, «Prueba Hall» y «Estado Hall» muestran la convención exterior (derechas
  positivas hacia adelante, izquierdas negativas), no el conteo crudo. Recalibrar una rueda desde Calibración > Hall da el mismo resultado.
- ATmega recompilado sin errores (aviso de estilo previo en `afr2_product.c`); SRAM estática 6 772 B. Sin cargar ni probar.
