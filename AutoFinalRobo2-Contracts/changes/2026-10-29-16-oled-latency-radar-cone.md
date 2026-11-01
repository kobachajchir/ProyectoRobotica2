> **AFR2-20261029-16 compilado, NO cargado por indicación del usuario:** producto/comunicación/biblioteca exit0, sin errores/advertencias, final29/10/2026 12:15:28. Flash75518bytes; SRAM6918bytes, margen1274bytes; main259 y renderer73bytes según .su individuales, sin certificación dinámica de pila. COM8 conserva15 (cargada12:02:57). [Evidencia](../documentation/builds/2026-10-29-16/README.md). El usuario pidió continuar con otros cambios y posponer la carga; no se solicitará flasheo hasta nueva instrucción.

# AFR2-20261029-16 — Menor latencia OLED y cono visual del radar

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Usuario reporta impresión más lenta después de15 y solicita puntos rellenos,
sección observada continua y resto punteado, según posición del servo. La
consulta posterior pide que la franja corresponda a la apertura del HC-SR04,
en lugar de una división arbitraria de5grados.

## Transporte y latencia

Se elimina yield_pending/resume_ms y la pausa obligatoria de1ms tras cada
comando/datos. El main sigue atendiendo servicios/input/render antes del OLED;
el callback registra estado y nunca encadena envío. Task intenta como máximo
una transferencia por llamada, hasta16bytes de datos; I2C Manager mantiene
exclusividad y transporte IRQ no bloqueante. Si input/respuesta permanecen
pendientes, la oportunidad de avance se acota a2ms en lugar de5ms. No es un
límite absoluto si una vuelta de main tarda más o el bus está ocupado.

Un cuadro completo desconocido requiere64tramos: las128etapas tenían además
aproximadamente128ms de esperas artificiales, que se eliminan. Es una cuenta
teórica; no se mide tiempo físico de refresco en esta tarea. Bus100kHz,
startup500ms y backoff50ms ante fallo permanecen. La comparación exacta por
bytes/página y recuperación sin DISPLAY OFF de14 se conservan. Se eliminan
5bytes del handle por quitar reloj y flag de espera.

## Geometría y significado del radar

La [ficha HC-SR04 de ElecFreaks distribuida por SparkFun](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf)
indica MeasuringAngle15degree. La ficha no aporta aquí una calibración del
haz de este módulo ni determina una posición lateral del eco. Para la
presentación se adopta un **sector visual nominal de15grados totales**
(+/-7.5), recortado al semicírculo0..180. Es una convención gráfica explícita,
no se afirma apertura física exacta ni forma del patrón de sensibilidad.
El ancho del auto corresponde al corredor de paso y no define el haz acústico.
No se inventan dimensiones del auto ni se modifica evitación de obstáculos.

- Grilla de cuatro arcos punteados, separación aproximada3pixels entre puntos.
  Sección activa de arcos, radios interiores y bordes del cono continuos.
- Centro usa head.angle, que Sensors_Task calcula del PWM aplicado1000..2000us
  con inversión configurada. No usa target_us. No hay feedback mecánico.
  Servo deshabilitado/PWM desconocido: sin cono activo, indicador OFF.
- Interpolación de la LUT existente en medias unidades de grado permite bordes
  +/-7.5 sin agregar tabla flash. Productos32bits y Bresenham acotado de15.
- Cada eco válido20..4000mm se representa por disco relleno de radio2pixels;
  bins conservan ángulo de adquisición. También se dibuja la última muestra
  manual si tiene ángulo/PWM de captura válidos. No se reubica una muestra
  histórica usando la posición nueva del servo. Error/timeout no dibuja punto.
  El punto es una proyección por el eje de lectura; no localiza el objeto
  lateralmente dentro del cono ni estima su ancho.

## Adaptaciones por archivo y vinculación

- Atmega/include/afr2_oled.h y src/display/afr2_oled.c: quita la espera/estado
  artificial; confirmación ACK, errores, snapshot TX y prioridad cooperativa.
- Atmega/src/main.c: oportunidad bajo carga2ms; no cambia orden de servicios.
- Atmega/src/ui/afr2_sensor_screens.c: grilla/cono/discos nominales, PWM aplicado,
  muestra manual histórica y LUT interpolada. Renderer puro, sin sensores/I2C.
- Web/src/features/oled/firmwareSensorRenderer.ts: mismos enteros, clipping,
  geometría y bucles acotados. latest.angle/pulseUs opcionales para snapshots
  antiguos: si faltan no se inventa posición de esa muestra. Bins disponibles
  mantienen sus coordenadas explícitas. Visor vivo sigue framebuffer AVR.
- Web/src/features/oled/firmwareRenderer.sources.json: fingerprints revisados
  del transporte y renderer después de portar comportamiento.
- Contracts/documentation/screens.json: presentación0x030031 registrada.
  Sin nuevo ScreenCode ni cambios de navegación/acciones, recursos o wire.
- Informes/README y nota de contrato8: estado y límites. Autorías preservadas.

Se conserva EEPROMv3 y Hall confirmado de15, calibraciones y control de motores.
No se ejecutan tests, validadores ni ensayos físicos por el agente. Compilar y
cargar AVR COM8 sigue la autorización del usuario al terminar. Estado inicial:
fuentes AVR/Web completas; build/carga pendientes. Web no compilada/promovida;
ESP/Loader sin cambios. Los resultados de build, memoria y carga se incorporan
al finalizar; escritura verificada no prueba desempeño ni detección física.
