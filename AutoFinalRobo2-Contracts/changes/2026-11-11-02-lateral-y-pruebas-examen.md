# AFR2-20261111-02 · Escala lateral de la odometría y menú «Pruebas examen»

Versión: 1.25.0 (subversión: función nueva compatible; incluye una corrección de calibración).
Estado: **atmega** fuente editada, sin compilar ni cargar; **esp** solo versión; **web** fuente editada (copia de las
pantallas), sin `tsc`, build ni navegador. Sin CMD nuevo ni cambio de EEPROM.

## 1. Lateral a 77,5 cm por cada metro

Informe del usuario: con la Web, las pruebas de 1 m adelante y atrás terminan exactamente en 1 m. Las de izquierda y derecha
terminan en 77,5 cm, igual en cuatro repeticiones. Pidió no tocar adelante ni atrás.

- Causa: en desplazamiento lateral los rodillos mecanum patinan. Las ruedas cuentan más recorrido del real y la odometría
  creía haber llegado a 1000 mm con 775 mm reales.
- Cambio: `AFR2_LATERAL_ODOM_PERMILLE 775` en `contracts/afr2_geometry.h`. `odometry()` (`afr2_product.c`) multiplica solo la
  componente lateral (`dy`) por 0,775. Con la odometría corregida, el GOTO sigue empujando hasta los 1000 mm estimados, que
  equivalen a 1000 mm reales.
- **No se tocó**: el avance/retroceso (`dx`), el giro (`radius_sum_mm`), los pulsos por vuelta y el mando de motores. Las
  diagonales llevan la parte lateral corregida.
- Es una constante de firmware, no un ajuste de EEPROM. Si con el piso o la carga cambia el patinamiento, se vuelve a medir y se
  reemplaza el valor.

## 2. Pruebas examen (OLED)

Nuevo primer renglón de **Pruebas**: «Pruebas examen» (0x03004C) → «Desplazamiento 1 m» (0x03004D) / «Giros» (0x03004E) / Volver.

| Lista | Opciones | Orden |
|---|---|---|
| Desplazamiento 1 m | Adelante, Atrás, Izquierda, Derecha | un GOTO relativo de 1000 mm a la velocidad límite configurada |
| Giros | 90° derecha (horario), 90° izquierda, 180°, 360° (antihorario) | giros sobre el centro en tramos GOTO de hasta 90°, encadenados hasta que el giro acumulado llega a la meta (tolerancia 25 mrad, como mucho 8 tramos) |

- Requisitos para iniciar: motores habilitados, sin falla, pose válida, auto detenido y sin radar. Si falta algo, aparece el
  aviso habitual con el motivo.
- Pantalla **«Ejecutando test»** (0x03004F), con refresco de 100 ms:
  - desplazamiento: «Despl. Adelante», `X: … mm` y `Y: … mm` desde 0, en el sistema del auto al partir (X adelante, Y izquierda);
  - giro: «Giro 90 der», `Angulo: … grados`, que sube desde 0 con el giro acumulado que estima el auto, y `Meta: … grados`.
- Al terminar el título pasa a **«Prueba terminada»**. Si se corta, dice «Prueba detenida» (pulsado) o «Interrumpida Fn»
  (falla o cancelación), y los valores quedan a la vista.
- **La pantalla queda hasta OK con el encoder.** Mientras ejecuta, corto o largo detienen el ensayo. Después, corto o largo es
  OK y vuelve al menú del examen. El aviso genérico «Operación lista» del GOTO no se muestra durante el examen.
- La UI remota de la Web no puede pulsar en esta pantalla. Salir de ella por cualquier camino detiene el examen.
- Acciones locales nuevas: `AFR2_A_EXAM_START = 111` (arg 0..3 / 10..13), `AFR2_A_EXAM_STOP = 112`, `AFR2_A_EXAM_CLEAR = 113` y
  `AFR2_A_EXAM_STATE = 114`. Ninguna es accesible por CONFIG ni por la UI remota.
- Log USART0: `EXAM arg= phase= turned= x= y= fault=` al terminar cada examen.

## Archivos

- ATmega: `include/afr2_menu_catalog.h`, `src/ui/afr2_menu_catalog.c`, `include/afr2_app.h`, `src/services/afr2_app.c`,
  `src/ui/afr2_screens.c`, `src/services/afr2_app_wire.c` y `src/services/afr2_product.c` (escala lateral).
- Contracts: `contracts/afr2_geometry.h`, `contracts/afr2_build_info.h`, `documentation/screens.json` (0x03004C–0x03004F; Pruebas pasa
  a 9 filas), `docs/VERSIONADO.md` y CHANGELOG.
- Web: `firmwareRenderer.ts` (Pruebas con 9 filas, listas del examen, pantalla de ejecución; el nodo virtual del selector de
  motor pasa de 75 a 79). `generated/firmwareLibrary.ts` se regeneró con `sync-oled-library.mjs --copy-only` y se actualizaron los
  hashes de `firmwareRenderer.sources.json` después de portar el dibujo. `package.json`/`package-lock.json` en 1.25.0.

## Pendientes

- Compilar el ATmega y la Web, cargar el auto y repetir el lateral de 1 m (debería dar 1 m).
- Los giros del examen todavía no se midieron afuera: el ángulo que muestra es el que estima la odometría (`radius_sum_mm` nominal).
- El simulador del desarrollador no modela el examen.
