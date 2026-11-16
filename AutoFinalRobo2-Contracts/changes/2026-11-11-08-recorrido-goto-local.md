# AFR2-20261111-08 · Recorrido GOTO completo en el auto, avance en vivo y correcciones de ensayo

Versión: 1.28.0 (subversión: comandos y función nuevos, compatibles).
Estado: **atmega**, **esp** y **web** compilados sin errores (11/11/2026); sin cargar ni probar en el auto.

## Pedido

- Un ensayo enviado sigue aunque se caiga el WebSocket, la Wi-Fi o lleguen avisos; al terminar, el auto entrega el informe.
- El GOTO se manda íntegro al auto y corre en local. Pantalla que muestre en qué etapa va; la Web valida cada nodo,
  mueve el modelo 3D con la pose informada y traza la trayectoria real sobre la esperada.
- Giro: se pasaba 2,5° cada 90° hacia el lado del giro (10° en 360°).
- Pruebas mínimas: aclarar en los campos de medición la dirección, medida desde el frente del auto (HC-SR04).
- GOTO: deshacer/rehacer con iconos en la fila de herramientas; la ayuda abajo, como subtítulo. Zoom hasta 250 %.
  Spinners donde se espera el modelo 3D; control táctil habilitado cuando el modelo está descargado.

## Comandos nuevos (EX01, `afr2_product_wire.h`, `wire_catalog.json`, `commands.json`, Protocol Studio)

| ID | Nombre | Pedido | Respuesta |
|---|---|---|---|
| 0x46 | ROUTE_BEGIN | reserved u32=0, token u32, count u8 (1..32), speed u16 mm/s | token, count |
| 0x47 | ROUTE_LEG | reserved, token, index u8, x i16 mm, y i16 mm, theta i16 mrad | token, index, loaded |
| 0x48 | ROUTE_START | reserved, token | token, state |
| 0x49 | ROUTE_STATUS | token u32 | token, state, count, leg, done, fault, x i32, y i32, yaw i32 (marco de partida), elapsed u32 ms |

Estados: 0 idle, 1 loading, 2 ready, 3 running, 4 completed, 5 cancelled, 6 failed. No son sensibles. Web: `robot.route.*`.

## ATmega

- Los tramos se guardan en el búfer de secuencia (unión, libre mientras no hay secuencia). Cada tramo apunta al objetivo
  **acumulado** en el marco de partida, así los errores de un tramo no se suman al siguiente; los tramos ya dentro de la
  tolerancia se saltan. Origen local, tope por tramo 62 s.
- `Afr2Product_LocalMission()`: `MOTION_HALT` (cierre del WebSocket), la caída del enlace UART y la revocación de la Web
  no detienen un GOTO local. F1 (lazo > 100 ms) se registra y no corta la misión local; F3/F4 y STOP sí cortan.
- Pantalla OLED: la vista «Ejecutando test» (EXAM_RUN, variante 20) muestra «Recorrido GOTO», «Tramo k/N», X/Y mm y giro;
  al terminar «Recorrido listo». La copia Web la dibuja desde el mismo snapshot.
- Giro: `AFR2_TURN_ODOM_PERMILLE 1028` (92,5/90) como valor por defecto de los factores CW y CCW: la odometría cuenta más
  ángulo y el auto gira 2,5° menos por cada 90°. Las EEPROM v12 ya guardadas conservan sus factores; se ajustan en
  Pruebas examen → Escala de factores.

## ESP

- Codifica/decodifica `robot.route.*`, valida largo y token de cada respuesta; ROUTE_START cuenta como entrada del usuario.

## Web

- `useGotoRouteRunner`: carga el recorrido una vez y luego lee `robot.route.status` cada 300 ms, tolerando lecturas perdidas
  y cortes; al reconectar retoma. El panel GOTO marca nodos completados, mueve el modelo con la pose real y dibuja la
  trayectoria real (rastro) junto a la planificada.
- Pruebas mínimas: envían el ensayo como recorrido; la odometría final y la duración salen del informe del auto. Los campos
  dicen x adelante (+)/atrás (−), y izquierda (+)/derecha (−), ψ antihorario (+), con una guía por ensayo.
- Deshacer/rehacer como iconos; ayuda como subtítulo; zoom hasta 250 %; `ModelSpinner`; control táctil espera el modelo.

## Pendiente / sin verificar

- Nada compilado ni cargado. Falta medir en el auto el giro con 1,028 y un recorrido con corte de WebSocket a mitad.

## Compilación (11/11/2026, sin cargar ni probar en el auto)

- ATmega (avr-gcc 7.3.0, `make firmware`): sin errores; solo el aviso de estilo ya existente (`-Wmisleading-indentation`
  en `afr2_product.c`, `odometry`). Flash 166 598 B (.text 166 244 + .data 354); SRAM estática 7 174 B (.data 354 +
  .bss 6 820), unos 1 018 B libres para la pila. `build/afr2.hex` generado.
- ESP (PlatformIO `esp12s_candidate`): SUCCESS. RAM 57,6 %, flash 44,7 % (467 067 de 1 044 464 B). Sin flashear.
- Web: `npm run build` (tsc -b + vite) y `npm run build:esp` sin errores; paquete esp de 23 archivos copiado a
  `AutoFinalRobo2-ESP/data/dist`. Sin prueba en navegador.
