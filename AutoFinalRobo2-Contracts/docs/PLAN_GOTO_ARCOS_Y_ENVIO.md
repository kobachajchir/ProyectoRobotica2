# Listado para implementar: GoTo en la Web (arcos, curvas y envío al auto)

Creado el 08/11/2026 (AFR2-20261108-02). Es un listado de trabajo; no está implementado.

## Punto de partida (verificado en el código)

- Web: `features/control/goto/` arma una lista de comandos (`move` con dirección cardinal y distancia, `point` X/Y) y la
  **simula localmente** (`gotoSimulation.ts`). No se envía al auto.
- Auto: `AFR2_EXT_GOTO` (0x16) recibe **un** objetivo relativo al rumbo inicial (X, Y mm; opcional giro y velocidad).
  La ley de control es proporcional con velocidad mínima, tolerancia 10 mm / 25 mrad y límite de 60 s. Necesita motores
  habilitados, Hall/geometría y pose válida. Las secuencias (`EX01 SEQ_*`) admiten 32 pasos por rueda, no trayectorias.
- Odometría por ruedas: sin giroscopio, la deriva de rumbo crece con la distancia. La prueba de distancia de
  Calibración > Odometría (AFR2-20261108-02) sirve para medirla antes de confiar en curvas largas.

## 1. Arcos alrededor de un punto

1. Modelo `arc`: centro (X, Y), radio, ángulo inicial, barrido (grados, con signo) y sentido; vista previa en el mapa.
2. Convertir el arco a segmentos: cuerda de largo fijo (un paso cada 5-10 grados) con el error de aproximación visible.
3. Dos variantes: el auto rota mirando al centro (mecanum: traslación tangencial + giro) o mantiene el rumbo y solo traslada.
4. Validar límites: radio mínimo según la geometría (`radius_sum_mm`), alcance ±5000 mm del GOTO actual, velocidad ≤ límite.
5. Probar en el simulador con el mismo planificador antes de enviar.

## 2. Curvas a seguir

1. Trayectoria por puntos de paso (waypoints) con tipo de unión: recta, arco o curva suave.
2. Curva suave (spline/Bézier) discretizada en segmentos cortos y limitada por curvatura máxima.
3. Perfil de velocidad: aceleración/frenado en los extremos y reducción en curvas.
4. Editar arrastrando sobre el mapa, con deshacer y lista ordenable (reusa el editor de secuencias).

## 3. Envío al auto

1. Decidir el mecanismo (mayor riesgo, primero): **A** la Web envía un segmento por vez con `AFR2_EXT_GOTO` y espera el
   resultado (sin cambios de firmware); **B** nueva extensión `EX01` para subir una trayectoria acotada (hasta N puntos,
   CRC, comenzar/cancelar/estado) que el ATmega recorre solo, al estilo de `SEQ_BEGIN/APPEND/COMMIT/START`. B exige ficha
   AFR2, presupuesto de SRAM y versión de extensión.
2. Estados visibles en la Web y en la OLED: cargando, lista, en curso (segmento i/N), completa, cancelada, fallo.
3. Parada y cancelación inmediatas (STOP, pérdida de enlace, largo del encoder) y vencimiento por segmento.
4. Rechazos claros: sin pose válida, motores bloqueados, radar activo, fallo 4 (signo PWM/Hall cruzado).
5. Pantalla OLED de "trayectoria en curso" con el mismo Sí/No y retorno por largo del resto de la UI.

## 4. Otras funciones de GoTo para considerar

- Objetivo con rumbo final (ya existe `theta` en el comando; falta en la Web) y giro en el lugar.
- Volver al origen / "Ir a casa" y fijar la posición actual como (0, 0).
- Repetir la lista N veces o en ciclo; pausar y reanudar.
- Guardar y cargar trayectorias con nombre (Web: almacenamiento local; auto: solo la activa).
- Informe de error al llegar (posición estimada contra objetivo) para comparar con la medición real de la prueba de odometría.
- Evitar obstáculos con el HC-SR04: hoy es solo telemetría (AFR2-20261106-13); decidir si se vuelve condición de parada.
- Corrección de deriva con la barra IR al cruzar una línea de referencia.
- Exportar/importar trayectorias en un archivo.

## 5. Orden sugerido

1. Medir la deriva con la prueba de distancia (Calibración > Odometría) y ajustar los pulsos por vuelta.
2. Envío por segmentos (A) con la lista actual, para validar el enlace y la parada.
3. Arcos con vista previa y luego curvas con waypoints.
4. Trayectoria completa en el auto (B) solo si A resulta lenta o poco confiable.
